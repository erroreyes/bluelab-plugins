#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <vector>
#include <deque>
#include <fstream>
#include <string>
using namespace std;

#include <FftProcessObj14.h>

#include <GUIHelper9.h>
#include <SecureRestarter.h>

#include <Utils.h>
#include <Debug.h>

#include <PPMFile.h>

#include <BlaTimer.h>

#include <TrialMode6.h>

#include <AudioFile.h>

// DNN
#include <pt_model.h>
#include <pt_tensor.h>

#include "Rebalance.h"

#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

// Same config as in article Simpson & Roma
#define BUFFER_SIZE 2048
#define OVERSAMPLING 4 //32

#define FREQ_RES 1

#define KEEP_SYNTHESIS_ENERGY 0

// When set to 0, we are accurate for frequencies
#define VARIABLE_HANNING 0

// Saving

#define DUMP_FILES 1

// Generation parameters
#define TRACK_LIST_FILE "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Tracks.txt"
#define MIXTURES_PATH "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Mixtures"
#define SOURCE_PATH "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Sources"

#define VOCALS_FILE_NAME "vocals.wav"
#define BASS_FILE_NAME   "bass.wav"
#define DRUMS_FILE_NAME  "drums.wav"
#define OTHER_FILE_NAME  "other.wav"

#define SOURCE_FILENAME VOCALS_FILE_NAME
//#define SOURCE_FILENAME BASS_FILE_NAME
//#define SOURCE_FILENAME DRUMS_FILE_NAME
//#define SOURCE_FILENAME OTHER_FILE_NAME

#define MIX_SAVE_FILE "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.0/DeepLearning/mix.dat"
#define MASK_SAVE_FILE "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.0/DeepLearning/mask.dat"

#define MASK_SAVE_FILE_VOCAL "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.0/DeepLearning/mask0.dat"
#define MASK_SAVE_FILE_BASS "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.0/DeepLearning/mask1.dat"
#define MASK_SAVE_FILE_DRUMS "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.0/DeepLearning/mask2.dat"
#define MASK_SAVE_FILE_OTHER "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.5.0/DeepLearning/mask3.dat"


// Save binary files
#define SAVE_BINARY 1

// Save in float format inside of double
#define SAVE_FLOATS 1


#define NUM_FFT_COLS_CUT 2 //4 //2 TEST //4// 1 // 2 //8 //20

// Resample buffers and masks
#define RESAMPLE_FACTOR 4 //8 //4 TEST


// Models
// (was "models/...")
//#define MODEL_VOCAL "dnn-model-vocal.model"
//#define MODEL_BASS "dnn-model-bass.model"
//#define MODEL_DRUMS "dnn-model-drums.model"
//#define MODEL_OTHER "dnn-model-other.model"
#define MODEL_VOCAL "dnn-model-source0.model"
#define MODEL_BASS "dnn-model-source1.model"
#define MODEL_DRUMS "dnn-model-source2.model"
#define MODEL_OTHER "dnn-model-source3.model"


#define DEBUG_DUMP_PPM 0 //1

#define MONO_PROCESS_TEST 0 //1

#define LSTM 0

// Listen to what the network is listening
#define DEBUG_BYPASS 0

// Do not predict other, but keep the remaining
#define OTHER_IS_REST 0

#define DEFAULT_PRECISION 0.0

// Dump "normalized" masks
// i.e each mask contains the contribution value, compared to the 3 other masks
#define DUMP_NORMALIZED_MASKS 1

#if 0
IDEA: 256x4, but only 256 as output (the network will make the mean)

TODO: gains in dB ??
TODO: load resource from memory (for windows)

TODO (?): rnn training: fix overfitting in vocal (make 2x more data with deque)

TEST: test OMP_NUM_THREADS

NOTE: for compiling with VST3 => created config of VST3 "base.xcodeproj" => separate directory
NOTE: switched to C++11 for pocket-tensor & OSX 10.7 + added __SSE4_1__ preprocessor macro
added __SSSE3__ too

NOTE: PocketTensor: PT_LOOP_UNROLLING_ENABLE to 1 => 15% perf gain

SALE: "in real time" !
SALE: attenuate bleeding (sometimes) (to test !)

NOTE: 2x256 seems better than 4x128
- no phasing
- no perf slow-down

NOTE: oversampling 32 seems to give same results as oversampling 4
#endif

const int kNumPrograms = 1;

enum EParams
{
    kVocal = 0,
    kBass,
    kDrums,
    kOther,
    
    kVocalPrecision,
    kBassPrecision,
    kDrumsPrecision,
    kOtherPrecision,
    
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
    
    kRadioButtonsModeX = 444,
    kRadioButtonsModeY = 68,
    kRadioButtonModeVSize = 42,
    kRadioButtonModeNumButtons = 2,
    kRadioButtonsModeFrames = 2,
    
    // Precisions
    kVocalPrecisionX = 52,
    kVocalPrecisionY = 190,
    kKnobVocalPrecisionFrames = 180,
    
    kBassPrecisionX = 152,
    kBassPrecisionY = 190,
    kKnobBassPrecisionFrames = 180,
    
    kDrumsPrecisionX = 252,
    kDrumsPrecisionY = 190,
    kKnobDrumsPrecisionFrames = 180,
    
    kOtherPrecisionX = 352,
    kOtherPrecisionY = 190,
    kKnobOtherPrecisionFrames = 180,
    
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
    RebalanceDumpFftObj(int bufferSize);
    
    virtual ~RebalanceDumpFftObj();
    
    virtual void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                  const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
    
#if DUMP_NORMALIZED_MASKS
    // Addition, for normalized masks
    
    // Get spectrogram slice for mix
    void GetMixSlices(vector<vector<WDL_TypedBuf<double> > > *mixSlices);
    
    // Get spectrogram slice for source
    void GetSourceSlices(vector<vector<WDL_TypedBuf<double> > > *sourceSlices);
    
    void ResetSlices();
#endif
    
    static void ColumnsToBuffer(WDL_TypedBuf<double> *buf,
                                /*const */vector<WDL_TypedBuf<double> > &cols);
    
protected:
    void ComputeMask(WDL_TypedBuf<double> *mask,
                     const WDL_TypedBuf<double> &magnsMix,
                     const WDL_TypedBuf<double> &magnsSource);
    
    void Downsample(WDL_TypedBuf<double> *ioBuf);
    
    int mNumCols;
    
    vector<WDL_TypedBuf<double> > mMixCols;
    vector<WDL_TypedBuf<double> > mMaskCols;
    
#if DUMP_NORMALIZED_MASKS
    // Addition, for normalized masks
    
