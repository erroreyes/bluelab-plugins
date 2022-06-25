#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <vector>
#include <deque>
#include <fstream>
#include <string>
using namespace std;

#include <FftProcessObj15.h>

#include <GUIHelper9.h>
#include <SecureRestarter.h>

#include <Utils.h>
#include <Debug.h>

#include <PPMFile.h>

#include <BlaTimer.h>

#include <TrialMode6.h>

// Do not manage data dumping on WIN32
// (more simple for compilation on WIN32)
#ifndef WIN32
#include <AudioFile.h>
#endif

#include <DbgSpectrogram.h>

// DNN
#include <pt_model.h>
#include <pt_tensor.h>

#include "Rebalance.h"

#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

#define EPS 1e-10

// Same config as in article Simpson & Roma
#define BUFFER_SIZE 2048
#define OVERSAMPLING 4 //32

#define FREQ_RES 1

#define KEEP_SYNTHESIS_ENERGY 0

// When set to 0, we are accurate for frequencies
#define VARIABLE_HANNING 0

//
// Saving
//

#define DUMP_DATASET_FILES 1

// Generation parameters
#define TRACK_LIST_FILE "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Tracks.txt"
#define MIXTURES_PATH "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Mixtures"
#define SOURCE_PATH "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Sources"

#define VOCALS_FILE_NAME "vocals.wav"
#define BASS_FILE_NAME   "bass.wav"
#define DRUMS_FILE_NAME  "drums.wav"
#define OTHER_FILE_NAME  "other.wav"

#define MIX_SAVE_FILE "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.1/DeepLearning/mix.dat"
#define MASK_SAVE_FILE "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.1/DeepLearning/mask.dat"

#define MASK_SAVE_FILE_VOCAL "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.1/DeepLearning/mask0.dat"
#define MASK_SAVE_FILE_BASS "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.1/DeepLearning/mask1.dat"
#define MASK_SAVE_FILE_DRUMS "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.1/DeepLearning/mask2.dat"
#define MASK_SAVE_FILE_OTHER "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.1/DeepLearning/mask3.dat"


// Resample buffers and masks
//
// e.g 4 will make buffers of 256
//
#define RESAMPLE_FACTOR 4 //8 //4 TEST

#define NUM_INPUT_COLS 4
#define NUM_OUTPUT_COLS 1

// Models
#define MODEL_VOCAL "dnn-model-source0.model"
#define MODEL_BASS "dnn-model-source1.model"
#define MODEL_DRUMS "dnn-model-source2.model"
#define MODEL_OTHER "dnn-model-source3.model"

// Old tests with LSTM (made too biug models)
#define LSTM 0

// Listen to what the network is listening
#define DEBUG_LISTEN_BUFFER 0

// Do not predict other, but keep the remaining
#define OTHER_IS_REST 0 //1 //0

// Debug
#define DBG_PRED_SPECTRO_DUMP 0 //0 //1

#define DBG_LEARN_SPECTRO_DUMP 0 // 1

#define DUMP_OVERLAP 1 // 0

#define DEFAULT_SENSITIVITY 1.0

// With 1: bad users feedbacks: pumping effect when extracting vocal
#define HIDE_SENSITIVITY 0 //1

// Params
#define MIN_KNOB_DB -120.0
#define MAX_KNOB_DB 12.0

// FIX: result is not good for 88200Hz for example
// So resample input data to 44100, process, then resample to
// current sample rate
#define FORCE_SAMPLE_RATE 1 //0 //1
#define SAMPLE_RATE 44100.0

#if FORCE_SAMPLE_RATE
#include "../../WDL/resample.h"
#endif

#if 0
WARNING: we don't have the same result at 44100Hz and 88200Hz

SUPPORT: when new version, write email to wheatwilliams1@gmail.com
(remove vocal => pumping effect)

TODO: test with the 2 downloaded mp3

NOTE: for compiling with VST3 => created config of VST3 "base.xcodeproj" => separate directory

NOTE: switched to C++11 for pocket-tensor & OSX 10.7 + added __SSE4_1__ preprocessor macro
added __SSSE3__ too

NOTE: PocketTensor: PT_LOOP_UNROLLING_ENABLE to 1 => 15% perf gain

NOTE: 2x256 seems better than 4x128
- no phasing
- no perf slow-down

NOTE: oversampling 32 seems to give same results as oversampling 4
NOTE: with normalized values, the sound seems to bleed more
#endif

const int kNumPrograms = 1;

enum EParams
{
    kVocal = 0,
    kBass,
    kDrums,
    kOther,
    
#if !HIDE_SENSITIVITY
    kVocalSensitivity,
    kBassSensitivity,
    kDrumsSensitivity,
    kOtherSensitivity,
#endif
    
    // Global precision
    kPrecision,
    
    kMode,
    
    // Buttons
    kFileOpenMixParam,
    kFileOpenSourceParam,
    kGenerateParam,
    
    kNumParams
};

enum ELayout
{
    kWidth = GUI_WIDTH,
    kHeight = GUI_HEIGHT,
    
    kVocalX = 34,
    kVocalY = 54,
    kKnobVocalFrames = 180,
    
    kBassX = 134,
    kBassY = 54,
    kKnobBassFrames = 180,
    
    kDrumsX = 234,
    kDrumsY = 54,
    kKnobDrumsFrames = 180,
    
    kOtherX = 334,
    kOtherY = 54,
    kKnobOtherFrames = 180,
    
#if 0
    kRadioButtonsModeX = 444,
    kRadioButtonsModeY = 68,
    kRadioButtonModeVSize = 42,
    kRadioButtonModeNumButtons = 2,
    kRadioButtonsModeFrames = 2,
#endif
    
    // Global precision
    kPrecisionX = 450, //444,
    kPrecisionY = 126, //90, //140, //68,
    kKnobPrecisionFrames = 180,

    
#if !HIDE_SENSITIVITY
    // Precisions
    kVocalSensitivityX = 52,
    kVocalSensitivityY = 200, //190,
    kKnobVocalSensitivityFrames = 180,
    
    kBassSensitivityX = 152,
    kBassSensitivityY = 200,
    kKnobBassSensitivityFrames = 180,
    
    kDrumsSensitivityX = 252,
    kDrumsSensitivityY = 200,
    kKnobDrumsSensitivityFrames = 180,
    
    kOtherSensitivityX = 352,
    kOtherSensitivityY = 200,
    kKnobOtherSensitivityFrames = 180,
#endif
    
    // Buttons
    kFileOpenMixX = 530,
    kFileOpenMixY = 50,
    kFileOpenMixFrames = 3,
    
    kFileOpenSourceX = 530,
    kFileOpenSourceY = 90,
    kFileOpenSourceFrames = 3,
    
    kGenerateX = 530,
    kGenerateY = 130,
    kGenerateFrames = 3
};

//

// RebalanceDumpFftObj
class RebalanceDumpFftObj : public ProcessObj
{
public:
    class Slice
    {
    public:
        Slice();
        
        virtual ~Slice();
        
        void SetData(const deque<WDL_TypedBuf<double> > &mixCols,
                     const deque<WDL_TypedBuf<double> > &sourceCols);
        
        void GetData(vector<WDL_TypedBuf<double> > *mixCols,
                     vector<WDL_TypedBuf<double> > *sourceCols) const;
        
    protected:
        vector<WDL_TypedBuf<double> > mMixCols;
        vector<WDL_TypedBuf<double> > mSourceCols;
    };
    
    //
    
    RebalanceDumpFftObj(int bufferSize);
    
    virtual ~RebalanceDumpFftObj();
    
    virtual void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                  const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);

    // Addition, for normalized masks
    
    // Get spectrogram slices
    void GetSlices(vector<Slice> *slices);
    
    void ResetSlices();
    
protected:
    deque<WDL_TypedBuf<double> > mMixCols;
    deque<WDL_TypedBuf<double> > mSourceCols;
    
    vector<Slice> mSlices;
};

// Slice
RebalanceDumpFftObj::Slice::Slice() {}

RebalanceDumpFftObj::Slice::~Slice() {}

void
RebalanceDumpFftObj::Slice::SetData(const deque<WDL_TypedBuf<double> > &mixCols,
                                    const deque<WDL_TypedBuf<double> > &sourceCols)
{
    mMixCols.resize(mixCols.size());
    for (int i = 0; i < mMixCols.size(); i++)
        mMixCols[i] = mixCols[i];
    
    mSourceCols.resize(sourceCols.size());
    for (int i = 0; i < mSourceCols.size(); i++)
        mSourceCols[i] = sourceCols[i];
}

void
RebalanceDumpFftObj::Slice::GetData(vector<WDL_TypedBuf<double> > *mixCols,
                                    vector<WDL_TypedBuf<double> > *sourceCols) const
{
    *mixCols = mMixCols;
    *sourceCols = mSourceCols;
}

RebalanceDumpFftObj::RebalanceDumpFftObj(int bufferSize)
: ProcessObj(bufferSize)
{
    // Fill with zeros at the beginning
    mMixCols.resize(NUM_INPUT_COLS);
    for (int i = 0; i < mMixCols.size(); i++)
    {
        Utils::ResizeFillZeros(&mMixCols[i], BUFFER_SIZE/2);
    }
    
    mSourceCols.resize(NUM_INPUT_COLS);
    for (int i = 0; i < mSourceCols.size(); i++)
    {
        Utils::ResizeFillZeros(&mSourceCols[i], BUFFER_SIZE/2);
    }
}

RebalanceDumpFftObj::~RebalanceDumpFftObj() {}

void
RebalanceDumpFftObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                      const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
    if (scBuffer == NULL)
        return;
    
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    Utils::ComplexToMagn(&magnsMix, mixBuffer);
    
    mMixCols.push_back(magnsMix);
    if (mMixCols.size() > NUM_INPUT_COLS)
        mMixCols.pop_front();
    
    // Source
    WDL_TypedBuf<WDL_FFT_COMPLEX> sourceBuffer = *scBuffer;
    Utils::TakeHalf(&sourceBuffer);
    
    WDL_TypedBuf<double> magnsSource;
    Utils::ComplexToMagn(&magnsSource, sourceBuffer);
    
    mSourceCols.push_back(magnsSource);
    if (mSourceCols.size() > NUM_INPUT_COLS)
        mSourceCols.pop_front();
    
#if !DUMP_OVERLAP
    // Do not take every (overlapping) steps of mix cols
    // Take non-overlapping slices of spectrogram
    if (mMixCols.size() == NUM_INPUT_COLS)
    {
        Slice slice;
        slice.SetData(mMixCols, mSourceCols);
        mSlices.push_back(slice);
        
        mMixCols.clear();
        mSourceCols.clear();
    }
#else
    Slice slice;
    slice.SetData(mMixCols, mSourceCols);
    mSlices.push_back(slice);
#endif
}

void
RebalanceDumpFftObj::GetSlices(vector<Slice> *slices)
{
    *slices = mSlices;
}

void
RebalanceDumpFftObj::ResetSlices()
{
    mSlices.clear();
}

