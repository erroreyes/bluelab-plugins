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

#include <GraphControl10.h>

//#include <Oversampler3.h>

// DNN
#include <pt_model.h>
#include <pt_tensor.h>

#include "Rebalance.h"

#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

// Same config as in article Simpson & Roma
#define BUFFER_SIZE 2048
#define OVERSAMPLING 4 //4

#define FREQ_RES 1

#define KEEP_SYNTHESIS_ENERGY 0

// When set to 0, we are accurate for frequencies
#define VARIABLE_HANNING 0

// Saving

// Generation parameters
#define TRACK_LIST_FILE "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Tracks.txt"
#define MIXTURES_PATH "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Mixtures"
#define SOURCE_PATH "/Users/applematuer/Documents/Dev/Mixes-DeepLearning/MSD100/Sources"

#define VOCALS_FILE_NAME "vocals.wav"
#define BASS_FILE_NAME   "bass.wav"
#define DRUMS_FILE_NAME  "drums.wav"
#define OTHER_FILE_NAME  "other.wav"

//#define SOURCE_FILENAME VOCALS_FILE_NAME
//#define SOURCE_FILENAME BASS_FILE_NAME
//#define SOURCE_FILENAME DRUMS_FILE_NAME
#define SOURCE_FILENAME OTHER_FILE_NAME

#define MIX_SAVE_FILE "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.0.0/DeepLearning/mix.dat"
#define MASK_SAVE_FILE "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.0.0/DeepLearning/mask.dat"

// Save binary files
#define SAVE_BINARY 1

// Save in float format inside of double
#define SAVE_FLOATS 1


#define NUM_FFT_COLS_CUT 2 //4// 1 // 2 //8 //20

// Resample buffers and masks
#define RESAMPLE_FACTOR 4


#define DEBUG_MODELS 1

#if !DEBUG_MODELS
// Models
// (was "models/...")
#define MODEL_VOCAL "dnn-model-vocal.model"
#define MODEL_BASS "dnn-model-bass.model"
#define MODEL_DRUMS "dnn-model-drums.model"
#define MODEL_OTHER "dnn-model-other.model"
#else
#define MODEL_VOCAL "dnn-model-source0.model"
#define MODEL_BASS "dnn-model-source0.model"
#define MODEL_DRUMS "dnn-model-source0.model"
#define MODEL_OTHER "dnn-model-source0.model"
#endif

#define DEFAULT_CONFIDENCE 15.0

#define DEBUG_DUMP_PPM 0 //1 //0

#define MONO_PROCESS_TEST 0

#define PROFILE_RNN 0

#define LSTM 0

#define COMPUTE_PRECISE_MASK 1

#define DEBUG_BYPASS 1

#if 0

TODO:
- check by outputting exactly what is sent to the network (downsample then upsample)
- consider using 4x128 (+ vertical transientness)


TEST: test OMP_NUM_THREADS

TODO: checkbox soft/hard mask ?

IDEA: try to normalize the masks: sum of the 4 masks should be 1

TODO: test with several overlappings (quality), to avoid musical noise if possible
-> in this case, manage well the history in the deque, to gate the same separation
between the spectrogram bands

TODO: adjust the interface (enlarge it, diminish graph height,
                            adjust knobs horiz position) => for visibility

TODO: gains en dB ??

TODO: load resource from memory (for windows)

NOTE: for compiling with VST3 => created config of VST3 "base.xcodeproj" => separate directory
NOTE: switched to C++11 for pocket-tensor & OSX 10.7 + added __SSE4_1__ preprocessor macro
added __SSSE3__ too

NOTE: PocketTensor: PT_LOOP_UNROLLING_ENABLE to 1 => 15% perf gain

SALE: "in real time" !
#endif

// RebalanceDumpFftObj
class RebalanceDumpFftObj : public ProcessObj
{
public:
    RebalanceDumpFftObj(int bufferSize);
    
    virtual ~RebalanceDumpFftObj();
    
    virtual void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                  const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
    
protected:
    void ComputeMask(WDL_TypedBuf<double> *mask,
                     const WDL_TypedBuf<double> &magnsMix,
                     const WDL_TypedBuf<double> &magnsSource);
    