    vector<WDL_TypedBuf<double> > mSourceCols;
    
    vector<vector<WDL_TypedBuf<double> > > mMixSlices;
    
    vector<vector<WDL_TypedBuf<double> > > mSourceSlices;
#endif
};

RebalanceDumpFftObj::RebalanceDumpFftObj(int bufferSize)
: ProcessObj(bufferSize)
{
    mNumCols = 0;
}

RebalanceDumpFftObj::~RebalanceDumpFftObj() {}

void
RebalanceDumpFftObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                      const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
    if (scBuffer == NULL)
        return;
    
    if (mNumCols >= NUM_FFT_COLS_CUT)
    {
#if !DUMP_NORMALIZED_MASKS // Normal behaviour
        WDL_TypedBuf<double> mixBuf;
        WDL_TypedBuf<double> maskBuf;
        
        ColumnsToBuffer(&mixBuf, mMixCols);
        ColumnsToBuffer(&maskBuf, mMaskCols);
        
#if !SAVE_BINARY // Save text
        Utils::AppendValuesFile(MIX_SAVE_FILE, mixBuf, ',');
        Utils::AppendValuesFile(MASK_SAVE_FILE, maskBuf, ',');
#else
#if !SAVE_FLOATS // Save double
        Utils::AppendValuesFileBin(MIX_SAVE_FILE, mixBuf);
        Utils::AppendValuesFileBin(MASK_SAVE_FILE, maskBuf);
#else // Save floats
        WDL_TypedBuf<float> mixBufFloat;
        mixBufFloat.Resize(mixBuf.GetSize());
        for (int i = 0; i < mixBuf.GetSize(); i++)
        {
            float val = mixBuf.Get()[i];
            mixBufFloat.Get()[i] = val;
        }
        
        Utils::AppendValuesFileBin(MIX_SAVE_FILE, mixBufFloat);
        
        WDL_TypedBuf<float> maskBufFloat;
        maskBufFloat.Resize(maskBuf.GetSize());
        for (int i = 0; i < maskBuf.GetSize(); i++)
        {
            float val = maskBuf.Get()[i];
            maskBufFloat.Get()[i] = val;
        }
        
        Utils::AppendValuesFileBin(MASK_SAVE_FILE, maskBufFloat);
#endif
        
#endif
#endif
        
#if DEBUG_DUMP_PPM
        //static int count = 0;
        //if (count++ == 20)
        {
            PPMFile::SavePPM("mix.ppm",
                             mixBuf.Get(),
                             mixBuf.GetSize()/NUM_FFT_COLS_CUT, NUM_FFT_COLS_CUT,
                             1, // bpp
                             255.0*128.0 // coeff
                             );
        
            Debug::DumpData("mix-buf.txt", mixBuf);
            
            PPMFile::SavePPM("mask.ppm",
                             maskBuf.Get(),
                             maskBuf.GetSize()/NUM_FFT_COLS_CUT, NUM_FFT_COLS_CUT,
                             1, // bpp
                             255.0 // coeff
                             );
            
            Debug::DumpData("mask-buf.txt", maskBuf);
        }
#endif
        
#if DUMP_NORMALIZED_MASKS
        mMixSlices.push_back(mMixCols);
        mSourceSlices.push_back(mSourceCols);
        
        mSourceCols.clear();
#endif
        
        mMixCols.clear();
        mMaskCols.clear();
        mNumCols = 0;
    }
    
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    Utils::ComplexToMagn(&magnsMix, mixBuffer);
    
    WDL_TypedBuf<double> magnsMixDown = magnsMix;
    Downsample(&magnsMixDown);
    
    mMixCols.push_back(magnsMixDown);
    
    // Source
    WDL_TypedBuf<WDL_FFT_COMPLEX> sourceBuffer = *scBuffer;
    Utils::TakeHalf(&sourceBuffer);
    
    WDL_TypedBuf<double> magnsSource;
    Utils::ComplexToMagn(&magnsSource, sourceBuffer);
    
    // Mask
    //
    // Here, we compute a an accurate mask over downsampled data
    // (intuitively, should be better)
    WDL_TypedBuf<double> magnsSourceDown = magnsSource;
    Downsample(&magnsSourceDown);
    
#if DUMP_NORMALIZED_MASKS
    mSourceCols.push_back(magnsSourceDown);
#endif
    
    WDL_TypedBuf<double> mask;
    ComputeMask(&mask, magnsMixDown, magnsSourceDown);
    
    mMaskCols.push_back(mask);

    mNumCols++;
}

void
RebalanceDumpFftObj::GetMixSlices(vector<vector<WDL_TypedBuf<double> > > *mixSlices)
{
    *mixSlices = mMixSlices;
}

void
RebalanceDumpFftObj::GetSourceSlices(vector<vector<WDL_TypedBuf<double> > >  *sourceSlices)
{
    *sourceSlices = mSourceSlices;
}

void
RebalanceDumpFftObj::ResetSlices()
{
    mMixSlices.clear();
    mSourceSlices.clear();
}

void
RebalanceDumpFftObj::ComputeMask(WDL_TypedBuf<double> *mask,
                                 const WDL_TypedBuf<double> &magnsMix,
                                 const WDL_TypedBuf<double> &magnsSource)
{
    mask->Resize(magnsMix.GetSize());
    
    for (int i = 0; i < mask->GetSize(); i++)
    {
        double mix = magnsMix.Get()[i];
        double source = magnsSource.Get()[i];
        
        double result = (source > mix) ? 1.0 : 0.0;
        
        mask->Get()[i] = result;
    }
}

void
RebalanceDumpFftObj::ColumnsToBuffer(WDL_TypedBuf<double> *buf,
                                     /*const */vector<WDL_TypedBuf<double> > &cols)
{
    if (cols.empty())
        return;
    
    buf->Resize(cols.size()*cols[0].GetSize());
    
    for (int i = 0; i < cols.size(); i++)
    {
        const WDL_TypedBuf<double> &col = cols[i];
        for (int j = 0; j < col.GetSize(); j++)
        {
            buf->Get()[j + i*col.GetSize()] = col.Get()[j];
        }
    }
}