//MaskPredictor
//
// Predict a single mask even with several channels
// (1 mask for 2 stereo channels)
//
class MaskPredictor : public MultichannelProcess
{
public:
    MaskPredictor(int bufferSize,
                  double overlapping, double oversampling,
                  double sampleRate,
                  IGraphics *graphics);
    
    virtual ~MaskPredictor();

    void Reset();
    
    void Reset(int overlapping, int oversampling, double sampleRate);
    
#if FORCE_SAMPLE_RATE
    void ProcessInputSamplesPre(vector<WDL_TypedBuf<double> * > *ioSamples);
#endif
    
    void ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples);
    
    // Get the masks
    void GetMaskVocal(WDL_TypedBuf<double> *maskVocal);
    
    void GetMaskBass(WDL_TypedBuf<double> *maskBass);
    
    void GetMaskDrums(WDL_TypedBuf<double> *maskDrums);
    
    void GetMaskOther(WDL_TypedBuf<double> *maskOther);
    
    
    static void Downsample(WDL_TypedBuf<double> *ioBuf, double sampleRate);
    
    static void Upsample(WDL_TypedBuf<double> *ioBuf, double sampleRate);

#if FORCE_SAMPLE_RATE
    void SetPlugSampleRate(double sampleRate);
#endif
    
protected:
#ifdef WIN32
    bool LoadModelWin(IGraphics *pGraphics, int rcId,
                      std::unique_ptr<pt::Model> *model);
#endif
    
    void UpsamplePredictedMask(WDL_TypedBuf<double> *ioBuf);
    
    void PredictMask(WDL_TypedBuf<double> *result,
                     const WDL_TypedBuf<double> &mixBufHisto,
                     std::unique_ptr<pt::Model> &model);
    
    void ComputeLineMask(WDL_TypedBuf<double> *maskResult,
                         const WDL_TypedBuf<double> &maskSource);
    
    static void ColumnsToBuffer(WDL_TypedBuf<double> *buf,
                                const deque<WDL_TypedBuf<double> > &cols);
    
    void ComputeMasks(WDL_TypedBuf<double> *maskVocal,
                      WDL_TypedBuf<double> *maskBass,
                      WDL_TypedBuf<double> *maskDrums,
                      WDL_TypedBuf<double> *maskOther,
                      const WDL_TypedBuf<double> &mixBufHisto);
    
#if FORCE_SAMPLE_RATE
    void InitResamplers();
#endif
    
    int mBufferSize;
    double mOverlapping;
    double mOversampling;
    double mSampleRate;
    
    // Masks
    WDL_TypedBuf<double> mMaskVocal;
    WDL_TypedBuf<double> mMaskBass;
    WDL_TypedBuf<double> mMaskDrums;
    WDL_TypedBuf<double> mMaskOther;
    
    // DNNs
    std::unique_ptr<pt::Model> mModelVocal;
    std::unique_ptr<pt::Model> mModelBass;
    std::unique_ptr<pt::Model> mModelDrums;
    std::unique_ptr<pt::Model> mModelOther;
    
    deque<WDL_TypedBuf<double> > mMixCols;
    
#if FORCE_SAMPLE_RATE
    WDL_Resampler mResamplers[2];
    
    double mPlugSampleRate;
#endif
};

MaskPredictor::MaskPredictor(int bufferSize,
                             double overlapping, double oversampling,
                             double sampleRate,
                             IGraphics *graphics)
{
    mBufferSize = bufferSize;
    mOverlapping = overlapping;
    mOversampling = oversampling;
    
    mSampleRate = sampleRate;
    
#if FORCE_SAMPLE_RATE
    mPlugSampleRate = sampleRate;
#endif
    
    // DNN
    
#ifndef WIN32
    WDL_String resPath;
    graphics->GetResourceDir(&resPath);
    const char *resourcePath = resPath.Get();
    
    // Vocal
    char modelVocalFileName[2048];
    sprintf(modelVocalFileName, "%s/%s", resourcePath, MODEL_VOCAL);
    mModelVocal = pt::Model::create(modelVocalFileName);
    
    // Bass
    char modelBassFileName[2048];
    sprintf(modelBassFileName, "%s/%s", resourcePath, MODEL_BASS);
    mModelBass = pt::Model::create(modelBassFileName);
    
    // Drums
    char modelDrumsFileName[2048];
    sprintf(modelDrumsFileName, "%s/%s", resourcePath, MODEL_DRUMS);
    mModelDrums = pt::Model::create(modelDrumsFileName);
    
    // Other
    char modelOtherFileName[2048];
    sprintf(modelOtherFileName, "%s/%s", resourcePath, MODEL_OTHER);
    mModelOther = pt::Model::create(modelOtherFileName);
#else // WIN32
    LoadModelWin(graphics, MODEL0_ID, &mModelVocal);
    LoadModelWin(graphics, MODEL1_ID, &mModelBass);
    LoadModelWin(graphics, MODEL2_ID, &mModelDrums);
    LoadModelWin(graphics, MODEL3_ID, &mModelOther);
#endif
    
    for (int i = 0; i < NUM_INPUT_COLS; i++)
    {
        WDL_TypedBuf<double> col;
        Utils::ResizeFillZeros(&col, bufferSize/(2*RESAMPLE_FACTOR));
        
        mMixCols.push_back(col);
    }
    
#if FORCE_SAMPLE_RATE
    InitResamplers();
#endif
}

MaskPredictor::~MaskPredictor() {}

void
MaskPredictor::Reset()
{
    Reset(mOverlapping, mOversampling, mSampleRate);
    
#if FORCE_SAMPLE_RATE
    for (int i = 0; i < 2; i++)
    {
        mResamplers[i].Reset();
        // set input and output samplerates
        mResamplers[i].SetRates(mPlugSampleRate, SAMPLE_RATE);
    }
#endif
}

void
MaskPredictor::Reset(int overlapping, int oversampling, double sampleRate)
{
    mOverlapping = overlapping;
    mOversampling = oversampling;
    
    mSampleRate = sampleRate;
}

#if FORCE_SAMPLE_RATE
void
MaskPredictor::ProcessInputSamplesPre(vector<WDL_TypedBuf<double> * > *ioSamples)
{
    if (mPlugSampleRate == SAMPLE_RATE)
        return;

#if 0
    Debug::AppendData("samples0.txt", *(*ioSamples)[0]);
    
    static int count = 0;
    count++;
    
    if (count == 100)
        Debug::DumpData("samples0-100.txt", *(*ioSamples)[0]);
    if (count == 101)
        Debug::DumpData("samples0-101.txt", *(*ioSamples)[0]);
#endif
    
    double sampleRate = mPlugSampleRate;
    for (int i = 0; i < 2; i++)
    {
        if (i >= ioSamples->size())
            break;
        
        WDL_ResampleSample *resampledAudio = NULL;
        //int desiredSamples = (*ioSamples)[i]->GetSize()*SAMPLE_RATE/sampleRate;
        int desiredSamples = (*ioSamples)[i]->GetSize(); // TEST, For input driven
        int numOutSamples = (*ioSamples)[i]->GetSize()*SAMPLE_RATE/sampleRate;
        int numSamples = mResamplers[i].ResamplePrepare(desiredSamples, 1, &resampledAudio);
        
        for (int j = 0; j < numSamples; j++)
        {
            if (j >= (*ioSamples)[i]->GetSize())
                break;
            resampledAudio[j] = (*ioSamples)[i]->Get()[j];
        }
        
        WDL_TypedBuf<double> outSamples;
        outSamples.Resize(desiredSamples);
        int numResampled = mResamplers[i].ResampleOut(outSamples.Get(),
                                                      (*ioSamples)[i]->GetSize(),
                                                      outSamples.GetSize(), 1);
        
        //outSamples.Resize(numOutSamples); // TEST: good but click with buffer size 447
        
        // GOOD !
        // Avoid clicks sometimes (for example with 88200Hz and buffer size 447)
        // The numResampled varies around a value, to keep consistency of the stream
        outSamples.Resize(numResampled); // TEST 2
        
        *((*ioSamples)[i]) = outSamples;
    }
    
#if 0
    Debug::AppendData("samples1.txt", *(*ioSamples)[0]);
    
    if (count == 100)
        Debug::DumpData("samples1-100.txt", *(*ioSamples)[0]);
    if (count == 101)
        Debug::DumpData("samples1-101.txt", *(*ioSamples)[0]);
#endif
}
#endif

void
MaskPredictor::ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples)
{
    if (ioFftSamples->size() < 1)
        return;
    
    // Take only the left channel...
    //
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples = *(*ioFftSamples)[0];
    
    // Take half of the complexes
    Utils::TakeHalf(&fftSamples);
    
    WDL_TypedBuf<double> magns;
    WDL_TypedBuf<double> phases;
    
    Utils::ComplexToMagnPhase(&magns, &phases, fftSamples);

    // Compute the masks
    WDL_TypedBuf<double> magnsDown = magns;
    Downsample(&magnsDown, mSampleRate);
    
    // mMixCols is filled with zeros at the origin
    mMixCols.push_back(magnsDown);
    mMixCols.pop_front();
    
    WDL_TypedBuf<double> mixBufHisto;
    ColumnsToBuffer(&mixBufHisto, mMixCols);
    
    ComputeMasks(&mMaskVocal, &mMaskBass, &mMaskDrums, &mMaskOther, mixBufHisto);
}

void
MaskPredictor::GetMaskVocal(WDL_TypedBuf<double> *maskVocal)
{
    *maskVocal = mMaskVocal;
}

void
MaskPredictor::GetMaskBass(WDL_TypedBuf<double> *maskBass)
{
    *maskBass = mMaskBass;
}

void
MaskPredictor::GetMaskDrums(WDL_TypedBuf<double> *maskDrums)
{
    *maskDrums = mMaskDrums;
}

void
MaskPredictor::GetMaskOther(WDL_TypedBuf<double> *maskOther)
{
    *maskOther = mMaskOther;
}

void
MaskPredictor::Downsample(WDL_TypedBuf<double> *ioBuf, double sampleRate)
{
#if FORCE_SAMPLE_RATE
    sampleRate = SAMPLE_RATE;
#endif
    
    double hzPerBin = sampleRate/(BUFFER_SIZE/2);
    WDL_TypedBuf<double> bufMel;
    Utils::FreqsToMelNorm(&bufMel, *ioBuf, hzPerBin);
    *ioBuf = bufMel;
    
    if (RESAMPLE_FACTOR == 1)
        // No need to downsample
        return;
    
    int newSize = ioBuf->GetSize()/RESAMPLE_FACTOR;
    
    Utils::ResizeLinear(ioBuf, newSize);
}

void
MaskPredictor::Upsample(WDL_TypedBuf<double> *ioBuf, double sampleRate)
{
#if FORCE_SAMPLE_RATE
    sampleRate = SAMPLE_RATE;
#endif

    if (RESAMPLE_FACTOR != 1)
    {
        int newSize = ioBuf->GetSize()*RESAMPLE_FACTOR;
        
        Utils::ResizeLinear(ioBuf, newSize);
    }
    
    double hzPerBin = sampleRate/(BUFFER_SIZE/2);
    WDL_TypedBuf<double> bufMel;
    Utils::MelToFreqsNorm(&bufMel, *ioBuf, hzPerBin);
    *ioBuf = bufMel;
}