    void ColumnsToBuffer(WDL_TypedBuf<double> *buf,
                         const vector<WDL_TypedBuf<double> > &cols);
    
    void Downsample(WDL_TypedBuf<double> *ioBuf);
    
    int mNumCols;
    
    vector<WDL_TypedBuf<double> > mMixCols;
    vector<WDL_TypedBuf<double> > mMaskCols;
    
    //Oversampler3 mDownsampler;
};

RebalanceDumpFftObj::RebalanceDumpFftObj(int bufferSize)
: ProcessObj(bufferSize)//,
  //mDownsampler(RESAMPLE_FACTOR, false)
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
        WDL_TypedBuf<double> mixBuf;
        WDL_TypedBuf<double> maskBuf;
        
        ColumnsToBuffer(&mixBuf, mMixCols);
        ColumnsToBuffer(&maskBuf, mMaskCols);
        
#if !SAVE_BINARY
        Utils::AppendValuesFile(MIX_SAVE_FILE, mixBuf, ',');
        Utils::AppendValuesFile(MASK_SAVE_FILE, maskBuf, ',');
#else
#if !SAVE_FLOATS // Save double
        Utils::AppendValuesFileBin(MIX_SAVE_FILE, mixBuf);
        Utils::AppendValuesFileBin(MASK_SAVE_FILE, maskBuf);
#else
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
        
        mMixCols.resize(0);
        mMaskCols.resize(0);
        mNumCols = 0;
    }
    
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    Utils::ComplexToMagn(&magnsMix, mixBuffer);
    
    //Debug::DumpData("magns0.txt", magnsMix);
    
    WDL_TypedBuf<double> magnsMixDown = magnsMix;
    Downsample(&magnsMixDown);
    
    //Debug::DumpData("magns1.txt", magnsMixDown);
    
    mMixCols.push_back(magnsMixDown);
    
    // Source
    WDL_TypedBuf<WDL_FFT_COMPLEX> sourceBuffer = *scBuffer;
    Utils::TakeHalf(&sourceBuffer);
    
    WDL_TypedBuf<double> magnsSource;
    Utils::ComplexToMagn(&magnsSource, sourceBuffer);
    
    // Mask
#if !COMPUTE_PRECISE_MASK
    // NOTE: here, we downsample a binary mask, computed on full data
    // => this smoothes the binary values
    // Other possibility: compute a precise mask on already downsampled data
    
    WDL_TypedBuf<double> mask;
    ComputeMask(&mask, magnsMix, magnsSource);
    
    //Debug::DumpData("mask0.txt", mask);
    
    Downsample(&mask);
#else
    // Here, we compute a an accurate mask over downsampled data
    // (intuitively, should be better)
    WDL_TypedBuf<double> magnsSourceDown = magnsSource;
    Downsample(&magnsSourceDown);
    
    WDL_TypedBuf<double> mask;
    ComputeMask(&mask, magnsMixDown, magnsSourceDown);
#endif
    
    //Debug::DumpData("mask1.txt", mask);
    
    mMaskCols.push_back(mask);

    mNumCols++;
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
                                     const vector<WDL_TypedBuf<double> > &cols)
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
    //mDownsampler.Resample(ioBuf);
    
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

// RebalanceProcessFftObj
class RebalanceProcessFftObj : public ProcessObj
{
public:
    RebalanceProcessFftObj(int bufferSize, const char *resPath);
    
    virtual ~RebalanceProcessFftObj();
    
    virtual void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                  const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
    
    void SetVocal(double voice);
    
    void SetVocalConfidence(double voiceConfidence);
    
    void SetBass(double voice);
    
    void SetBassConfidence(double voiceConfidence);
    
    void SetDrums(double voice);
    
    void SetDrumsConfidence(double voiceConfidence);
    
    void SetOther(double voice);
    
    void SetOtherConfidence(double voiceConfidence);
    
protected:
    void ColumnsToBuffer(WDL_TypedBuf<double> *buf,
                         const deque<WDL_TypedBuf<double> > &cols);
    