void
RebalanceDumpFftObj::Downsample(WDL_TypedBuf<double> *ioBuf)
{
    double hzPerBin = ((double)mSampleRate)/(BUFFER_SIZE/2);
    WDL_TypedBuf<double> bufMel;
    Utils::FreqsToMelNorm(&bufMel, *ioBuf, hzPerBin);
    *ioBuf = bufMel;
 
    if (RESAMPLE_FACTOR == 1)
        // No need to downsample
        return;
    
    int newSize = ioBuf->GetSize()/RESAMPLE_FACTOR;
    Utils::ResizeLinear(ioBuf, newSize);
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
                  const char *resourcePath);
    
    virtual ~MaskPredictor();

    void Reset();
    
    void Reset(int overlapping, int oversampling, double sampleRate);
    
    void ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples);
    
    // Get the masks
    void GetMaskVocal(WDL_TypedBuf<double> *maskVocal);
    
    void GetMaskBass(WDL_TypedBuf<double> *maskBass);
    
    void GetMaskDrums(WDL_TypedBuf<double> *maskDrums);
    
    void GetMaskOther(WDL_TypedBuf<double> *maskOther);
    
protected:
    void Downsample(WDL_TypedBuf<double> *ioBuf);
    
    void Upsample(WDL_TypedBuf<double> *ioBuf);
    
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
};

MaskPredictor::MaskPredictor(int bufferSize,
                             double overlapping, double oversampling,
                             double sampleRate,
                             const char *resourcePath)
{
    mBufferSize = bufferSize;
    mOverlapping = overlapping;
    mOversampling = oversampling;
    
    mSampleRate = sampleRate;
    
    // DNN
    
    // Vocal
    char modelVocalFileName[2048];
    sprintf(modelVocalFileName, "%s/%s", resourcePath, MODEL_VOCAL);
    mModelVocal = pt::Model::create(modelVocalFileName);
    
    // Vocal
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
    
    for (int i = 0; i < NUM_FFT_COLS_CUT; i++)
    {
        WDL_TypedBuf<double> col;
        Utils::ResizeFillZeros(&col, bufferSize/(2*RESAMPLE_FACTOR));
        
        mMixCols.push_back(col);
    }
}

MaskPredictor::~MaskPredictor() {}

void
MaskPredictor::Reset()
{
    Reset(mOverlapping, mOversampling, mSampleRate);
}