#if FORCE_SAMPLE_RATE
void
MaskPredictor::SetPlugSampleRate(double sampleRate)
{
    mPlugSampleRate = sampleRate;
    
    InitResamplers();
}
#endif

#ifdef WIN32
bool
MaskPredictor::LoadModelWin(IGraphics *pGraphics, int rcId,
                            std::unique_ptr<pt::Model> *model)
{
    void *rcBuf;
	long rcSize;
	bool loaded = ((IGraphicsWin *)pGraphics)->LoadWindowsResource(rcId, "RCDATA", &rcBuf, &rcSize);
	if (!loaded)
		return false;
    
    *model = pt::Model::create(rcBuf, rcSize);

	return true;
}
#endif

// Split the mask cols, get each mask and upsample it
void
MaskPredictor::UpsamplePredictedMask(WDL_TypedBuf<double> *ioBuf)
{
    if (RESAMPLE_FACTOR == 1)
        // No need to upsample
        return;
    
    WDL_TypedBuf<double> result;
    
    for (int j = 0; j < NUM_OUTPUT_COLS; j++)
    {
        WDL_TypedBuf<double> mask;
        mask.Resize(BUFFER_SIZE/(2*RESAMPLE_FACTOR));
        
        // Optim
        memcpy(mask.Get(),
               &ioBuf->Get()[j*mask.GetSize()],
               mask.GetSize()*sizeof(double));
        
        Upsample(&mask, mSampleRate);
        
        result.Add(mask.Get(), mask.GetSize());
    }
    
    *ioBuf = result;
}

void
MaskPredictor::PredictMask(WDL_TypedBuf<double> *result,
                           const WDL_TypedBuf<double> &mixBufHisto,
                           std::unique_ptr<pt::Model> &model)
{
#if !LSTM
    pt::Tensor in(mixBufHisto.GetSize());
    for (int i = 0; i < mixBufHisto.GetSize(); i++)
    {
        in(i) = mixBufHisto.Get()[i];
    }
#else
    pt::Tensor in(1, mixBufHisto.GetSize());
    for (int i = 0; i < mixBufHisto.GetSize(); i++)
    {
        in(0, i) = mixBufHisto.Get()[i];
    }
#endif
    
    pt::Tensor out;
    model->predict(in, out);
    
    WDL_TypedBuf<double> maskBuf;
    maskBuf.Resize(NUM_OUTPUT_COLS*BUFFER_SIZE/(RESAMPLE_FACTOR*2));
    
    for (int i = 0; i < maskBuf.GetSize(); i++)
    {
        maskBuf.Get()[i] = out(i);
    }
    
    Utils::ClipMin(&maskBuf, 0.0);
    
    UpsamplePredictedMask(&maskBuf);
    
    Utils::ClipMin(&maskBuf, 0.0);
    
    *result = maskBuf;
}

void
MaskPredictor::ColumnsToBuffer(WDL_TypedBuf<double> *buf,
                               const deque<WDL_TypedBuf<double> > &cols)
{
    if (cols.empty())
        return;
    
    buf->Resize(cols.size()*cols[0].GetSize());
    
    for (int j = 0; j < cols.size(); j++)
    {
        const WDL_TypedBuf<double> &col = cols[j];
        for (int i = 0; i < col.GetSize(); i++)
        {
            int bufIndex = i + j*col.GetSize();
            
            buf->Get()[bufIndex] = col.Get()[i];
        }
    }
}

void
MaskPredictor::ComputeMasks(WDL_TypedBuf<double> *maskVocal,
                            WDL_TypedBuf<double> *maskBass,
                            WDL_TypedBuf<double> *maskDrums,
                            WDL_TypedBuf<double> *maskOther,
                            const WDL_TypedBuf<double> &mixBufHisto)
{
    WDL_TypedBuf<double> maskVocalFull;
    PredictMask(&maskVocalFull, mixBufHisto, mModelVocal);
    
    WDL_TypedBuf<double> maskBassFull;
    PredictMask(&maskBassFull, mixBufHisto, mModelBass);
    
    WDL_TypedBuf<double> maskDrumsFull;
    PredictMask(&maskDrumsFull, mixBufHisto, mModelDrums);
    
    WDL_TypedBuf<double> maskOtherFull;
    PredictMask(&maskOtherFull, mixBufHisto, mModelOther);
    
    ComputeLineMask(maskVocal, maskVocalFull);
    ComputeLineMask(maskBass, maskBassFull);
    ComputeLineMask(maskDrums, maskDrumsFull);
    ComputeLineMask(maskOther, maskOtherFull);
}

void
MaskPredictor::ComputeLineMask(WDL_TypedBuf<double> *maskResult,
                               const WDL_TypedBuf<double> &maskSource)
{
    int numFreqs = BUFFER_SIZE/2;
    int numCols = NUM_OUTPUT_COLS;
    
    maskResult->Resize(numFreqs);
    
    for (int i = 0; i < numFreqs; i++)
    {
        double avg = 0.0;
        for (int j = 0; j < numCols; j++)
        {
            int idx = i + j*numFreqs;
            
            double m = maskSource.Get()[idx];
            avg += m;
        }
        
        avg /= numCols;
        
        maskResult->Get()[i] = avg;
    }
}

#if FORCE_SAMPLE_RATE
void
MaskPredictor::InitResamplers()
{
    // In
    
    for (int i = 0; i < 2; i++)
    {
        mResamplers[i].SetMode(true, 1, false, 0, 0);
        //mResamplers[i].SetMode(true, 1, true, 64, 32); // TEST
        mResamplers[i].SetFilterParms();
        // set it output driven
        //mResamplers[i].SetFeedMode(false);
        
        // GOOD !
        // TEST: set input driven
        // (because output driven has a bug when downsampling:
        // the first samples are bad)
        mResamplers[i].SetFeedMode(true);
        
        // set input and output samplerates
        mResamplers[i].SetRates(mPlugSampleRate, SAMPLE_RATE);
    }
}
#endif

// RebalanceProcessFftObj
//
// Apply the masks to the current channels
//
class RebalanceProcessFftObj : public ProcessObj
{
public:
    RebalanceProcessFftObj(int bufferSize, MaskPredictor *maskPred);
    
    virtual ~RebalanceProcessFftObj();
    
    void Reset(int oversampling, int freqRes, double sampleRate);
    
    void Reset();
    
#if FORCE_SAMPLE_RATE
    void ResetResamplers();
    
    void ProcessInputSamplesPre(WDL_TypedBuf<double> *ioBuffer,
                                const WDL_TypedBuf<double> *scBuffer);
#endif
    
    virtual void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                  const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
    
#if FORCE_SAMPLE_RATE
    void ProcessSamplesPost(WDL_TypedBuf<double> *ioBuffer);
#endif

    void SetVocal(double vocal);
    void SetBass(double bass);
    void SetDrums(double drums);
    void SetOther(double other);
    
    void SetVocalSensitivity(double vocalSensitivity);
    void SetBassSensitivity(double bassSensitivity);
    void SetDrumsSensitivity(double drumsSensitivity);
    void SetOtherSensitivity(double otherSensitivity);
    
    void SetMode(Rebalance::Mode mode);
    
    // Global precision (previous soft/hard)
    void SetPrecision(double precision);
    
    // Apply on values
    void ApplySensitivity(double *maskVocal, double *maskBass,
                          double *maskDrums, double *maskOther);
    
    void DBG_SetSpectrogramDump(bool flag);
    
#if FORCE_SAMPLE_RATE
    void SetPlugSampleRate(double sampleRate);
#endif

protected:
    void ComputeMixSoft(WDL_TypedBuf<double> *resultMagns,
                        const WDL_TypedBuf<double> &magnsMix);
    
    void ComputeMixHard(WDL_TypedBuf<double> *resultMagns,
                        const WDL_TypedBuf<double> &magnsMix);
    
    void NormalizeMasks(WDL_TypedBuf<double> *maskVocal,
                        WDL_TypedBuf<double> *maskBass,
                        WDL_TypedBuf<double> *maskDrums,
                        WDL_TypedBuf<double> *maskOther);
    
    // Apply on whole masks
    void ApplySensitivity(WDL_TypedBuf<double> *maskVocal,
                          WDL_TypedBuf<double> *maskBass,
                          WDL_TypedBuf<double> *maskDrums,
                          WDL_TypedBuf<double> *maskOther);

    
    // Debug: listen to the audio sent to the dnn
    void DBG_ListenBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                          const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
    
    void DBG_SaveSpectrograms();
    
#if FORCE_SAMPLE_RATE
    void InitResamplers();
#endif

    //
    
    MaskPredictor *mMaskPred;
    
    // Parameters
    double mVocal;
    double mBass;
    double mDrums;
    double mOther;
    
    double mVocalSensitivity;
    double mBassSensitivity;
    double mDrumsSensitivity;
    double mOtherSensitivity;
    
    // Global precision
    double mPrecision;
    
    Rebalance::Mode mMode;
    
    // Debug
    bool mDbgSpectrogramDump;
    
    DbgSpectrogram *mDbgMixSpectrogram;
    DbgSpectrogram *mDbgSourceSpectrograms[4];
    
#if FORCE_SAMPLE_RATE
    WDL_Resampler mResamplerIn;
    WDL_Resampler mResamplerOut;
    
    double mPlugSampleRate;
    
    // ratio of sample remaining after upsampling
    // (to adjust and avoid clicks / blank zones)
    double mRemainingSamples;
#endif
};

RebalanceProcessFftObj::RebalanceProcessFftObj(int bufferSize,
                                               MaskPredictor *maskPred)
: ProcessObj(bufferSize)
{
    mMaskPred = maskPred;
    
    // Parameters
    mVocal = 0.0;
    mBass = 0.0;
    mDrums = 0.0;
    mOther = 0.0;
    
    mVocalSensitivity = 1.0;
    mBassSensitivity = 1.0;
    mDrumsSensitivity = 1.0;
    mOtherSensitivity = 1.0;
    
    // Global precision (soft/hard)
    mPrecision = 0.0;
    
    mMode = Rebalance::SOFT;
    
    // Spectrogram debug
    mDbgSpectrogramDump = false;
    
    mDbgMixSpectrogram = NULL;
    for (int k = 0; k < 4; k++)
        mDbgSourceSpectrograms[k] = NULL;
    
#if FORCE_SAMPLE_RATE
    mSampleRate = SAMPLE_RATE;
    mPlugSampleRate = SAMPLE_RATE;
    
    InitResamplers();
    
    mRemainingSamples = 0.0;
#endif
}

RebalanceProcessFftObj::~RebalanceProcessFftObj()
{
    if (mDbgMixSpectrogram != NULL)
        delete mDbgMixSpectrogram;
    
    for (int k = 0; k < 4; k++)
    {
        if (mDbgSourceSpectrograms[k] != NULL)
            delete mDbgSourceSpectrograms[k];
    }
}