    void ComputeLineMasks(WDL_TypedBuf<double> *maskMixResult,
                          WDL_TypedBuf<double> *maskSourceResult,
                          const WDL_TypedBuf<double> &maskSource,
                          double alpha);
    
    void ComputeRemixedMagns(WDL_TypedBuf<double> *resultMagns,
                             const WDL_TypedBuf<double> &magnsMix,
                             const WDL_TypedBuf<double> &maskMix,
                             const WDL_TypedBuf<double> &maskSource,
                             double mixFactor, double otherFactor);

    
    void ComputeSourceMagns(WDL_TypedBuf<double> *result,
                            const WDL_TypedBuf<double> &mixBufHisto,
                            const WDL_TypedBuf<double> &mixBuffer,
                            std::unique_ptr<pt::Model> &model,
                            double ratio, double confidence);
    
    void Downsample(WDL_TypedBuf<double> *ioBuf);
    
    void Upsample(WDL_TypedBuf<double> *ioBuf);
    
    void UpsamplePredictedMask(WDL_TypedBuf<double> *ioBuf);

    
    // Parameters
    double mVocal;
    double mVocalConfidence;
    
    double mBass;
    double mBassConfidence;
    
    double mDrums;
    double mDrumsConfidence;
    
    double mOther;
    double mOtherConfidence;
    
    // DNNs
    std::unique_ptr<pt::Model> mModelVocal;
    std::unique_ptr<pt::Model> mModelBass;
    std::unique_ptr<pt::Model> mModelDrums;
    std::unique_ptr<pt::Model> mModelOther;
    
    deque<WDL_TypedBuf<double> > mMixCols;
    
    //Oversampler3 mDownsampler;
    //Oversampler3 mUpsampler;
    
#if PROFILE_RNN
    BlaTimer BLTimer__;
    long BLTimer__Count;
#endif
};

RebalanceProcessFftObj::RebalanceProcessFftObj(int bufferSize,
                                               const char *resourcePath)
: ProcessObj(bufferSize)//,
  //mDownsampler(RESAMPLE_FACTOR, false),
  //mUpsampler(RESAMPLE_FACTOR, true)
{
    // Parameters
    mVocal = 0.0;
    mVocalConfidence = DEFAULT_CONFIDENCE/100.0;
    
    mBass = 0.0;
    mBassConfidence = DEFAULT_CONFIDENCE/100.0;
    
    mDrums = 0.0;
    mDrumsConfidence = DEFAULT_CONFIDENCE/100.0;
    
    mOther = 0.0;
    mOtherConfidence = DEFAULT_CONFIDENCE/100.0;
    
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
    
    // Otgher
    char modelOtherFileName[2048];
    sprintf(modelOtherFileName, "%s/%s", resourcePath, MODEL_OTHER);
    mModelOther = pt::Model::create(modelOtherFileName);
    
    for (int i = 0; i < NUM_FFT_COLS_CUT; i++)
    {
        WDL_TypedBuf<double> col;
        Utils::ResizeFillZeros(&col, bufferSize/(2*RESAMPLE_FACTOR));
        
        mMixCols.push_back(col);
    }
    
#if PROFILE_RNN
    BLTimer__.Reset();
    BLTimer__Count = 0;
#endif
}

RebalanceProcessFftObj::~RebalanceProcessFftObj() {}

void
RebalanceProcessFftObj::SetVocal(double vocal)
{
    mVocal = vocal;
}

void
RebalanceProcessFftObj::SetVocalConfidence(double vocalConfidence)
{
    mVocalConfidence = vocalConfidence;
}

void
RebalanceProcessFftObj::SetBass(double bass)
{
    mBass = bass;
}

void
RebalanceProcessFftObj::SetBassConfidence(double bassConfidence)
{
    mBassConfidence = bassConfidence;
}

void
RebalanceProcessFftObj::SetDrums(double drums)
{
    mDrums = drums;
}

void
RebalanceProcessFftObj::SetDrumsConfidence(double drumsConfidence)
{
    mDrumsConfidence = drumsConfidence;
}