void
MaskPredictor::Reset(int overlapping, int oversampling, double sampleRate)
{
    mOverlapping = overlapping;
    mOversampling = oversampling;
    
    mSampleRate = sampleRate;
}

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
    Downsample(&magnsDown);
    
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
MaskPredictor::Downsample(WDL_TypedBuf<double> *ioBuf)
{
    double hzPerBin = ((double)mSampleRate)/(BUFFER_SIZE/2);
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
MaskPredictor::Upsample(WDL_TypedBuf<double> *ioBuf)
{
    if (RESAMPLE_FACTOR != 1)
    {
        int newSize = ioBuf->GetSize()*RESAMPLE_FACTOR;
        
        Utils::ResizeLinear(ioBuf, newSize);
    }
    
    double hzPerBin = ((double)mSampleRate)/(BUFFER_SIZE/2);
    WDL_TypedBuf<double> bufMel;
    Utils::MelToFreqsNorm(&bufMel, *ioBuf, hzPerBin);
    *ioBuf = bufMel;
}

// Split the mask cols, get each mask and upsample it
void
MaskPredictor::UpsamplePredictedMask(WDL_TypedBuf<double> *ioBuf)
{
    if (RESAMPLE_FACTOR == 1)
        // No need to upsample
        return;
    
    WDL_TypedBuf<double> result;
    
    for (int j = 0; j < NUM_FFT_COLS_CUT; j++)
    {
        WDL_TypedBuf<double> mask;
        mask.Resize(BUFFER_SIZE/(2*RESAMPLE_FACTOR));
        
#if 0
        for (int i = 0; i < mask.GetSize(); i++)
        {
            int id = i + j*mask.GetSize();
            
            if (id >= ioBuf->GetSize())
                // Just in case
                break;
            
            mask.Get()[i] = ioBuf->Get()[id];
        }
#endif
        
        // Optim
        memcpy(mask.Get(),
               &ioBuf->Get()[j*mask.GetSize()],
               mask.GetSize()*sizeof(double));
        
        Upsample(&mask);
        
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
    
    // Check (just in case)
    if (out.getSize() != mixBufHisto.GetSize())
        return;
    
    WDL_TypedBuf<double> maskBuf;
    maskBuf.Resize(mixBufHisto.GetSize());
    for (int i = 0; i < maskBuf.GetSize(); i++)
    {
        maskBuf.Get()[i] = out(i);
    }
    
#if DEBUG_DUMP_PPM
    //static int count = 0;
    //if (count++ == 20)
    {
        PPMFile::SavePPM("mix-proc.ppm",
                         mixBufHisto.Get(),
                         mixBufHisto.GetSize()/NUM_FFT_COLS_CUT, NUM_FFT_COLS_CUT,
                         1, // bpp
                         255.0*128.0 // coeff
                         );
        
        Debug::DumpData("mix-buf-res.txt", mixBufHisto);
        
        PPMFile::SavePPM("mask-proc.ppm",
                         maskBuf.Get(),
                         maskBuf.GetSize()/NUM_FFT_COLS_CUT, NUM_FFT_COLS_CUT,
                         1, // bpp
                         255.0 // coeff
                         );
        
        Debug::DumpData("mask-buf-res.txt", maskBuf);
    }
#endif
    
    UpsamplePredictedMask(&maskBuf);
    
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
    int numCols = NUM_FFT_COLS_CUT;
    
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


// RebalanceProcessFftObj
//
// Apply the masks to the current channels
//
class RebalanceProcessFftObj : public ProcessObj
{
public:
    RebalanceProcessFftObj(int bufferSize, MaskPredictor *maskPred);
    
    virtual ~RebalanceProcessFftObj();
    
    virtual void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                  const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
    
    void SetVocal(double vocal);
    void SetBass(double bass);
    void SetDrums(double drums);
    void SetOther(double other);
    
    void SetVocalPrecision(double vocalPrecision);
    void SetBassPrecision(double bassPrecision);
    void SetDrumsPrecision(double drumsPrecision);
    void SetOtherPrecision(double otherPrecision);
    
    void SetMode(Rebalance::Mode mode);
    
    void ApplyPrecision(double *maskVocal, double *maskBass,
                        double *maskDrums, double *maskOther);
    
protected:
    void ComputeLineMasks0(WDL_TypedBuf<double> *maskMixResult,
                           WDL_TypedBuf<double> *maskSourceResult,
                           const WDL_TypedBuf<double> &maskSource,
                           double alpha);
    
    void ComputeRemixedMagns(WDL_TypedBuf<double> *resultMagns,
                             const WDL_TypedBuf<double> &magnsMix,
                             const WDL_TypedBuf<double> &maskMix,
                             const WDL_TypedBuf<double> &maskSource,
                             double mixFactor, double otherFactor);

   
#if 0 // Not used anymore ? (method0 ?)
    void ComputeSourceMagns(WDL_TypedBuf<double> *result,
                            const WDL_TypedBuf<double> &mixBufHisto,
                            const WDL_TypedBuf<double> &mixBuffer,
                            std::unique_ptr<pt::Model> &model,
                            double ratio, double confidence);
#endif
    
#if 0
    // Debug: listen to the audio sent to the dnn
    void DBG_ListenBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                          const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
#endif
    
    //void ComputeMixMethod0(WDL_TypedBuf<double> *resultMagns,
    //                       const WDL_TypedBuf<double> &magnsMix);

    void ComputeMixSoft(WDL_TypedBuf<double> *resultMagns,
                        const WDL_TypedBuf<double> &magnsMix);
    
    void ComputeMixHard(WDL_TypedBuf<double> *resultMagns,
                        const WDL_TypedBuf<double> &magnsMix);
    
    MaskPredictor *mMaskPred;
    
    // Parameters
    double mVocal;
    double mBass;
    double mDrums;
    double mOther;
    
    double mVocalPrecision;
    double mBassPrecision;
    double mDrumsPrecision;
    double mOtherPrecision;
    
    Rebalance::Mode mMode;
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
    
    mVocalPrecision = 0.0;
    mBassPrecision = 0.0;
    mDrumsPrecision = 0.0;
    mOtherPrecision = 0.0;
    
    mMode = Rebalance::SOFT;
}

RebalanceProcessFftObj::~RebalanceProcessFftObj() {}

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
RebalanceProcessFftObj::SetVocalPrecision(double vocalPrecision)
{
    mVocalPrecision = vocalPrecision;
}

void
RebalanceProcessFftObj::SetBassPrecision(double bassPrecision)
{
    mBassPrecision = bassPrecision;
}

void
RebalanceProcessFftObj::SetDrumsPrecision(double drumsPrecision)
{
    mDrumsPrecision = drumsPrecision;
}

void
RebalanceProcessFftObj::SetOtherPrecision(double otherPrecision)
{
    mOtherPrecision = otherPrecision;
}

void
RebalanceProcessFftObj::SetMode(Rebalance::Mode mode)
{
    mMode = mode;
}

void
RebalanceProcessFftObj::ApplyPrecision(double *maskVocal, double *maskBass,
                                       double *maskDrums, double *maskOther)
{
    if (*maskVocal < mVocalPrecision)
        *maskVocal = 0.0;
    
    if (*maskBass < mBassPrecision)
        *maskBass = 0.0;
    
    if (*maskDrums < mDrumsPrecision)
        *maskDrums = 0.0;
    
    if (*maskOther < mOtherPrecision)
        *maskOther = 0.0;
}

void
RebalanceProcessFftObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                         const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
#if 0
#if DEBUG_BYPASS // Debug
    DBG_ListenBuffer(ioBuffer, scBuffer);

    return;
#endif
#endif
    
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    WDL_TypedBuf<double> phasesMix;
    Utils::ComplexToMagnPhase(&magnsMix, &phasesMix, mixBuffer);
    
    WDL_TypedBuf<double> resultMagns;
    
    if (mMode == Rebalance::SOFT)
        ComputeMixSoft(&resultMagns, magnsMix);
    else
        ComputeMixHard(&resultMagns, magnsMix);
    
    // Fill the result
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    Utils::MagnPhaseToComplex(&fftSamples, resultMagns, phasesMix);
    
    fftSamples.Resize(fftSamples.GetSize()*2);
    
    Utils::FillSecondFftHalf(&fftSamples);
    
    // Result
    *ioBuffer = fftSamples;
}

#if 0
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

    MaskPredictor::Downsample(&magnsMix);
    MaskPredictor::Upsample(&magnsMix);
    
    // Fill the result
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    Utils::MagnPhaseToComplex(&fftSamples, magnsMix, phasesMix);
    
    fftSamples.Resize(fftSamples.GetSize()*2);
    
    Utils::FillSecondFftHalf(&fftSamples);
    
    // Result
    *ioBuffer = fftSamples;
}
#endif

void
RebalanceProcessFftObj::ComputeLineMasks0(WDL_TypedBuf<double> *maskMixResult,
                                          WDL_TypedBuf<double> *maskSourceResult,
                                          const WDL_TypedBuf<double> &maskSource,
                                          double alpha)
{
    // See: https://www.researchgate.net/publication/275279991_Deep_Karaoke_Extracting_Vocals_from_Musical_Mixtures_Using_a_Convolutional_Deep_Neural_Network
    //
    int numFreqs = BUFFER_SIZE/2;
    int numCols = NUM_FFT_COLS_CUT;

    maskMixResult->Resize(numFreqs);
    maskSourceResult->Resize(numFreqs);
    
    for (int i = 0; i < numFreqs; i++)
    {
        double avgMaskSource = 0.0;

        for (int j = 0; j < numCols; j++)
        {
            int idx = i + j*numFreqs;
            
            double m = maskSource.Get()[idx];
            avgMaskSource += m;
        }
        
        avgMaskSource /= numCols;
        
        double source = (avgMaskSource > alpha) ? 1.0 : 0.0;
        maskSourceResult->Get()[i] = source;
        
        // Problem:
        // The values returned by the prediction are often in [0, 0.5]
        // then mix will almost every time to 1
        //
        double mix = (avgMaskSource < (1.0 - alpha) ) ? 1.0 : 0.0;
        maskMixResult->Get()[i] = mix;
    }
}

void
RebalanceProcessFftObj::ComputeRemixedMagns(WDL_TypedBuf<double> *resultMagns,
                                            const WDL_TypedBuf<double> &magnsMix,
                                            const WDL_TypedBuf<double> &maskMix,
                                            const WDL_TypedBuf<double> &maskSource,
                                            double mixFactor, double otherFactor)
{
    resultMagns->Resize(maskMix.GetSize());
    
    // Checks (just in case)
    if (maskMix.GetSize() != resultMagns->GetSize())
        return;
    
    if (maskSource.GetSize() != resultMagns->GetSize())
        return;
    
    if (magnsMix.GetSize() != resultMagns->GetSize())
        return;
    
    for (int i = 0; i < resultMagns->GetSize(); i++)
    {
        double mix = maskMix.Get()[i];
        double source = maskSource.Get()[i];
        
        double magn = magnsMix.Get()[i];
        
        // NOTE: not sure about all that... (return to see the article !)
        if (source > 0.0)
        {
            magn *= mixFactor;
        }
        
#if 0 // TEST
      // NOTE: since mix is almost everytime to 1, disable it for the moment
      // Gives better results (for the moment)
        if (mix > 0.0)
        {
            magn *= otherFactor;
        }
#endif
        
        resultMagns->Get()[i] = magn;
    }
}

#if 0
void
RebalanceProcessFftObj::ComputeSourceMagns(WDL_TypedBuf<double> *result,
                                           const WDL_TypedBuf<double> &mixBufHisto,
                                           const WDL_TypedBuf<double> &mixBuffer,
                                           std::unique_ptr<pt::Model> &model,
                                           double ratio, double confidence)
{
    // Do not compute using model prediction if the ratio is 1
    // (i.e no change)
#define EPS 1e-6
    if (fabs(ratio - 1.0) < EPS)
    {
        *result = mixBuffer;
        
        return;
    }
    
    WDL_TypedBuf<double> maskBuf;
    PredictMask(&maskBuf, mixBufHisto, model);
    
    WDL_TypedBuf<double> maskMix;
    WDL_TypedBuf<double> maskSource;

    // For the moment, we try only with vocal
    ComputeLineMasks0(&maskMix, &maskSource, maskBuf, confidence);

    ComputeRemixedMagns(result, mixBuffer, maskMix, maskSource, ratio, 0.0);
}
#endif

#if 0
void
RebalanceProcessFftObj::ComputeMixMethod0(WDL_TypedBuf<double> *resultMagns,
                                          const WDL_TypedBuf<double> &magnsMix)
{
    Utils::ResizeFillZeros(resultMagns, magnsMix.GetSize());
    
    WDL_TypedBuf<double> resultVocal;
    ComputeSourceMagns(&resultVocal, mixBufHisto,
                       magnsMix, mModelVocal, mVocal, mVocalConfidence);
    
    WDL_TypedBuf<double> resultBass;
    ComputeSourceMagns(&resultBass, mixBufHisto,
                       magnsMix, mModelBass, mBass, mBassConfidence);
    
    WDL_TypedBuf<double> resultDrums;
    ComputeSourceMagns(&resultDrums, mixBufHisto,
                       magnsMix, mModelDrums, mDrums, mDrumsConfidence);
    
    WDL_TypedBuf<double> resultOther;
    ComputeSourceMagns(&resultOther, mixBufHisto,
                       magnsMix, mModelOther, mOther, mOtherConfidence);
    
    for (int i = 0; i < resultMagns->GetSize(); i++)
    {
        double vocal = resultVocal.Get()[i];
        double bass = resultBass.Get()[i];
        double drums = resultDrums.Get()[i];
        double other = resultOther.Get()[i];
        
        double result = vocal; // + bass + drums + other;
        
        // TODO
        
        resultMagns->Get()[i] = result;
    }
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
        
        ApplyPrecision(&maskVocal0, &maskBass0,
                       &maskDrums0, &maskOther0);

        
#define EPS 1e-8
        
        // Normalize
        double sum = maskVocal0 + maskBass0 + maskDrums0 + maskOther0;
        if (fabs(sum) > EPS)
        {
            maskVocal0 /= sum;
            maskBass0 /= sum;
            maskDrums0 /= sum;
            maskOther0 /= sum;
        }
        
#if OTHER_IS_REST
        maskOther0 = 1.0 - (maskVocal0 + maskBass0 + maskDrums0);
#endif
        
        double vocalCoeff = maskVocal0*mVocal;
        double bassCoeff = maskBass0*mBass;
        double drumsCoeff = maskDrums0*mDrums;
        double otherCoeff = maskOther0*mOther;
        
        double coeff = vocalCoeff + bassCoeff + drumsCoeff + otherCoeff;
        
        double val = magnsMix.Get()[i];
        
        //resultMagns->Get()[i] = val * coeff;
        resultMagns->Get()[i] = coeff/50.0; // TEST !!!!!!!!!!
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
       
        //ApplyPrecision(&maskVocal0, &maskBass0,
        //               &maskDrums0, &maskOther0);
        
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
        
#define EPS 1e-8
        
        double val = magnsMix.Get()[i];
        
        resultMagns->Get()[i] = val * coeffMax;
    }
}