void
RebalanceProcessFftObj::Reset(int oversampling, int freqRes, double sampleRate)
{
    ProcessObj::Reset(oversampling, freqRes, sampleRate);
    
#if FORCE_SAMPLE_RATE
    mPlugSampleRate = sampleRate;
    
    ResetResamplers();
    
    mRemainingSamples = 0.0;
#endif
}

void
RebalanceProcessFftObj::Reset()
{
    ProcessObj::Reset();

#if FORCE_SAMPLE_RATE
    ResetResamplers();
    
    mRemainingSamples = 0.0;
#endif
}

#if FORCE_SAMPLE_RATE
void
RebalanceProcessFftObj::ResetResamplers()
{
    mResamplerIn.Reset();
    mResamplerIn.SetRates(mPlugSampleRate, SAMPLE_RATE);
    
    mResamplerOut.Reset();
    mResamplerOut.SetRates(SAMPLE_RATE, mPlugSampleRate);
}
#endif

void
RebalanceProcessFftObj::SetVocal(double vocal)
{
    mVocal = vocal;
}

void
RebalanceProcessFftObj::SetBass(double bass)
{
    mBass = bass;
}

void
RebalanceProcessFftObj::SetDrums(double drums)
{
    mDrums = drums;
}

void
RebalanceProcessFftObj::SetOther(double other)
{
    mOther = other;
}

void
RebalanceProcessFftObj::SetVocalSensitivity(double vocalSensitivity)
{
    mVocalSensitivity = vocalSensitivity;
}

void
RebalanceProcessFftObj::SetBassSensitivity(double bassSensitivity)
{
    mBassSensitivity = bassSensitivity;
}

void
RebalanceProcessFftObj::SetDrumsSensitivity(double drumsSensitivity)
{
    mDrumsSensitivity = drumsSensitivity;
}

void
RebalanceProcessFftObj::SetOtherSensitivity(double otherSensitivity)
{
    mOtherSensitivity = otherSensitivity;
}

void
RebalanceProcessFftObj::SetPrecision(double precision)
{
    mPrecision = precision;
}

void
RebalanceProcessFftObj::SetMode(Rebalance::Mode mode)
{
    mMode = mode;
}

void
RebalanceProcessFftObj::ApplySensitivity(double *maskVocal, double *maskBass,
                                       double *maskDrums, double *maskOther)
{
    if (*maskVocal < mVocalSensitivity)
        *maskVocal = 0.0;
    
    if (*maskBass < mBassSensitivity)
        *maskBass = 0.0;
    
    if (*maskDrums < mDrumsSensitivity)
        *maskDrums = 0.0;
    
    if (*maskOther < mOtherSensitivity)
        *maskOther = 0.0;
}

void
RebalanceProcessFftObj::DBG_SetSpectrogramDump(bool flag)
{
    mDbgSpectrogramDump = flag;
    
    if (mDbgSpectrogramDump)
    {
        mDbgMixSpectrogram = new DbgSpectrogram(BUFFER_SIZE/(2/**RESAMPLE_FACTOR*/));
        mDbgMixSpectrogram->SetAmpDb(true);
        
        for (int k = 0; k < 4; k++)
            mDbgSourceSpectrograms[k] = new DbgSpectrogram(BUFFER_SIZE/(2/**RESAMPLE_FACTOR*/));
    }
}

void
RebalanceProcessFftObj::DBG_SaveSpectrograms()
{
    if (mDbgSpectrogramDump)
    {
        mDbgMixSpectrogram->SavePPM("pred-mix.ppm");
        
        mDbgSourceSpectrograms[0]->SavePPM("pred-source0.ppm");
        mDbgSourceSpectrograms[1]->SavePPM("pred-source1.ppm");
        mDbgSourceSpectrograms[2]->SavePPM("pred-source2.ppm");
        mDbgSourceSpectrograms[3]->SavePPM("pred-source3.ppm");
    }
}

#if FORCE_SAMPLE_RATE
void
RebalanceProcessFftObj::ProcessInputSamplesPre(WDL_TypedBuf<double> *ioBuffer,
                                               const WDL_TypedBuf<double> *scBuffer)
{
#if 0 // No need to resample, it has already been resampled
      // by the multichannel obj
    if (mSampleRate == SAMPLE_RATE)
        return;
    
    WDL_ResampleSample *resampledAudio = NULL;
    int numSamples = ioBuffer->GetSize()*SAMPLE_RATE/mSampleRate;
    int numSamples0 = mResamplerIn.ResamplePrepare(numSamples, 1, &resampledAudio);
    for (int i = 0; i < numSamples0; i++)
    {
        if (i >= ioBuffer->GetSize())
            break;
        resampledAudio[i] = ioBuffer->Get()[i];
    }
    
    WDL_TypedBuf<double> outSamples;
    outSamples.Resize(numSamples);
    int numResampled = mResamplerIn.ResampleOut(outSamples.Get(), ioBuffer->GetSize(), numSamples, 1);
    if (numResampled != numSamples)
    {
        //failed somehow
        //memset(outSamples.Get(), 0, numSamples*sizeof(double));
    }
    
    *ioBuffer = outSamples;
#endif
}
#endif


void
RebalanceProcessFftObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                         const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
#if DEBUG_LISTEN_BUFFER
    DBG_ListenBuffer(ioBuffer, scBuffer);

    return;
#endif
    
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    WDL_TypedBuf<double> phasesMix;
    Utils::ComplexToMagnPhase(&magnsMix, &phasesMix, mixBuffer);
    
    WDL_TypedBuf<double> resultMagns;
    
    // Prev code: binary choice: soft or hard
#if 0
    if (mMode == Rebalance::SOFT)
        ComputeMixSoft(&resultMagns, magnsMix);
    else
        ComputeMixHard(&resultMagns, magnsMix);
#endif
    
#if 1
    // New code: interpolate between soft and hard
    WDL_TypedBuf<double> magnsSoft;
    ComputeMixSoft(&magnsSoft, magnsMix);
    
    WDL_TypedBuf<double> magnsHard;
    ComputeMixHard(&magnsHard, magnsMix);
    
    Utils::Interp(&resultMagns, &magnsSoft, &magnsHard, mPrecision);
#endif
    
    // Fill the result
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    Utils::MagnPhaseToComplex(&fftSamples, resultMagns, phasesMix);
    
    fftSamples.Resize(fftSamples.GetSize()*2);
    
    Utils::FillSecondFftHalf(&fftSamples);
    
    // Result
    *ioBuffer = fftSamples;
}

#if FORCE_SAMPLE_RATE
void
RebalanceProcessFftObj::ProcessSamplesPost(WDL_TypedBuf<double> *ioBuffer)
{
#if 0
    //Debug::AppendData("samples0.txt", *ioBuffer);
    static int count = 0;
    count++;
    
    if (count == 100)
        Debug::DumpData("samples0-100.txt", *ioBuffer);
    if (count == 101)
        Debug::DumpData("samples0-101.txt", *ioBuffer);
#endif
    
    if (mPlugSampleRate == SAMPLE_RATE)
        return;
    
    double sampleRate = mPlugSampleRate;
    
    WDL_ResampleSample *resampledAudio = NULL;
    //int desiredSamples = ioBuffer->GetSize()*sampleRate/SAMPLE_RATE;
    int desiredSamples = ioBuffer->GetSize(); // TEST input driven
    int numOutSamples = ioBuffer->GetSize()*sampleRate/SAMPLE_RATE; // TEST input driven
    int numSamples = mResamplerOut.ResamplePrepare(desiredSamples, 1, &resampledAudio);
    
    // Compute remaining "parts of sample", due to rounding
    // and re-add it to the number of requested samples
    // FIX: fixes blank frame with sample rate 48000 and buffer size 447
    //
    double remaining = ((double)ioBuffer->GetSize())*sampleRate/SAMPLE_RATE - numOutSamples;
    mRemainingSamples += remaining;
    if (mRemainingSamples >= 1.0)
    {
        int addSamples = floor(mRemainingSamples);
        mRemainingSamples -= addSamples;
        
        numOutSamples += addSamples;
    }
    
    for (int i = 0; i < numSamples; i++)
    {
        if (i >= ioBuffer->GetSize())
            break;
        resampledAudio[i] = ioBuffer->Get()[i];
    }
    
    WDL_TypedBuf<double> outSamples;
    //outSamples.Resize(desiredSamples);
    outSamples.Resize(numOutSamples); // TEST input driven
    
    int numResampled = mResamplerOut.ResampleOut(outSamples.Get(),
                                                 ioBuffer->GetSize(),
                                                 outSamples.GetSize(), 1);
    
    //fprintf(stderr, "desired: %d num out samples: %d numSamples: %d numResampled: %d\n",
    //        desiredSamples, numOutSamples, numSamples, numResampled);
    
    *ioBuffer = outSamples;
    
#if 0
    //Debug::AppendData("samples1.txt", *ioBuffer);
    
    if (count == 100)
        Debug::DumpData("samples1-100.txt", *ioBuffer);
    if (count == 101)
        Debug::DumpData("samples1-101.txt", *ioBuffer);
#endif
}
#endif


void
RebalanceProcessFftObj::DBG_ListenBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                         const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    WDL_TypedBuf<double> phasesMix;
    Utils::ComplexToMagnPhase(&magnsMix, &phasesMix, mixBuffer);

    MaskPredictor::Downsample(&magnsMix, mSampleRate);
    MaskPredictor::Upsample(&magnsMix, mSampleRate);
    
    // Fill the result
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    Utils::MagnPhaseToComplex(&fftSamples, magnsMix, phasesMix);
    
    fftSamples.Resize(fftSamples.GetSize()*2);
    
    Utils::FillSecondFftHalf(&fftSamples);
    
    // Result
    *ioBuffer = fftSamples;
}

void
RebalanceProcessFftObj::NormalizeMasks(WDL_TypedBuf<double> *maskVocal,
                                       WDL_TypedBuf<double> *maskBass,
                                       WDL_TypedBuf<double> *maskDrums,
                                       WDL_TypedBuf<double> *maskOther)
{
    for (int i = 0; i < maskVocal->GetSize(); i++)
    {
        double vocal = maskVocal->Get()[i];
        double bass = maskBass->Get()[i];
        double drums = maskDrums->Get()[i];
        double other = maskOther->Get()[i];
        
        double sum = vocal + bass + drums + other;
        
        if (fabs(sum) > EPS)
        {
            vocal /= sum;
            bass /= sum;
            drums /= sum;
            other /= sum;
        }
        
        maskVocal->Get()[i] = vocal;
        maskBass->Get()[i] = bass;
        maskDrums->Get()[i] = drums;
        maskOther->Get()[i] = other;
    }
}

#if FORCE_SAMPLE_RATE
void
RebalanceProcessFftObj::SetPlugSampleRate(double sampleRate)
{
    mPlugSampleRate = sampleRate;
    
    InitResamplers();
}
#endif