void
RebalanceProcessFftObj::SetOther(double other)
{
    mOther = other;
}

void
RebalanceProcessFftObj::SetOtherConfidence(double otherConfidence)
{
    mOtherConfidence = otherConfidence;
}

void
RebalanceProcessFftObj::ColumnsToBuffer(WDL_TypedBuf<double> *buf,
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

#if !DEBUG_BYPASS // Normal
void
RebalanceProcessFftObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                         const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
#if PROFILE_RNN
    BLTimer__.Start();
#endif
    
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    WDL_TypedBuf<double> phasesMix;
    Utils::ComplexToMagnPhase(&magnsMix, &phasesMix, mixBuffer);
    
    //Debug::DumpData("magns0.txt", magnsMix);
    
    //double hzPerBin = 43.066; // TODO: do not hardcode this !
    //double hzPerBin = ((double)mSampleRate)/(BUFFER_SIZE/2);
    
    //WDL_TypedBuf<double> testMfcc;
    //Utils::FreqsToMelNorm(&testMfcc, magnsMix, hzPerBin);
    
    //Debug::DumpData("mfcc.txt", testMfcc);
    
    //WDL_TypedBuf<double> magns1;
    //Utils::MelToFreqsNorm(&magns1, testMfcc, hzPerBin);
    
    //Debug::DumpData("magns1.txt", magns1);
    
    //Debug::DumpData("magns0.txt", magnsMix);
    
    WDL_TypedBuf<double> magnsMixDown = magnsMix;
    Downsample(&magnsMixDown);
    
    //Debug::DumpData("magns1.txt", magnsMixDown);
    
    //WDL_TypedBuf<double> test = magnsMixDown;
    //Upsample(&test);
    
    //Debug::DumpData("magns2.txt", test);
    
    // mMixCols is filled with zeros at the origin
    mMixCols.push_back(magnsMixDown);
    mMixCols.pop_front();
    
    WDL_TypedBuf<double> mixBufHisto;
    ColumnsToBuffer(&mixBufHisto, mMixCols);
    
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
    
    WDL_TypedBuf<double> resultMagns;
    Utils::ResizeFillZeros(&resultMagns, magnsMix.GetSize());
    
    for (int i = 0; i < resultMagns.GetSize(); i++)
    {
        double vocal = resultVocal.Get()[i];
        double bass = resultBass.Get()[i];
        double drums = resultDrums.Get()[i];
        double other = resultOther.Get()[i];
        
        double result = vocal; // + bass + drums + other;
        
        resultMagns.Get()[i] = result;
    }
    
    
    // Fill the result
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    Utils::MagnPhaseToComplex(&fftSamples, resultMagns, phasesMix);
    
    fftSamples.Resize(fftSamples.GetSize()*2);
    //Utils::ResizeFillZeros(&fftSamples, ioBuffer->GetSize());
    
    Utils::FillSecondFftHalf(&fftSamples);
    
    // Result
    *ioBuffer = fftSamples;
    
#if PROFILE_RNN
    BLTimer__.Stop();
    
    if (BLTimer__Count++ > 50)
    {
        long t = BLTimer__.Get();
        
        char message[1024];
        sprintf(message, "elapsed: %ld ms\n", t);
        
        Debug::AppendMessage("profile.txt", message);
        
        BLTimer__.Reset();
        
        BLTimer__Count = 0;
    }
#endif
}
#endif

#if DEBUG_BYPASS // Debug
void
RebalanceProcessFftObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                         const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
#if PROFILE_RNN
    BLTimer__.Start();