////

Rebalance::Rebalance(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;
    
  GUIHelper9 guiHelper;
  
  // Init WDL FFT
  FftProcessObj14::Init();

  for (int i = 0; i < 4; i++)
    mFftObjs[i] = NULL;

#if SA_API
  for (int i = 0; i < 4; i++)
    mRebalanceDumpFftObjs[i] = NULL;
#else
  mMaskPred = NULL;
#endif
    
  // GUI
  
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);
  
  // Vocal
#if !SA_API
  double defaultVocal = 100.0;
  mVocal = defaultVocal;
  
  GetParam(kVocal)->InitDouble("Vocal", defaultVocal, 0.0, 200.0, 0.1, "%");
#endif
    
  guiHelper.CreateKnob3(this, pGraphics,
                       KNOB_VOCAL_ID, KNOB_VOCAL_FN, kKnobVocalFrames,
                       kVocalX, kVocalY, kVocal, "VOCAL",
                       KNOB_SHADOW_ID, KNOB_SHADOW_FN,
                       BLUE_KNOB_DIFFUSE_ID, BLUE_KNOB_DIFFUSE_FN,
                       TEXTFIELD_ID, TEXTFIELD_FN);
    
    // Bass
#if !SA_API
    double defaultBass = 100.0;
    mBass = defaultBass;
    
    GetParam(kBass)->InitDouble("Bass", defaultBass, 0.0, 200.0, 0.1, "%");
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_BASS_ID, KNOB_BASS_FN, kKnobBassFrames,
                          kBassX, kBassY, kBass, "BASS",
                          KNOB_SHADOW_ID, KNOB_SHADOW_FN,
                          BLUE_KNOB_DIFFUSE_ID, BLUE_KNOB_DIFFUSE_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN);
    
    // Drums
#if !SA_API
    double defaultDrums = 100.0;
    mDrums = defaultDrums;

    GetParam(kDrums)->InitDouble("Drums", defaultDrums, 0.0, 200.0, 0.1, "%");
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_DRUMS_ID, KNOB_DRUMS_FN, kKnobDrumsFrames,
                          kDrumsX, kDrumsY, kDrums, "DRUMS",
                          KNOB_SHADOW_ID, KNOB_SHADOW_FN,
                          BLUE_KNOB_DIFFUSE_ID, BLUE_KNOB_DIFFUSE_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN);
  
    // Other