void
RebalanceProcessFftObj::ComputeMixSoft(WDL_TypedBuf<double> *resultMagns,
                                       const WDL_TypedBuf<double> &magnsMix)
{
    Utils::ResizeFillZeros(resultMagns, magnsMix.GetSize());
    
    if (mMaskPred == NULL)
        return;
    
    WDL_TypedBuf<double> maskVocal;
    WDL_TypedBuf<double> maskBass;
    WDL_TypedBuf<double> maskDrums;
    WDL_TypedBuf<double> maskOther;
    
    mMaskPred->GetMaskVocal(&maskVocal);
    mMaskPred->GetMaskBass(&maskBass);
    mMaskPred->GetMaskDrums(&maskDrums);
    mMaskPred->GetMaskOther(&maskOther);
    
    // NEW
    // NOTE: previously, when setting Other sensitivity to 0,
    // there was no change (in soft mode only), than with sensitivity set
    // to 100
    ApplySensitivity(&maskVocal, &maskBass, &maskDrums, &maskOther);
    
    NormalizeMasks(&maskVocal, &maskBass, &maskDrums, &maskOther);
    
    if (mDbgSpectrogramDump)
    {
        mDbgMixSpectrogram->AddLine(magnsMix);
        
        mDbgSourceSpectrograms[0]->AddLine(maskVocal);
        mDbgSourceSpectrograms[1]->AddLine(maskBass);
        mDbgSourceSpectrograms[2]->AddLine(maskDrums);
        mDbgSourceSpectrograms[3]->AddLine(maskOther);
        
        static int count = 0;
        if (count++ == 600)
            DBG_SaveSpectrograms();
    }
   
    // ORIGIN
    //ApplySensitivity(&maskVocal, &maskBass, &maskDrums, &maskOther);
    
    for (int i = 0; i < resultMagns->GetSize(); i++)
    {
        double maskVocal0 = maskVocal.Get()[i];
        double maskBass0 = maskBass.Get()[i];
        double maskDrums0 = maskDrums.Get()[i];
        double maskOther0 = maskOther.Get()[i];
        
        ApplySensitivity(&maskVocal0, &maskBass0,
                         &maskDrums0, &maskOther0);
        
        maskVocal.Get()[i] = maskVocal0;
        maskBass.Get()[i] = maskBass0;
        maskDrums.Get()[i] = maskDrums0;
        maskOther.Get()[i] = maskOther0;
    }
    
    for (int i = 0; i < resultMagns->GetSize(); i++)
    {
        double maskVocal0 = maskVocal.Get()[i];
        double maskBass0 = maskBass.Get()[i];
        double maskDrums0 = maskDrums.Get()[i];
        double maskOther0 = maskOther.Get()[i];
        
#if OTHER_IS_REST
        maskOther0 = 1.0 - (maskVocal0 + maskBass0 + maskDrums0);
#endif
        
        double vocalCoeff = maskVocal0*mVocal;
        double bassCoeff = maskBass0*mBass;
        double drumsCoeff = maskDrums0*mDrums;
        double otherCoeff = maskOther0*mOther;
        
        double coeff = vocalCoeff + bassCoeff + drumsCoeff + otherCoeff;
        
        double val = magnsMix.Get()[i];
        
        resultMagns->Get()[i] = val * coeff;
    }
}

void
RebalanceProcessFftObj::ComputeMixHard(WDL_TypedBuf<double> *resultMagns,
                                       const WDL_TypedBuf<double> &magnsMix)
{
    Utils::ResizeFillZeros(resultMagns, magnsMix.GetSize());
    
    if (mMaskPred == NULL)
        return;
    
    WDL_TypedBuf<double> maskVocal;
    WDL_TypedBuf<double> maskBass;
    WDL_TypedBuf<double> maskDrums;
    WDL_TypedBuf<double> maskOther;
    
    mMaskPred->GetMaskVocal(&maskVocal);
    mMaskPred->GetMaskBass(&maskBass);
    mMaskPred->GetMaskDrums(&maskDrums);
    mMaskPred->GetMaskOther(&maskOther);
    
    for (int i = 0; i < resultMagns->GetSize(); i++)
    {
        double maskVocal0 = maskVocal.Get()[i];
        double maskBass0 = maskBass.Get()[i];
        double maskDrums0 = maskDrums.Get()[i];
        double maskOther0 = maskOther.Get()[i];
        
        // Theshold, just in case (prediction can return negative mask values)
        if (maskVocal0 < 0.0)
            maskVocal0 = 0.0;
        if (maskBass0 < 0.0)
            maskBass0 = 0.0;
        if (maskDrums0 < 0.0)
            maskDrums0 = 0.0;
        if (maskOther0 < 0.0)
            maskOther0 = 0.0;
       
        ApplySensitivity(&maskVocal0, &maskBass0,
                        &maskDrums0, &maskOther0);
        
#if OTHER_IS_REST
        // Normalize
        double sum = maskVocal0 + maskBass0 + maskDrums0 + maskOther0;
        if (fabs(sum) > EPS)
        {
            maskVocal0 /= sum;
            maskBass0 /= sum;
            maskDrums0 /= sum;
            maskOther0 /= sum;
        }
        
        maskOther0 = 1.0 - (maskVocal0 + maskBass0 + maskDrums0);
#endif
        
        // Compute max coeff
        double maxMask = maskVocal0;
        double coeffMax = mVocal;
        
        // NEW
        if (maskVocal0 < EPS)
            coeffMax = 0.0;
        
        if (maskBass0 > maxMask)
        {
            maxMask = maskBass0;
            coeffMax = mBass;
        }
        
        if (maskDrums0 > maxMask)
        {
            maxMask = maskDrums0;
            coeffMax = mDrums;
        }
        
        if (maskOther0 > maxMask)
        {
            maxMask = maskOther0;
            coeffMax = mOther;
        }
        
        double val = magnsMix.Get()[i];
        
        resultMagns->Get()[i] = val * coeffMax;
    }
}

void
RebalanceProcessFftObj::ApplySensitivity(WDL_TypedBuf<double> *maskVocal,
                                         WDL_TypedBuf<double> *maskBass,
                                         WDL_TypedBuf<double> *maskDrums,
                                         WDL_TypedBuf<double> *maskOther)
{
    for (int i = 0; i < maskVocal->GetSize(); i++)
    {
        double maskVocal0 = maskVocal->Get()[i];
        double maskBass0 = maskBass->Get()[i];
        double maskDrums0 = maskDrums->Get()[i];
        double maskOther0 = maskOther->Get()[i];
    
        ApplySensitivity(&maskVocal0, &maskBass0,
                         &maskDrums0, &maskOther0);
    
        maskVocal->Get()[i] = maskVocal0;
        maskBass->Get()[i] = maskBass0;
        maskDrums->Get()[i] = maskDrums0;
        maskOther->Get()[i] = maskOther0;
    }
}

#if FORCE_SAMPLE_RATE
void
RebalanceProcessFftObj::InitResamplers()
{
    // In
    mResamplerIn.SetMode(true, 1, false, 0, 0);
    mResamplerIn.SetFilterParms();
    // set it output driven
    mResamplerIn.SetFeedMode(false);
    
    // set input and output samplerates
    mResamplerIn.SetRates(mPlugSampleRate, SAMPLE_RATE);
    
    // Out
    mResamplerOut.SetMode(true, 1, false, 0, 0);
    mResamplerOut.SetFilterParms();
    // set it output driven
    //mResamplerOut.SetFeedMode(false);
    // GOOD !
    mResamplerOut.SetFeedMode(true); // TEST: input driven
    
    // set input and output samplerates
    mResamplerOut.SetRates(SAMPLE_RATE, mPlugSampleRate);
}
#endif

////

Rebalance::Rebalance(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;
    
  GUIHelper9 guiHelper;
  
  // Init WDL FFT
  FftProcessObj15::Init();

  for (int i = 0; i < 4; i++)
    mFftObjs[i] = NULL;

#if SA_API
  for (int i = 0; i < 4; i++)
    mRebalanceDumpFftObjs[i] = NULL;
#else
  mMaskPred = NULL;
#endif
    
  mDbgSpectrogramDump = DBG_LEARN_SPECTRO_DUMP;
  
  mDbgMixSpectrogram = NULL;
    
  for (int k = 0; k < 4; k++)
      mDbgSourceSpectrograms[k] = NULL;
    
  if (mDbgSpectrogramDump)
  {
      mDbgMixSpectrogram = new DbgSpectrogram(BUFFER_SIZE/(2*RESAMPLE_FACTOR));
      mDbgMixSpectrogram->SetAmpDb(true);
      
      for (int k = 0; k < 4; k++)
          mDbgSourceSpectrograms[k] = new DbgSpectrogram(BUFFER_SIZE/(2*RESAMPLE_FACTOR));
  }
    
  mVocalSensitivity = DEFAULT_SENSITIVITY;
  mBassSensitivity = DEFAULT_SENSITIVITY;
  mDrumsSensitivity = DEFAULT_SENSITIVITY;
  mOtherSensitivity = DEFAULT_SENSITIVITY;
    
  mMode = SOFT;
    
  // GUI
  
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);
  
  // Vocal
#if !SA_API    
  double knobsShape = Utils::ComputeShapeForCenter0(MIN_KNOB_DB, MAX_KNOB_DB);
    
  double defaultVocal = 0.0;

  mVocal = defaultVocal;
  GetParam(kVocal)->InitDouble("Vocal", defaultVocal, MIN_KNOB_DB, MAX_KNOB_DB, 0.1, "dB");
  GetParam(kVocal)->SetShape(knobsShape);
#endif
    
  guiHelper.CreateKnob3(this, pGraphics,
                       KNOB_VOCAL_ID, KNOB_VOCAL_FN, kKnobVocalFrames,
                       kVocalX, kVocalY, kVocal, "VOCAL",
                       KNOB_SHADOW_ID, KNOB_SHADOW_FN,
                       BLUE_KNOB_DIFFUSE_ID, BLUE_KNOB_DIFFUSE_FN,
                       TEXTFIELD_ID, TEXTFIELD_FN);
    
    // Bass
#if !SA_API
    double defaultBass = 0.0;
    mBass = defaultBass;
    
    GetParam(kBass)->InitDouble("Bass", defaultBass, MIN_KNOB_DB, MAX_KNOB_DB, 0.1, "dB");
    GetParam(kBass)->SetShape(knobsShape);
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_BASS_ID, KNOB_BASS_FN, kKnobBassFrames,
                          kBassX, kBassY, kBass, "BASS",
                          KNOB_SHADOW_ID, KNOB_SHADOW_FN,
                          BLUE_KNOB_DIFFUSE_ID, BLUE_KNOB_DIFFUSE_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN);
    
    // Drums