#endif
    
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    WDL_TypedBuf<double> phasesMix;
    Utils::ComplexToMagnPhase(&magnsMix, &phasesMix, mixBuffer);
    
    //Debug::DumpData("magns0.txt", magnsMix);
    
    //double hzPerBin = 43.066; // TODO: do not hardcode this !
    //double hzPerBin = ((double)mSampleRate)/(BUFFER_SIZE/2);
    
    //WDL_TypedBuf<double> testMfcc;
    //Utils::FreqsToMelNorm(&testMfcc, magnsMix, hzPerBin);
    
    //Debug::DumpData("mfcc.txt", testMfcc);
    
    //WDL_TypedBuf<double> magns1;
    //Utils::MelToFreqsNorm(&magns1, testMfcc, hzPerBin);
    
    //Debug::DumpData("magns1.txt", magns1);
    
    //Debug::DumpData("magns0.txt", magnsMix);
    
    WDL_TypedBuf<double> magnsMixDown = magnsMix;
    Downsample(&magnsMixDown);
    
    //Debug::DumpData("magns1.txt", magnsMixDown);
    
    //WDL_TypedBuf<double> test = magnsMixDown;
    //Upsample(&test);
    
    //Debug::DumpData("magns2.txt", test);
    
    // mMixCols is filled with zeros at the origin
    mMixCols.push_back(magnsMixDown);
    mMixCols.pop_front();
    
    WDL_TypedBuf<double> mixBufHisto;
    ColumnsToBuffer(&mixBufHisto, mMixCols);
    
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
    
    WDL_TypedBuf<double> resultMagns;
    Utils::ResizeFillZeros(&resultMagns, magnsMix.GetSize());
    
    for (int i = 0; i < resultMagns.GetSize(); i++)
    {
        double vocal = resultVocal.Get()[i];
        double bass = resultBass.Get()[i];
        double drums = resultDrums.Get()[i];
        double other = resultOther.Get()[i];
        
        double result = vocal; // + bass + drums + other;
        
        resultMagns.Get()[i] = result;
    }
    
    
    // Fill the result
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    Utils::MagnPhaseToComplex(&fftSamples, resultMagns, phasesMix);
    
    fftSamples.Resize(fftSamples.GetSize()*2);
    //Utils::ResizeFillZeros(&fftSamples, ioBuffer->GetSize());
    
    Utils::FillSecondFftHalf(&fftSamples);
    
    // Result
    *ioBuffer = fftSamples;
    
#if PROFILE_RNN
    BLTimer__.Stop();
    
    if (BLTimer__Count++ > 50)
    {
        long t = BLTimer__.Get();
        
        char message[1024];
        sprintf(message, "elapsed: %ld ms\n", t);
        
        Debug::AppendMessage("profile.txt", message);
        
        BLTimer__.Reset();
        
        BLTimer__Count = 0;
    }
#endif
}
#endif

void
RebalanceProcessFftObj::ComputeLineMasks(WDL_TypedBuf<double> *maskMixResult,
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
    
    //Upsample(&maskBuf);
    
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
    
#if DEBUG_DUMP_PPM
    //static int count = 0;
    //if (count++ == 20)
    {
        PPMFile::SavePPM("mask-proc2.ppm",
                         maskBuf.Get(),
                         maskBuf.GetSize()/NUM_FFT_COLS_CUT, NUM_FFT_COLS_CUT,
                         1, // bpp
                         255.0 // coeff
                         );
        
        Debug::DumpData("mask-buf-res2.txt", maskBuf);
    }
#endif
    
    WDL_TypedBuf<double> maskMix;
    WDL_TypedBuf<double> maskSource;

    // For the moment, we try only with vocal
    ComputeLineMasks(&maskMix, &maskSource, maskBuf, confidence);

    ComputeRemixedMagns(result, mixBuffer, maskMix, maskSource, ratio, 0.0);
}