#if !SA_API
    double defaultOther = 100.0;
    mOther = defaultOther;
    
    GetParam(kOther)->InitDouble("Other", defaultOther, 0.0, 200.0, 0.1, "%");
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_OTHER_ID, KNOB_OTHER_FN, kKnobOtherFrames,
                          kOtherX, kOtherY, kOther, "OTHER",
                          KNOB_SHADOW_ID, KNOB_SHADOW_FN,
                          BLUE_KNOB_DIFFUSE_ID, BLUE_KNOB_DIFFUSE_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN);

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
    
    // Vocal precision
#if !SA_API
    double defaultVocalPrecision = DEFAULT_PRECISION;
    mVocalPrecision = defaultVocalPrecision;
    
    GetParam(kVocalPrecision)->InitDouble("VocalPrecision", defaultVocalPrecision, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_VOCAL_PRECISION_ID, KNOB_VOCAL_PRECISION_FN, kKnobVocalPrecisionFrames,
                          kVocalPrecisionX, kVocalPrecisionY, kVocalPrecision, "PRECISION",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
#endif
    
    // Bass precision
#if !SA_API
    double defaultBassPrecision = DEFAULT_PRECISION;
    mBassPrecision = defaultBassPrecision;
    
    GetParam(kBassPrecision)->InitDouble("BassPrecision", defaultBassPrecision, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_BASS_PRECISION_ID, KNOB_BASS_PRECISION_FN, kKnobBassPrecisionFrames,
                          kBassPrecisionX, kBassPrecisionY, kBassPrecision, "PRECISION",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
#endif
    
    // Drums precision
#if !SA_API
    double defaultDrumsPrecision = DEFAULT_PRECISION;
    mDrumsPrecision = defaultDrumsPrecision;
    
    GetParam(kDrumsPrecision)->InitDouble("DrumsPrecision", defaultDrumsPrecision, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_DRUMS_PRECISION_ID, KNOB_DRUMS_PRECISION_FN, kKnobDrumsPrecisionFrames,
                          kDrumsPrecisionX, kDrumsPrecisionY, kDrumsPrecision, "PRECISION",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
#endif
    
    // Other precision
#if !SA_API
    double defaultOtherPrecision = DEFAULT_PRECISION;
    mOtherPrecision = defaultOtherPrecision;
    
    GetParam(kOtherPrecision)->InitDouble("OtherPrecision", defaultOtherPrecision, 0.0, 100.0, 0.1, "%");
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_OTHER_PRECISION_ID, KNOB_OTHER_PRECISION_FN, kKnobOtherPrecisionFrames,
                          kOtherPrecisionX, kOtherPrecisionY, kOtherPrecision, "PRECISION",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
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
  guiHelper.CreateLogo(this, pGraphics, LOGO_ID, LOGO_FN, GUIHelper9::BOTTOM);
  
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
}


void
Rebalance::Init(int oversampling, int freqRes)
{
#if SA_API
    
#if !DUMP_NORMALIZED_MASKS
    InitSaBinaryMasks(oversampling, freqRes);
#else
    InitSaNormMasks(oversampling, freqRes);
#endif
    
#else // Plugin
    InitPlug(oversampling, freqRes);
#endif
}

void
Rebalance::InitSaBinaryMasks(int oversampling, int freqRes)
{
    double sampleRate = GetSampleRate();
    
    if (mFftObjs[0] == NULL)
    {
        int numChannels = 1;
        int numScInputs = 1;
        
        vector<ProcessObj *> processObjs;
        
        mRebalanceDumpFftObjs[0] = new RebalanceDumpFftObj(BUFFER_SIZE);
        processObjs.push_back(mRebalanceDumpFftObjs[0]);

        
        mFftObjs[0] = new FftProcessObj14(processObjs,
                                          numChannels, numScInputs,
                                          BUFFER_SIZE, oversampling, freqRes,
                                          sampleRate);
#if !VARIABLE_HANNING
        mFftObjs[0]->SetAnalysisWindow(FftProcessObj14::ALL_CHANNELS,
                                       FftProcessObj14::WindowHanning);
        mFftObjs[0]->SetSynthesisWindow(FftProcessObj14::ALL_CHANNELS,
                                        FftProcessObj14::WindowHanning);
#else
        mFftObjs[0]->SetAnalysisWindow(FftProcessObj14::ALL_CHANNELS,
                                       FftProcessObj14::WindowVariableHanning);
        mFftObjs[0]->SetSynthesisWindow(FftProcessObj14::ALL_CHANNELS,
                                        FftProcessObj14::WindowVariableHanning);
#endif
        
        mFftObjs[0]->SetKeepSynthesisEnergy(FftProcessObj14::ALL_CHANNELS,
                                            KEEP_SYNTHESIS_ENERGY);
    }
    else
    {
        mFftObjs[0]->Reset(oversampling, freqRes, mSampleRate);
    }
    
#if DUMP_FILES
    //OpenMixFile("/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.4.0/DeepLearning/Audio/Mix.wav");
    //OpenSourceFile("/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.4.0/DeepLearning/Audio/Vocal.wav");
    //Generate();
    
    GenerateMSD100();
#endif
}

void
Rebalance::InitSaNormMasks(int oversampling, int freqRes)
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

            mFftObjs[i] = new FftProcessObj14(processObjs[i],
                                              numChannels, numScInputs,
                                              BUFFER_SIZE, oversampling, freqRes,
                                              sampleRate);
            
#if !VARIABLE_HANNING
            mFftObjs[i]->SetAnalysisWindow(FftProcessObj14::ALL_CHANNELS,
                                           FftProcessObj14::WindowHanning);
            mFftObjs[i]->SetSynthesisWindow(FftProcessObj14::ALL_CHANNELS,
                                            FftProcessObj14::WindowHanning);
#else
            mFftObjs[i]->SetAnalysisWindow(FftProcessObj14::ALL_CHANNELS,
                                           FftProcessObj14::WindowVariableHanning);
            mFftObjs[i]->SetSynthesisWindow(FftProcessObj14::ALL_CHANNELS,
                                            FftProcessObj14::WindowVariableHanning);
#endif
        
            mFftObjs[i]->SetKeepSynthesisEnergy(FftProcessObj14::ALL_CHANNELS,
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
    
#if DUMP_FILES
    GenerateMSD100Norm();
#endif
}

void
Rebalance::InitPlug(int oversampling, int freqRes)
{
  double sampleRate = GetSampleRate();
  
  if (mFftObjs[0] == NULL)
  {
    int numChannels = 2;
      
#if MONO_PROCESS_TEST
    numChannels = 1;
#endif
      
    int numScInputs = 0;
    
    vector<ProcessObj *> processObjs;
      
    IGraphics *graphics = GetGUI();
    WDL_String resPath;
    graphics->GetResourceDir(&resPath);
    
    mMaskPred = new MaskPredictor(BUFFER_SIZE, oversampling,
                                  freqRes, sampleRate,
                                  resPath.Get());
      
    for (int i = 0; i < numChannels; i++)
    {
        RebalanceProcessFftObj *obj = new RebalanceProcessFftObj(BUFFER_SIZE, mMaskPred);
        mRebalanceProcessFftObjs.push_back(obj);
          
        processObjs.push_back(obj);
    }
      
    mFftObjs[0] = new FftProcessObj14(processObjs,
                                      numChannels, numScInputs,
                                      BUFFER_SIZE, oversampling, freqRes,
                                      sampleRate);
#if !VARIABLE_HANNING
    mFftObjs[0]->SetAnalysisWindow(FftProcessObj14::ALL_CHANNELS,
                                   FftProcessObj14::WindowHanning);
    mFftObjs[0]->SetSynthesisWindow(FftProcessObj14::ALL_CHANNELS,
                                    FftProcessObj14::WindowHanning);
#else
    mFftObjs[0]->SetAnalysisWindow(FftProcessObj14::ALL_CHANNELS,
                                   FftProcessObj14::WindowVariableHanning);
    mFftObj[0]->SetSynthesisWindow(FftProcessObj14::ALL_CHANNELS,
                                   FftProcessObj14::WindowVariableHanning);
#endif
      
    mFftObjs[0]->SetKeepSynthesisEnergy(FftProcessObj14::ALL_CHANNELS,
                                        KEEP_SYNTHESIS_ENERGY);
      
    // For plugin processing
    mFftObjs[0]->AddMultichannelProcess(mMaskPred);
  }
  else
  {
    mFftObjs[0]->Reset(oversampling, freqRes, mSampleRate);
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
      
#if MONO_PROCESS_TEST
    in.resize(1);
#endif
      
#if PROFILE_RNN
      BlaTimer::Start(&__timer0);
#endif
      
    mFftObjs[0]->Process(in, scIn, &out);
      
#if PROFILE_RNN
      BlaTimer::StopAndDump(&__timer0, &__timerCount0,
                            "profile.txt", "timer0: %ld ms");
#endif
      
#if MONO_PROCESS_TEST
    out[1] = out[0];
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
  
  for (int i = 0; i < 4; i++)
  {
      if (mFftObjs[i] != NULL)
          mFftObjs[i]->Reset();
  }
    
#if SA_API
  for (int i = 0; i < 4; i++)
  {
      if (mRebalanceDumpFftObjs[i] != NULL)
          mRebalanceDumpFftObjs[i]->Reset();
  }
#else
  for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
  {
        if (mRebalanceProcessFftObjs[i] != NULL)
            mRebalanceProcessFftObjs[i]->Reset();
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
      double vocal = GetParam(paramIdx)->Value();
      
      mVocal = vocal/100.0;
      
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
        double bass = GetParam(paramIdx)->Value();
          
        mBass = bass/100.0;
          
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
        double drums = GetParam(paramIdx)->Value();
          
        mDrums = drums/100.0;
        
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
        double other = GetParam(paramIdx)->Value();
        
        mOther = other/100.0;
        
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
     
    case kVocalPrecision:
    {
        double vocalPrecision = GetParam(paramIdx)->Value();
          
          mVocalPrecision = vocalPrecision/100.0;
          
          for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
          {
              if (mRebalanceProcessFftObjs[i] != NULL)
                  mRebalanceProcessFftObjs[i]->SetVocalPrecision(mVocalPrecision);
          }
          
          GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
    
    case kBassPrecision:
    {
        double bassPrecision = GetParam(paramIdx)->Value();
          
        mBassPrecision = bassPrecision/100.0;
          
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetBassPrecision(mBassPrecision);
        }
        
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
    
    case kDrumsPrecision:
    {
        double drumsPrecision = GetParam(paramIdx)->Value();
          
        mDrumsPrecision = drumsPrecision/100.0;
          
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetDrumsPrecision(mDrumsPrecision);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
    
    case kOtherPrecision:
    {
        double otherPrecision = GetParam(paramIdx)->Value();
          
        mOtherPrecision = otherPrecision/100.0;
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetOtherPrecision(mOtherPrecision);
        }
          
        GUIHelper9::UpdateText(this, paramIdx);
    }
    break;
    
          
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
                OpenSourceFile(fileName.Get());
              
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
    
    return true;
}

bool
Rebalance::OpenSourceFile(const char *fileName)
{
    // NOTE: Would crash when opening very large files
    // under the debugger, and using Scheme -> Guard Edge/ Guard Malloc
    
    IMutexLock lock(this);
    
    // Clear previous loaded file if any
    //mCurrentSourceChannels.clear();
    
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
        mCurrentSourceChannels0[0].Add(sourceChannels[0].Get(), sourceChannels[0].GetSize());
    
    delete audioFile;
    
    return true;
}

bool
Rebalance::OpenSourceFile2(const char *fileName, WDL_TypedBuf<double> *result)
{
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
    
    return true;
}

void
Rebalance::Generate()
{
    // NOTE: Take only the left channel
    //
    //if (mCurrentMixChannels.empty())
    //    return;
    
    //if (mCurrentSourceChannels.empty())
    //    return;
    
    //if (mCurrentMixChannels[0].GetSize() != mCurrentSourceChannels[0].GetSize())
    //    return;
    
    if (mCurrentMixChannel0.GetSize() != mCurrentSourceChannels0[0].GetSize())
        return;
    
    long pos = 0;
    while(pos < mCurrentMixChannel0/*s[0]*/.GetSize() - BUFFER_SIZE)
    {
        vector<WDL_TypedBuf<double> > in;
        in.resize(1);
        in[0].Add(&mCurrentMixChannel0/*s[0]*/.Get()[pos], BUFFER_SIZE);
        
        vector<WDL_TypedBuf<double> > sc;
        sc.resize(1);
        sc[0].Add(&mCurrentSourceChannels0[0]/*s[0]*/.Get()[pos], BUFFER_SIZE);
        
        // Process
        mFftObjs[0]->Process(in, sc, NULL);
        
        pos += BUFFER_SIZE;
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
        
        // Open the mix file
        char mixPath[1024];
        sprintf(mixPath, "%s/%s/mixture.wav", MIXTURES_PATH, track);
        
        OpenMixFile(mixPath);
        
        // Open the source path
        char sourcePath[1024];
        sprintf(sourcePath, "%s/%s/%s", SOURCE_PATH, track, SOURCE_FILENAME);
        OpenSourceFile(sourcePath);
    }
    
    // Generate
    Generate();
}

void
Rebalance::GenerateNorm()
{
    // NOTE: Take only the left channel
    //
    if (mCurrentMixChannel0.GetSize() != mCurrentSourceChannels0[0].GetSize())
        return;
    
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
        
        DumpNormFiles();
        
        pos += BUFFER_SIZE;
    }
}

void
Rebalance::GenerateMSD100Norm()
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
        
        // Open the mix file
        char mixPath[1024];
        sprintf(mixPath, "%s/%s/mixture.wav", MIXTURES_PATH, track);
        
        OpenMixFile(mixPath);
        
        // Open the vocal source path
        char sourcePath0[1024];
        sprintf(sourcePath0, "%s/%s/%s", SOURCE_PATH, track, VOCALS_FILE_NAME);
        OpenSourceFile2(sourcePath0, &mCurrentSourceChannels0[0]);
        
        // Open the bass source path
        char sourcePath1[1024];
        sprintf(sourcePath1, "%s/%s/%s", SOURCE_PATH, track, BASS_FILE_NAME);
        OpenSourceFile2(sourcePath1, &mCurrentSourceChannels0[1]);
        
        // Open the vocal source path
        char sourcePath2[1024];
        sprintf(sourcePath2, "%s/%s/%s", SOURCE_PATH, track, DRUMS_FILE_NAME);
        OpenSourceFile2(sourcePath2, &mCurrentSourceChannels0[2]);
        
        // Open the vocal source path
        char sourcePath3[1024];
        sprintf(sourcePath3, "%s/%s/%s", SOURCE_PATH, track, OTHER_FILE_NAME);
        OpenSourceFile2(sourcePath3, &mCurrentSourceChannels0[3]);
    }
    
    // Generate
    GenerateNorm();
}

void
Rebalance::DumpNormFiles()
{
    vector<vector<WDL_TypedBuf<double> > > mixSlices;
    vector<vector<WDL_TypedBuf<double> > > sourceSlices[4];
    
    for (int k = 0; k < 4; k++)
    {
        if (k == 0)
        {
            mRebalanceDumpFftObjs[k]->GetMixSlices(&mixSlices);
        }
        
        // Get spectrogram slice for source
        mRebalanceDumpFftObjs[k]->GetSourceSlices(&sourceSlices[k]);
        
        mRebalanceDumpFftObjs[k]->ResetSlices();
    }
    
    NormalizeSourceSlices(sourceSlices);
    
    for (int i = 0; i < mixSlices.size(); i++)
        // Dump the packets
    {
        WDL_TypedBuf<double> mixBuf;
        WDL_TypedBuf<double> sourceBufs[4];
        
        RebalanceDumpFftObj::ColumnsToBuffer(&mixBuf, mixSlices[i]);
        
        RebalanceDumpFftObj::ColumnsToBuffer(&sourceBufs[0], sourceSlices[0][i]);
        RebalanceDumpFftObj::ColumnsToBuffer(&sourceBufs[1], sourceSlices[1][i]);
        RebalanceDumpFftObj::ColumnsToBuffer(&sourceBufs[2], sourceSlices[2][i]);
        RebalanceDumpFftObj::ColumnsToBuffer(&sourceBufs[3], sourceSlices[3][i]);
        
        // Save floats
        WDL_TypedBuf<float> mixBufFloat;
        mixBufFloat.Resize(mixBuf.GetSize());
        for (int j = 0; j < mixBuf.GetSize(); j++)
        {
            float val = mixBuf.Get()[j];
            mixBufFloat.Get()[j] = val;
        }
        
        //Utils::AppendValuesFile(MIX_SAVE_FILE, mixBufFloat);
        Utils::AppendValuesFileBin(MIX_SAVE_FILE, mixBufFloat);
        
        WDL_TypedBuf<float> sourceBufsFloat[4];
        for (int k = 0; k < 4; k++)
        {
            sourceBufsFloat[k].Resize(sourceBufs[k].GetSize());
            
            for (int j = 0; j < sourceBufs[k].GetSize(); j++)
            {
                float val = sourceBufs[k].Get()[j];
                sourceBufsFloat[k].Get()[j] = val;
            }
        }
        
        //Utils::AppendValuesFile(MASK_SAVE_FILE_VOCAL, sourceBufsFloat[0]);
        //Utils::AppendValuesFile(MASK_SAVE_FILE_BASS, sourceBufsFloat[1]);
        //Utils::AppendValuesFile(MASK_SAVE_FILE_DRUMS, sourceBufsFloat[2]);
        //Utils::AppendValuesFile(MASK_SAVE_FILE_OTHER, sourceBufsFloat[3]);
        
        Utils::AppendValuesFileBin(MASK_SAVE_FILE_VOCAL, sourceBufsFloat[0]);
        Utils::AppendValuesFileBin(MASK_SAVE_FILE_BASS, sourceBufsFloat[1]);
        Utils::AppendValuesFileBin(MASK_SAVE_FILE_DRUMS, sourceBufsFloat[2]);
        Utils::AppendValuesFileBin(MASK_SAVE_FILE_OTHER, sourceBufsFloat[3]);
    }
}

void
Rebalance::NormalizeSourceSlices(vector<vector<WDL_TypedBuf<double> > > sourceSlices[4])
{
    if (sourceSlices[0].empty())
        return;
    
    for (int i = 0; i < sourceSlices[0].size(); i++)
    {
        if (sourceSlices[0][i].empty())
            break;
        
        for (int j = 0; j < sourceSlices[0][i].size(); j++)
        {
            for (int l = 0; l < sourceSlices[0][i][j].GetSize(); l++)
            {
                double sum = 0.0;
                for (int k = 0; k < 4; k++)
                {
                    double val = sourceSlices[k][i][j].Get()[l];
                
                    sum += val;
                }
            
#define EPS 1e-10
                if (fabs(sum) > EPS)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        double val = sourceSlices[k][i][j].Get()[l];
                    
                        val /= sum;
                    
                        sourceSlices[k][i][j].Get()[l] = val;
                    }
                }
            }
        }
    }
}