#if !SA_API
    double defaultDrums = 0.0;
    mDrums = defaultDrums;
    
    GetParam(kDrums)->InitDouble("Drums", defaultDrums, MIN_KNOB_DB, MAX_KNOB_DB, 0.1, "dB");
    GetParam(kDrums)->SetShape(knobsShape);
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_DRUMS_ID, KNOB_DRUMS_FN, kKnobDrumsFrames,
                          kDrumsX, kDrumsY, kDrums, "DRUMS",
                          KNOB_SHADOW_ID, KNOB_SHADOW_FN,
                          BLUE_KNOB_DIFFUSE_ID, BLUE_KNOB_DIFFUSE_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN);
  
    // Other
#if !SA_API
    double defaultOther = 0.0;
    mOther = defaultOther;
    
    GetParam(kOther)->InitDouble("Other", defaultOther, MIN_KNOB_DB, MAX_KNOB_DB, 0.1, "dB");
    GetParam(kOther)->SetShape(knobsShape);
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_OTHER_ID, KNOB_OTHER_FN, kKnobOtherFrames,
                          kOtherX, kOtherY, kOther, "OTHER",
                          KNOB_SHADOW_ID, KNOB_SHADOW_FN,
                          BLUE_KNOB_DIFFUSE_ID, BLUE_KNOB_DIFFUSE_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN);

#if 0 // Mode
    // Mode
    Mode defaultMode = SOFT;
    mMode = defaultMode;
    
    GetParam(kMode)->InitInt("Mode", (int)defaultMode, 0, 1);
    
#define NUM_RADIO_LABELS 2
    const char *radioLabels[NUM_RADIO_LABELS] = { "SOFT", "HARD" };
    
    guiHelper.CreateRadioButtons(this, pGraphics,
                                 RADIOBUTTON_MODE_ID,
                                 RADIOBUTTON_MODE_FN,
                                 kRadioButtonsModeFrames,
                                 kRadioButtonsModeX, kRadioButtonsModeY,
                                 kRadioButtonModeNumButtons,
                                 kRadioButtonModeVSize, kMode, false, "MODE",
                                 RADIOBUTTON_DIFFUSE_ID, RADIOBUTTON_DIFFUSE_FN,
                                 IText::kAlignNear, IText::kAlignNear, radioLabels, NUM_RADIO_LABELS);