void
RebalanceProcessFftObj::Downsample(WDL_TypedBuf<double> *ioBuf)
{
    //mDownsampler.Resample(ioBuf);
    
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
RebalanceProcessFftObj::Upsample(WDL_TypedBuf<double> *ioBuf)
{
    if (RESAMPLE_FACTOR != 1)
    {
        //mUpsampler.Resample(ioBuf);
    
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
RebalanceProcessFftObj::UpsamplePredictedMask(WDL_TypedBuf<double> *ioBuf)
{
    if (RESAMPLE_FACTOR == 1)
        // No need to upsample
        return;
    
    WDL_TypedBuf<double> result;
    
    for (int j = 0; j < NUM_FFT_COLS_CUT; j++)
    {
        WDL_TypedBuf<double> mask;
        mask.Resize(BUFFER_SIZE/(2*RESAMPLE_FACTOR));
        
        for (int i = 0; i < mask.GetSize(); i++)
        {
            int id = i + j*mask.GetSize();
            
            if (id >= ioBuf->GetSize())
                // Just in case
                break;
            
            mask.Get()[i] = ioBuf->Get()[id];
        }
        
        Upsample(&mask);
        
        result.Add(mask.Get(), mask.GetSize());
    }
    
    *ioBuf = result;
}

////

const int kNumPrograms = 1;

enum EParams
{
  kVocal = 0,
  kVocalConfidence,
  kBass,
  kBassConfidence,
  kDrums,
  kDrumsConfidence,
  kOther,
  kOtherConfidence,
  
  kGraph,
  
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
  
  kGraphX = 0,
  kGraphY = 0,
  kGraphFrames = 1,
  
  kVocalX = 52,
  kVocalY = 340,
  kKnobVocalFrames = 180,
  
  kVocalConfidenceX = 72,
  kVocalConfidenceY = 480,
  kKnobVocalConfidenceFrames = 180,
    
  kBassX = 150,
  kBassY = 340,
  kKnobBassFrames = 180,
  
  kBassConfidenceX = 170,
  kBassConfidenceY = 480,
  kKnobBassConfidenceFrames = 180,
    
  kDrumsX = 254,
  kDrumsY = 340,
  kKnobDrumsFrames = 180,
  
  kDrumsConfidenceX = 274,
  kDrumsConfidenceY = 480,
  kKnobDrumsConfidenceFrames = 180,
    
  kOtherX = 356,
  kOtherY = 340,
  kKnobOtherFrames = 180,
    
  kOtherConfidenceX = 376,
  kOtherConfidenceY = 480,
  kKnobOtherConfidenceFrames = 180,
    
  // Buttons
  kFileOpenMixX = 510,
  kFileOpenMixY = 50,
  kFileOpenMixFrames = 3,
    
  kFileOpenSourceX = 510,
  kFileOpenSourceY = 90,
  kFileOpenSourceFrames = 3,
    
  kGenerateX = 510,
  kGenerateY = 130,
  kGenerateFrames = 3
};


Rebalance::Rebalance(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;
    
  GUIHelper9 guiHelper;
  
  // Init WDL FFT
  FftProcessObj14::Init();

  mFftObj = NULL;

#if SA_API
  mRebalanceDumpFftObj = NULL;
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
  
  // Vocal confidence
#if !SA_API
  double defaultVocalConfidence = DEFAULT_CONFIDENCE;
  mVocalConfidence = defaultVocalConfidence;
    
  GetParam(kVocalConfidence)->InitDouble("VocalConfidence", defaultVocalConfidence, 0.0, 100.0, 0.1, "%");
#endif
    
  guiHelper.CreateKnob3(this, pGraphics,
                        KNOB_VOCAL_CONFIDENCE_ID, KNOB_VOCAL_CONFIDENCE_FN, kKnobVocalConfidenceFrames,
                        kVocalConfidenceX, kVocalConfidenceY, kVocalConfidence, "CONFIDENCE",
                        KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                        BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                        TEXTFIELD_ID, TEXTFIELD_FN,
                        GUIHelper9::SIZE_SMALL);
    
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
    
    // Bass confidence
#if !SA_API
    double defaultBassConfidence = DEFAULT_CONFIDENCE;
    mBassConfidence = defaultBassConfidence;
    
    GetParam(kBassConfidence)->InitDouble("BassConfidence", defaultBassConfidence, 0.0, 100.0, 0.1, "%");
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_BASS_CONFIDENCE_ID, KNOB_BASS_CONFIDENCE_FN, kKnobBassConfidenceFrames,
                          kBassConfidenceX, kBassConfidenceY, kBassConfidence, "CONFIDENCE",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
    
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
    
    // Drums confidence
#if !SA_API
    double defaultDrumsConfidence = DEFAULT_CONFIDENCE;
    mDrumsConfidence = defaultDrumsConfidence;
    
    GetParam(kDrumsConfidence)->InitDouble("DrumsConfidence", defaultDrumsConfidence, 0.0, 100.0, 0.1, "%");
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_DRUMS_CONFIDENCE_ID, KNOB_DRUMS_CONFIDENCE_FN, kKnobDrumsConfidenceFrames,
                          kDrumsConfidenceX, kDrumsConfidenceY, kDrumsConfidence, "CONFIDENCE",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);
  
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
    
    // Other confidence
#if !SA_API
    double defaultOtherConfidence = DEFAULT_CONFIDENCE;
    mOtherConfidence = defaultOtherConfidence;
    
    GetParam(kOtherConfidence)->InitDouble("OtherConfidence", defaultOtherConfidence, 0.0, 100.0, 0.1, "%");
#endif
    
    guiHelper.CreateKnob3(this, pGraphics,
                          KNOB_OTHER_CONFIDENCE_ID, KNOB_OTHER_CONFIDENCE_FN, kKnobOtherConfidenceFrames,
                          kOtherConfidenceX, kOtherConfidenceY, kOtherConfidence, "CONFIDENCE",
                          KNOB_SHADOW_SMALL_ID, KNOB_SHADOW_SMALL_FN,
                          BLUE_KNOB_DIFFUSE_SMALL_ID, BLUE_KNOB_DIFFUSE_SMALL_FN,
                          TEXTFIELD_ID, TEXTFIELD_FN,
                          GUIHelper9::SIZE_SMALL);

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
    
  // Graph
  //
  mGraph = guiHelper.CreateGraph10(this, pGraphics, kGraph,
                                  GRAPH_ID, GRAPH_FN,
                                  kGraphFrames,
                                  kGraphX, kGraphY,
                                  1, //0, //GRAPH_CONTROL_NUM_CURVES
                                  512, //0, //GRAPH_CONTROL_NUM_POINTS,
                                  GRAPH_SHADOWS_ID, GRAPH_SHADOWS_FN);
  
  mGraph->SetClearColor(0, 0, 0, 255);
  
  int sepColor[4] = { 24, 24, 24, 255 };
  mGraph->SetSeparatorY0(2.0, sepColor);
    
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
  
  //Init(OVERSAMPLING, FREQ_RES);
  
  AttachGraphics(pGraphics);
  
  Init(OVERSAMPLING, FREQ_RES);
    
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  // Useful especially when using sidechain
  IPlug::NameBusses();
    
  BL_PROFILE_RESET;
}


Rebalance::~Rebalance()
{
  if (mFftObj != NULL)
    delete mFftObj;
  
#if SA_API
  if (mRebalanceDumpFftObj != NULL)
      delete mRebalanceDumpFftObj;
#else
    for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
    {
        if (mRebalanceProcessFftObjs[i] != NULL)
            delete mRebalanceProcessFftObjs[i];
    }
#endif
}

void
Rebalance::Init(int oversampling, int freqRes)
{
  double sampleRate = GetSampleRate();
  
  if (mFftObj == NULL)
  {
#if SA_API
    int numChannels = 1;
    int numScInputs = 1;
#else
    int numChannels = 2;
      
#if MONO_PROCESS_TEST
    numChannels = 1;
#endif
      
    int numScInputs = 0;
#endif
    
    vector<ProcessObj *> processObjs;
      
#if SA_API
    mRebalanceDumpFftObj = new RebalanceDumpFftObj(BUFFER_SIZE);
    processObjs.push_back(mRebalanceDumpFftObj);
#else
    
    IGraphics *graphics = GetGUI();
    WDL_String resPath;
    graphics->GetResourceDir(&resPath);
    
    for (int i = 0; i < numChannels; i++)
    {
        RebalanceProcessFftObj *obj = new RebalanceProcessFftObj(BUFFER_SIZE, resPath.Get());
        mRebalanceProcessFftObjs.push_back(obj);
          
        processObjs.push_back(obj);
    }
#endif
      
    mFftObj = new FftProcessObj14(processObjs,
                                  numChannels, numScInputs,
                                  BUFFER_SIZE, oversampling, freqRes,
                                  sampleRate);
#if !VARIABLE_HANNING
    mFftObj->SetAnalysisWindow(FftProcessObj14::ALL_CHANNELS,
                                 FftProcessObj14::WindowHanning);
    mFftObj->SetSynthesisWindow(FftProcessObj14::ALL_CHANNELS,
                                  FftProcessObj14::WindowHanning);
#else
    mFftObj->SetAnalysisWindow(FftProcessObj14::ALL_CHANNELS,
                               FftProcessObj14::WindowVariableHanning);
    mFftObj->SetSynthesisWindow(FftProcessObj14::ALL_CHANNELS,
                                FftProcessObj14::WindowVariableHanning);
#endif
      
    mFftObj->SetKeepSynthesisEnergy(FftProcessObj14::ALL_CHANNELS,
                                    KEEP_SYNTHESIS_ENERGY);
  }
  else
  {
    mFftObj->Reset(oversampling, freqRes, mSampleRate);
  }
    
#if SA_API
    
#define DBG_FILES 1
#if DBG_FILES
    //OpenMixFile("/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.0.0/DeepLearning/Audio/Mix.wav");
    //OpenSourceFile("/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.0.0/DeepLearning/Audio/Vocal.wav");
    //Generate();
    
    GenerateMSD100();
#endif
    
#endif
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
      
    
    mFftObj->Process(in, scIn, &out);
      
      
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
  
  mFftObj->Reset();
    
#if SA_API
  mRebalanceDumpFftObj->Reset();
#else
  for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
  {
        if (mRebalanceProcessFftObjs[i] != NULL)
            mRebalanceProcessFftObjs[i]->Reset();
  }
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
    
    case kVocalConfidence:
    {
        double vocalConfidence = GetParam(paramIdx)->Value();
          
          mVocalConfidence = vocalConfidence/100.0;
          
          for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
          {
              if (mRebalanceProcessFftObjs[i] != NULL)
                  mRebalanceProcessFftObjs[i]->SetVocalConfidence(mVocalConfidence);
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
          
    case kBassConfidence:
    {
        double bassConfidence = GetParam(paramIdx)->Value();
          
        mBassConfidence = bassConfidence/100.0;
          
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetBassConfidence(mBassConfidence);
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
          
    case kDrumsConfidence:
    {
        double drumsConfidence = GetParam(paramIdx)->Value();
          
        mDrumsConfidence = drumsConfidence/100.0;
          
          for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
          {
              if (mRebalanceProcessFftObjs[i] != NULL)
                  mRebalanceProcessFftObjs[i]->SetDrumsConfidence(mDrumsConfidence);
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
          
    case kOtherConfidence:
    {
        double otherConfidence = GetParam(paramIdx)->Value();
          
        mOtherConfidence = otherConfidence/100.0;
          
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetOtherConfidence(mOtherConfidence);
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
        mCurrentSourceChannel0.Add(sourceChannels[0].Get(), sourceChannels[0].GetSize());
    
    delete audioFile;
    
    return true;
}

void
Rebalance::Generate()
{
    // NOTE: Take onlmy the left channel
    //
    //if (mCurrentMixChannels.empty())
    //    return;
    
    //if (mCurrentSourceChannels.empty())
    //    return;
    
    //if (mCurrentMixChannels[0].GetSize() != mCurrentSourceChannels[0].GetSize())
    //    return;
    
    if (mCurrentMixChannel0.GetSize() != mCurrentSourceChannel0.GetSize())
        return;
    
    long pos = 0;
    while(pos < mCurrentMixChannel0/*s[0]*/.GetSize() - BUFFER_SIZE)
    {
        vector<WDL_TypedBuf<double> > in;
        in.resize(1);
        in[0].Add(&mCurrentMixChannel0/*s[0]*/.Get()[pos], BUFFER_SIZE);
        
        vector<WDL_TypedBuf<double> > sc;
        sc.resize(1);
        sc[0].Add(&mCurrentSourceChannel0/*s[0]*/.Get()[pos], BUFFER_SIZE);
        
        // Process
        mFftObj->Process(in, sc, NULL);
        
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