#endif
    
    // New code
    // Precision
    double defaultPrecision = 0.0; // Soft
    mPrecision = defaultPrecision;
    
    GetParam(kPrecision)->InitDouble("SoftHard", defaultPrecision, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_PRECISION_ID, KNOB_PRECISION_FN, kKnobPrecisionFrames,
                          kPrecisionX, kPrecisionY, kPrecision, "SOFT/HARD",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
    
    // Vocal precision
#if !SA_API
    
#if !HIDE_SENSITIVITY
    double defaultVocalSensitivity = DEFAULT_SENSITIVITY*100.0;
    mVocalSensitivity = defaultVocalSensitivity;
    
    GetParam(kVocalSensitivity)->InitDouble("VocalSensitivity",
                                            defaultVocalSensitivity, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_VOCAL_SENSITIVITY_ID,
                          KNOB_VOCAL_SENSITIVITY_FN,
                          kKnobVocalSensitivityFrames,
                          kVocalSensitivityX, kVocalSensitivityY, kVocalSensitivity,
                          "SENSITIVITY",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
#endif
    
#endif
    
    // Bass precision
#if !SA_API
    
#if !HIDE_SENSITIVITY
    double defaultBassSensitivity = DEFAULT_SENSITIVITY*100.0;
    mBassSensitivity = defaultBassSensitivity;
    
    GetParam(kBassSensitivity)->InitDouble("BassSensitivity",
                                           defaultBassSensitivity, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_BASS_SENSITIVITY_ID, KNOB_BASS_SENSITIVITY_FN,
                          kKnobBassSensitivityFrames,
                          kBassSensitivityX, kBassSensitivityY, kBassSensitivity,
                          "SENSITIVITY",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
#endif
    
#endif
    
    // Drums precision
#if !SA_API

#if !HIDE_SENSITIVITY
    double defaultDrumsSensitivity = DEFAULT_SENSITIVITY*100.0;
    mDrumsSensitivity = defaultDrumsSensitivity;
    
    GetParam(kDrumsSensitivity)->InitDouble("DrumsSensitivity",
                                            defaultDrumsSensitivity, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_DRUMS_SENSITIVITY_ID, KNOB_DRUMS_SENSITIVITY_FN,
                          kKnobDrumsSensitivityFrames,
                          kDrumsSensitivityX, kDrumsSensitivityY,
                          kDrumsSensitivity, "SENSITIVITY",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
#endif
    
#endif
    
    // Other precision
#if !SA_API
    
#if !HIDE_SENSITIVITY
    double defaultOtherSensitivity = DEFAULT_SENSITIVITY*100.0;
    mOtherSensitivity = defaultOtherSensitivity;
    
    GetParam(kOtherSensitivity)->InitDouble("OtherSensitivity",
                                            defaultOtherSensitivity, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_OTHER_SENSITIVITY_ID, KNOB_OTHER_SENSITIVITY_FN,
                          kKnobOtherSensitivityFrames,
                          kOtherSensitivityX, kOtherSensitivityY, kOtherSensitivity,
                          "SENSITIVITY",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
#endif
    
#endif
    
#if SA_API // SA_API
    
    // File selector "Open Mix"
    GetParam(kFileOpenMixParam)->InitInt("FileOpenMix", 0, 0, 1);
    GetParam(kFileOpenMixParam)->SetIsMeta(true);
    
    guiHelper.CreateRolloverButton(this, pGraphics,
                                   OPEN_MIX_BUTTON_ID, OPEN_MIX_BUTTON_FN,
                                   kFileOpenMixFrames,
                                   kFileOpenMixX, kFileOpenMixY, kFileOpenMixParam,
                                   "Open Mix...");
    
    // File selector "Open Source"
    GetParam(kFileOpenSourceParam)->InitInt("FileOpenSource", 0, 0, 1);
    GetParam(kFileOpenSourceParam)->SetIsMeta(true);
    
    guiHelper.CreateRolloverButton(this, pGraphics,
                                   OPEN_SOURCE_BUTTON_ID, OPEN_SOURCE_BUTTON_FN,
                                   kFileOpenSourceFrames,
                                   kFileOpenSourceX, kFileOpenSourceY, kFileOpenSourceParam,
                                   "Open Source...");
    
    
    // Generate button
    GetParam(kGenerateParam)->InitInt("Generate", 0, 0, 1);
    GetParam(kGenerateParam)->SetIsMeta(true);
    
    guiHelper.CreateRolloverButton(this, pGraphics,
                                   GENERATE_BUTTON_ID, GENERATE_BUTTON_FN,
                                   kGenerateFrames, kGenerateX, kGenerateY,
                                   kGenerateParam,
                                   "Generate");
    
#endif
    
  // Version
  guiHelper.CreateVersion(this, pGraphics, VST3_VER_STR, GUIHelper9::BOTTOM);
  
  // Set the shadows before graph.
  // So the graphs won't receive shadows
  guiHelper.CreateShadows(this, pGraphics, SHADOWS_ID, SHADOWS_FN);
  
  // Logo
  //guiHelper.CreateLogo(this, pGraphics, LOGO_ID, LOGO_FN, GUIHelper9::BOTTOM);
  guiHelper.CreateLogoAnim(this, pGraphics, LOGO_ID, LOGO_FN, GUIHelper9::BOTTOM);
    
  // Plugin name
  guiHelper.CreatePlugName(this, pGraphics, PLUGNAME_ID, PLUGNAME_FN, GUIHelper9::BOTTOM);
  
  // Help button
  guiHelper.CreateHelpButton(this, pGraphics, HELP_BUTTON_ID, HELP_BUTTON_FN,
                             MANUAL_ID, MANUAL_FN);
  
  TrialMode6::SetTrialMessage(this, pGraphics, &guiHelper);
  
#if USE_DEBUG_GRAPH
  DebugGraph::Create(this, pGraphics, kDebugGraph, &guiHelper, 11, BUFFER_SIZE/2);
#endif

  guiHelper.AddAllObjects(pGraphics);
  
#if 0 // Debug
  pGraphics->ShowControlBounds(true);
#endif
  
  // Demo mode
  mDemoManager.Init(this, pGraphics);
  
  AttachGraphics(pGraphics);
  
  // Init after AttachGraphics, because we need pGraphics in init
  // (for resource path)
  Init(OVERSAMPLING, FREQ_RES);
    
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  // Useful especially when using sidechain
  IPlug::NameBusses();
    
#if PROFILE_RNN
    BlaTimer::Reset(&__timer0, &__timerCount0);
#endif
    
  BL_PROFILE_RESET;
}


Rebalance::~Rebalance()
{
    for (int i = 0; i < 4; i++)
    {
        if (mFftObjs[i] != NULL)
            delete mFftObjs[i];
    }
    
#if SA_API
  for (int i = 0; i < 4; i++)
  {
      if (mRebalanceDumpFftObjs[i] != NULL)
          delete mRebalanceDumpFftObjs[i];
  }
#else
    for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
    {
        if (mRebalanceProcessFftObjs[i] != NULL)
            delete mRebalanceProcessFftObjs[i];
    }
    
    if (mMaskPred != NULL)
        delete mMaskPred;
#endif
    
    if (mDbgMixSpectrogram != NULL)
        delete mDbgMixSpectrogram;
    
    for (int k = 0; k < 4; k++)
    {
        if (mDbgSourceSpectrograms[k] != NULL)
            delete mDbgSourceSpectrograms[k];
    }
}


void
Rebalance::Init(int oversampling, int freqRes)
{
#if SA_API
    InitStandalone(oversampling, freqRes);
#else // Plugin
    InitPlug(oversampling, freqRes);
#endif
}

void
Rebalance::InitStandalone(int oversampling, int freqRes)
{
    double sampleRate = GetSampleRate();
    
    if (mFftObjs[0] == NULL)
    {
        int numChannels = 1;
        int numScInputs = 1;
        
        
        vector<ProcessObj *> processObjs[4];
        
        for (int i = 0; i < 4; i++)
        {
            mRebalanceDumpFftObjs[i] = new RebalanceDumpFftObj(BUFFER_SIZE);
            processObjs[i].push_back(mRebalanceDumpFftObjs[i]);

            mFftObjs[i] = new FftProcessObj15(processObjs[i],
                                              numChannels, numScInputs,
                                              BUFFER_SIZE, oversampling, freqRes,
                                              sampleRate);
            
#if !VARIABLE_HANNING
            mFftObjs[i]->SetAnalysisWindow(FftProcessObj15::ALL_CHANNELS,
                                           FftProcessObj15::WindowHanning);
            mFftObjs[i]->SetSynthesisWindow(FftProcessObj15::ALL_CHANNELS,
                                            FftProcessObj15::WindowHanning);
#else
            mFftObjs[i]->SetAnalysisWindow(FftProcessObj15::ALL_CHANNELS,
                                           FftProcessObj15::WindowVariableHanning);
            mFftObjs[i]->SetSynthesisWindow(FftProcessObj15::ALL_CHANNELS,
                                            FftProcessObj15::WindowVariableHanning);
#endif
        
            mFftObjs[i]->SetKeepSynthesisEnergy(FftProcessObj15::ALL_CHANNELS,
                                            KEEP_SYNTHESIS_ENERGY);
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            mFftObjs[i]->Reset(oversampling, freqRes, mSampleRate);
        }
    }
    
#if DUMP_DATASET_FILES
    GenerateMSD100();
#endif
}

void
Rebalance::InitPlug(int oversampling, int freqRes)
{
  double sampleRate = GetSampleRate();
  
#if FORCE_SAMPLE_RATE
    sampleRate = SAMPLE_RATE;
#endif
    
  if (mFftObjs[0] == NULL)
  {
    int numChannels = 2;
      
    int numScInputs = 0;
    
    vector<ProcessObj *> processObjs;
      
    IGraphics *graphics = GetGUI();
    mMaskPred = new MaskPredictor(BUFFER_SIZE, oversampling,
                                  freqRes, sampleRate,
                                  graphics);

#if FORCE_SAMPLE_RATE
    double plugSampleRate = GetSampleRate();
    mMaskPred->SetPlugSampleRate(plugSampleRate);
#endif
      
    for (int i = 0; i < numChannels; i++)
    {
        RebalanceProcessFftObj *obj = new RebalanceProcessFftObj(BUFFER_SIZE, mMaskPred);
        
#if FORCE_SAMPLE_RATE
        double plugSampleRate = GetSampleRate();
        obj->SetPlugSampleRate(plugSampleRate);
#endif
        
#if DBG_PRED_SPECTRO_DUMP
        if (i == 0)
            obj->DBG_SetSpectrogramDump(true);
#endif
        mRebalanceProcessFftObjs.push_back(obj);
          
        processObjs.push_back(obj);
    }
      
    mFftObjs[0] = new FftProcessObj15(processObjs,
                                      numChannels, numScInputs,
                                      BUFFER_SIZE, oversampling, freqRes,
                                      sampleRate);
#if !VARIABLE_HANNING
    mFftObjs[0]->SetAnalysisWindow(FftProcessObj15::ALL_CHANNELS,
                                   FftProcessObj15::WindowHanning);
    mFftObjs[0]->SetSynthesisWindow(FftProcessObj15::ALL_CHANNELS,
                                    FftProcessObj15::WindowHanning);
#else
    mFftObjs[0]->SetAnalysisWindow(FftProcessObj15::ALL_CHANNELS,
                                   FftProcessObj15::WindowVariableHanning);
    mFftObj[0]->SetSynthesisWindow(FftProcessObj15::ALL_CHANNELS,
                                   FftProcessObj15::WindowVariableHanning);
#endif
      
    mFftObjs[0]->SetKeepSynthesisEnergy(FftProcessObj15::ALL_CHANNELS,
                                        KEEP_SYNTHESIS_ENERGY);
      
    // For plugin processing
    mFftObjs[0]->AddMultichannelProcess(mMaskPred);
  }
  else
  {
    mFftObjs[0]->Reset(oversampling, freqRes, mSampleRate);
      
#if FORCE_SAMPLE_RATE
      double plugSampleRate = GetSampleRate();
      mMaskPred->SetPlugSampleRate(plugSampleRate);
      
      for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
      {
          mRebalanceProcessFftObjs[i]->SetPlugSampleRate(plugSampleRate);
      }
#endif
  }
}

void
Rebalance::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  
  BL_PROFILE_BEGIN;
    
#if SA_API
    // If we are in standalone application mode, we will want to load
    // sound files using buttons and to generate the out audio file and binary masks
    // in one step (not in ProcessDoubleReplacing)
    return;
#endif
    
  IPlug::CheckNeedReset();
    
  IPlug::UpdateConnections(nFrames);
  
  vector<WDL_TypedBuf<double> > in;
  vector<WDL_TypedBuf<double> > scIn;
  vector<WDL_TypedBuf<double> > out;
  Utils::GetPlugIOBuffers(this, inputs, outputs, nFrames,
                          &in, &scIn, &out);
  
  // Warning: there is a bug in Logic EQ plugin:
  // - when not playing, ProcessDoubleReplacing is still called continuously
  // - and the values are not zero ! (1e-5 for example)
  // This is the same for Protools, and if the plugin consumes, this slows all without stop
  // For example when selecting "offline"
  // Can be the case if we switch to the offline quality option:
  // All slows down, and Protools or Logix doesn't prompt for insufficient resources
  
  
  // Warning: there is a bug in Logic EQ plugin:
  // - when not playing, ProcessDoubleReplacing is still called continuously
  // - and the values are not zero ! (1e-5 for example)
  bool allZero = Utils::PlugIOAllZero(in, out);
  if (!allZero)
  {
    mSecureRestarter.Process(in);
    
    // If we have only one channel, duplicate it
    // (simpler for the rest...) 
    if (in.size() == 1)
    {
      in.resize(2);
      in[1] = in[0];
    }
      
#if PROFILE_RNN
      BlaTimer::Start(&__timer0);
#endif
      
    mFftObjs[0]->Process(in, scIn, &out);
      
#if PROFILE_RNN
      BlaTimer::StopAndDump(&__timer0, &__timerCount0,
                            "profile.txt", "timer0: %ld ms");
#endif
      
    Utils::PlugCopyOutputs(out, outputs, nFrames);
  }
  else
    // For Protools which makes buzz when not playing
    // Output the same thing as input
    Utils::BypassPlug(inputs, outputs, nFrames);
    
  // Demo mode
  if (mDemoManager.MustProcess())
  {
    mDemoManager.Process(outputs, nFrames);
  }
  
    BL_PROFILE_END;
}

void
Rebalance::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  // Logic X has a bug: when it restarts after stop,
  // sometimes it provides full volume directly, making a sound crack
  mSecureRestarter.Reset();
  
  // Called when we restart the playback
  // The cursor position may have changed
  // Then we must reset
  
  double sampleRate = GetSampleRate();
    
#if FORCE_SAMPLE_RATE
    sampleRate = SAMPLE_RATE;
#endif

  for (int i = 0; i < 4; i++)
  {
      if (mFftObjs[i] != NULL)
          //mFftObjs[i]->Reset();
          mFftObjs[i]->Reset(OVERSAMPLING, FREQ_RES, sampleRate); // NEW
  }
    
#if SA_API
  for (int i = 0; i < 4; i++)
  {
      if (mRebalanceDumpFftObjs[i] != NULL)
          //mRebalanceDumpFftObjs[i]->Reset();
          mRebalanceDumpFftObjs[i]->Reset(OVERSAMPLING, FREQ_RES, sampleRate); // NEW
  }
#else
  for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
  {
        if (mRebalanceProcessFftObjs[i] != NULL)
            //mRebalanceProcessFftObjs[i]->Reset();
            mRebalanceProcessFftObjs[i]->Reset(OVERSAMPLING, FREQ_RES, sampleRate); // NEW
  }
#endif
    
#if FORCE_SAMPLE_RATE
    double plugSampleRate = GetSampleRate();
    mMaskPred->SetPlugSampleRate(plugSampleRate);
    
    for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
    {
        mRebalanceProcessFftObjs[i]->SetPlugSampleRate(plugSampleRate);
    }
#endif

    
#if PROFILE_RNN
    BlaTimer::Reset(&__timer0, &__timerCount0);
#endif

  BL_PROFILE_RESET;
}

void
Rebalance::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  switch (paramIdx)
  {
#if !SA_API
    case kVocal:
    {        
      mVocal = GetParam(paramIdx)->DBToAmp();
      
      for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
      {
          if (mRebalanceProcessFftObjs[i] != NULL)
              mRebalanceProcessFftObjs[i]->SetVocal(mVocal);
      }
      
      GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
    
    case kBass:
    {
        mBass = GetParam(paramIdx)->DBToAmp();
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetBass(mBass);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
         
    case kDrums:
    {        
        mDrums = GetParam(paramIdx)->DBToAmp();
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetDrums(mDrums);
        }
          
          GUIHelper9::UpdateText(this, paramIdx);
      }
    break;
          
    case kOther:
    {
        mOther = GetParam(paramIdx)->DBToAmp();
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetOther(mOther);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
      }
    break;
       
    case kMode:
    {
        Mode mode = (Mode)GetParam(kMode)->Int();
          
        mMode = mode;
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetMode(mMode);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
      }
    break;
    
    // Global precision
    case kPrecision:
    {
        double precision = GetParam(paramIdx)->Value();
          
        mPrecision = precision/100.0;
          
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetPrecision(mPrecision);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
          
#if !HIDE_SENSITIVITY
    case kVocalSensitivity:
    {
        double vocalSensitivity = GetParam(paramIdx)->Value();
        
        mVocalSensitivity = 1.0 - vocalSensitivity/100.0;
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetVocalSensitivity(mVocalSensitivity);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
    
    case kBassSensitivity:
    {
        double bassSensitivity = GetParam(paramIdx)->Value();
        
        mBassSensitivity = 1.0 - bassSensitivity/100.0;
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetBassSensitivity(mBassSensitivity);
        }
        
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
    
    case kDrumsSensitivity:
    {
        double drumsSensitivity = GetParam(paramIdx)->Value();
        
        mDrumsSensitivity = 1.0 - drumsSensitivity/100.0;
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetDrumsSensitivity(mDrumsSensitivity);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
    
    case kOtherSensitivity:
    {
        double otherSensitivity = GetParam(paramIdx)->Value();
        
        mOtherSensitivity = 1.0 - otherSensitivity/100.0;
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetOtherSensitivity(mOtherSensitivity);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
#endif
          
#else // SA_API
    case kFileOpenMixParam:
    {
        int value = GetParam(paramIdx)->Value();
        if (value == 1)
        {
            WDL_String fileName;
              
            bool fileOk = GUIHelper9::PromptForFile(this, kFileOpen, &fileName, "", "wav aif");
            if (fileOk)
                OpenMixFile(fileName.Get());
              
            // The action is done, reset the button
            GUIHelper9::ResetParameter(this, paramIdx);
          }
      }
    break;
    
    case kFileOpenSourceParam:
    {
        int value = GetParam(paramIdx)->Value();
        if (value == 1)
        {
            WDL_String fileName;
              
            bool fileOk = GUIHelper9::PromptForFile(this, kFileOpen, &fileName, "", "wav aif");
            if (fileOk)
                OpenSourceFile(fileName.Get(), &mCurrentSourceChannels0[0]);
              
            // The action is done, reset the button
            GUIHelper9::ResetParameter(this, paramIdx);
        }
    }
    break;
          
    case kGenerateParam:
    {
        int value = GetParam(paramIdx)->Value();
        if (value == 1)
        {
            Generate();
              
            // The action is done, reset the button
            GUIHelper9::ResetParameter(this, paramIdx);
              
            // Seems to freeze Protools
            //GUIHelper9::UpdateText(this, paramIdx);
        }
    }
    break;
#endif
          
    default:
      break;
  }
}

bool
Rebalance::OpenMixFile(const char *fileName)
{
#ifndef WIN32
    // NOTE: Would crash when opening very large files
    // under the debugger, and using Scheme -> Guard Edge/ Guard Malloc
   
    IMutexLock lock(this);
    
    // Clear previous loaded file if any
    //mCurrentMixChannels.clear();
    
    vector<WDL_TypedBuf<double> > mixChannels;
    
    // Open the audio file
    double sampleRate = GetSampleRate();
    AudioFile *audioFile = AudioFile::Load(fileName, &mixChannels);
    if (audioFile == NULL)
    {
        return false;
    }
                            
    audioFile->Resample(sampleRate);
    
    if (!mixChannels.empty())
        mCurrentMixChannel0.Add(mixChannels[0].Get(), mixChannels[0].GetSize());
    
    delete audioFile;
 
#endif

    return true;
}

bool
Rebalance::OpenSourceFile(const char *fileName, WDL_TypedBuf<double> *result)
{
#ifndef WIN32

    // NOTE: Would crash when opening very large files
    // under the debugger, and using Scheme -> Guard Edge/ Guard Malloc
    
    IMutexLock lock(this);
    
    vector<WDL_TypedBuf<double> > sourceChannels;
    
    // Open the audio file
    double sampleRate = GetSampleRate();
    AudioFile *audioFile = AudioFile::Load(fileName, &sourceChannels);
    if (audioFile == NULL)
    {
        return false;
    }
    
    audioFile->Resample(sampleRate);
    
    if (!sourceChannels.empty())
        result->Add(sourceChannels[0].Get(), sourceChannels[0].GetSize());
    
    delete audioFile;
 
#endif WIN32

    return true;
}

long
Rebalance::Generate()
{
    // NOTE: Take only the left channel
    //
    if (mCurrentMixChannel0.GetSize() != mCurrentSourceChannels0[0].GetSize())
        return 0;
    
    long pos = 0;
    while(pos < mCurrentMixChannel0.GetSize() - BUFFER_SIZE)
    {
        for (int i = 0; i < 4; i++)
        {
            vector<WDL_TypedBuf<double> > in;
            in.resize(1);
            in[0].Add(&mCurrentMixChannel0.Get()[pos], BUFFER_SIZE);
        
            vector<WDL_TypedBuf<double> > sc;
            sc.resize(1);
            sc[0].Add(&mCurrentSourceChannels0[i].Get()[pos], BUFFER_SIZE);
        
            // Process
            mFftObjs[i]->Process(in, sc, NULL);
        }
        
        ComputeAndDump();
        
        // TEST: try to consume earlier to avoid disk swapping
        // NOTE: this is time-consuming to consume left big buffers
	//
	// But this is horribly slow to resize file buffers avery 1024 samples
	// (took 24 hours to dump 100 tracks with data overlap 4)
	//
        ConsumeBuffers(BUFFER_SIZE);
        
        //pos += BUFFER_SIZE;
    }
    
    return pos;
}

void
Rebalance::ConsumeBuffers(long numToConsume)
{
    Utils::ConsumeLeft(&mCurrentMixChannel0, numToConsume);
    
    for (int i = 0; i < 4; i++)
    {
        Utils::ConsumeLeft(&mCurrentSourceChannels0[i], numToConsume);
    }
}

void
Rebalance::GenerateMSD100()
{
    // Get the tracks list
    vector<string> tracks;
    
    std::ifstream input(TRACK_LIST_FILE);
    std::string line;
    
    while(std::getline(input, line))
    {
        tracks.push_back(line);
    }
    
    for (int i = 0; i < tracks.size(); i++)
    {
        const char *track = tracks[i].c_str();
        
        char message0[2048];
        sprintf(message0, "dump: %d/%d", i + 1, (int)tracks.size());
        Debug::AppendMessage("dump.txt", message0);
        
        // Open the mix file
        char mixPath[1024];
        sprintf(mixPath, "%s/%s/mixture.wav", MIXTURES_PATH, track);
        
        OpenMixFile(mixPath);
        
        // Open the vocal source path
        char sourcePath0[1024];
        sprintf(sourcePath0, "%s/%s/%s", SOURCE_PATH, track, VOCALS_FILE_NAME);
        OpenSourceFile(sourcePath0, &mCurrentSourceChannels0[0]);
        
        // Open the bass source path
        char sourcePath1[1024];
        sprintf(sourcePath1, "%s/%s/%s", SOURCE_PATH, track, BASS_FILE_NAME);
        OpenSourceFile(sourcePath1, &mCurrentSourceChannels0[1]);
        
        // Open the vocal source path
        char sourcePath2[1024];
        sprintf(sourcePath2, "%s/%s/%s", SOURCE_PATH, track, DRUMS_FILE_NAME);
        OpenSourceFile(sourcePath2, &mCurrentSourceChannels0[2]);
        
        // Open the vocal source path
        char sourcePath3[1024];
        sprintf(sourcePath3, "%s/%s/%s", SOURCE_PATH, track, OTHER_FILE_NAME);
        OpenSourceFile(sourcePath3, &mCurrentSourceChannels0[3]);
        
        // Generate
        long numProcessed = Generate();
        //ConsumeBuffers(numProcessed);
    
        Debug::AppendMessage("dump.txt", "ok\n");
    }
}

void
Rebalance::ComputeAndDump()
{
    vector<RebalanceDumpFftObj::Slice> slices[4];
    for (int k = 0; k < 4; k++)
    {
        // Get spectrogram slice for source
        mRebalanceDumpFftObjs[k]->GetSlices(&slices[k]);
        mRebalanceDumpFftObjs[k]->ResetSlices();
    }
    
    for (int i = 0; i < slices[0].size(); i++)
    {
        vector<WDL_TypedBuf<double> > sourceData[4];
        vector<WDL_TypedBuf<double> > mixData;
        
        for (int k = 0; k < 4; k++)
        {
            const RebalanceDumpFftObj::Slice &slice = slices[k][i];
            slice.GetData(&mixData, &sourceData[k]);
        }
        
        Downsample(&mixData);
        
        for (int k = 0; k < 4; k++)
            Downsample(&sourceData[k]);
        
        // Compute normalized mask (normalized vs binary)
        //NormalizeSourceSlices(sourceData);
        
        // Normalize each fft bin
        // (not tested)
        //NormalizeSourceSlicesFreq(sourceData); // TEST
        
        // Compute binary mask (normalized vs binary)
        BinarizeSourceSlices(sourceData);
        
        if (mDbgSpectrogramDump)
        {
            DBG_SpectroAddLines(mDbgMixSpectrogram, mixData);
            
            for (int k = 0; k < 4; k++)
                DBG_SpectroAddLines(mDbgSourceSpectrograms[k], sourceData[k]);
        }
        
        // TODO: here, can binarize masks
        
        // Dump mix (several columns)
        for (int j = 0; j < mixData.size(); j++)
            Utils::AppendValuesFileBinFloat(MIX_SAVE_FILE, mixData[j]);
        
        // Dump sources (last column)
        Utils::AppendValuesFileBinFloat(MASK_SAVE_FILE_VOCAL, sourceData[0][sourceData[0].size()-1]);
        Utils::AppendValuesFileBinFloat(MASK_SAVE_FILE_BASS, sourceData[1][sourceData[1].size()-1]);
        Utils::AppendValuesFileBinFloat(MASK_SAVE_FILE_DRUMS, sourceData[2][sourceData[2].size()-1]);
        Utils::AppendValuesFileBinFloat(MASK_SAVE_FILE_OTHER, sourceData[3][sourceData[3].size()-1]);
    }
    
    static int count = 0;
    if (count++ == 50)
    {
        DBG_SaveSpectrograms();
    }
}

void
Rebalance::NormalizeSourceSlices(vector<WDL_TypedBuf<double> > sourceData[4])
{
    for (int i = 0; i < sourceData[0].size(); i++)
    {
        for (int j = 0; j < sourceData[0][i].GetSize(); j++)
        {
            double sum = 0.0;
            for (int k = 0; k < 4; k++)
            {
                double val = sourceData[k][i].Get()[j];
                
                sum += val;
            }
            
            if (fabs(sum) > EPS)
            {
                for (int k = 0; k < 4; k++)
                {
                    double val = sourceData[k][i].Get()[j];
                    
                    val /= sum;
                    
                    sourceData[k][i].Get()[j] = val;
                }
            }
        }
    }
}

void
Rebalance::NormalizeSourceSlicesFreq(vector<WDL_TypedBuf<double> > sourceData[4])
{
    for (int i = 0; i < sourceData[0].size(); i++)
    {
        for (int k = 0; k < 4; k++)
        {
            double sum = 0.0;
            for (int j = 0; j < sourceData[0][i].GetSize(); j++)
            {
                double val = sourceData[k][i].Get()[j];
                
                sum += val;
            }
            
            if (fabs(sum) > EPS)
            {
                for (int j = 0; j < sourceData[0][i].GetSize(); j++)
                {
                    double val = sourceData[k][i].Get()[j];
                    
                    val /= sum;
                    
                    sourceData[k][i].Get()[j] = val;
                }
            }
        }
    }
}

void
Rebalance::BinarizeSourceSlices(vector<WDL_TypedBuf<double> > sourceData[4])
{
    for (int i = 0; i < sourceData[0].size(); i++)
    {
        for (int j = 0; j < sourceData[0][i].GetSize(); j++)
        {
            int maxIndex = 0;
            double maxValue = sourceData[0][i].Get()[j];
            
            for (int k = 1; k < 4; k++)
            {
                double val = sourceData[k][i].Get()[j];
                
                if (val > maxValue)
                {
                    maxValue = val;
                    maxIndex = k;
                }
            }
            
            for (int k = 0; k < 4; k++)
            {
                double val = (maxIndex == k) ? 1.0 : 0.0;
                
                sourceData[k][i].Get()[j] = val;
            }
        }
    }
}

void
Rebalance::Downsample(vector<WDL_TypedBuf<double> > *lines)
{
    for (int i = 0; i < lines->size(); i++)
    {
        WDL_TypedBuf<double> &data = (*lines)[i];
        MaskPredictor::Downsample(&data, mSampleRate);
    }
}
    
void
Rebalance::DBG_SaveSpectrograms()
{
    if (mDbgSpectrogramDump)
    {
        mDbgMixSpectrogram->SavePPM("learn-mix.ppm");
        
        mDbgSourceSpectrograms[0]->SavePPM("learn-source0.ppm");
        mDbgSourceSpectrograms[1]->SavePPM("learn-source1.ppm");
        mDbgSourceSpectrograms[2]->SavePPM("learn-source2.ppm");
        mDbgSourceSpectrograms[3]->SavePPM("learn-source3.ppm");
    }
}

void
Rebalance::DBG_SpectroAddLines(DbgSpectrogram *spectro,
                               const vector<WDL_TypedBuf<double> > &lines)
{
    for (int i = 0; i < lines.size(); i++)
    {
        const WDL_TypedBuf<double> &line = lines[i];
            
        spectro->AddLine(line);
    }
}

