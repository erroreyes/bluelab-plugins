#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <vector>
#include <deque>
using namespace std;

#include <FftProcessObj14.h>

#include <GUIHelper9.h>
#include <SecureRestarter.h>

#include <Utils.h>
#include <Debug.h>

#include <PPMFile.h>

#include <TrialMode6.h>

#include <AudioFile.h>

#include <GraphControl10.h>

// DNN
#include <pt_model.h>
#include <pt_tensor.h>

#include "Rebalance.h"

#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

// Same config as in article Simpson & Roma
#define BUFFER_SIZE 2048
#define OVERSAMPLING_NORMAL 4

#define FREQ_RES 1

#define KEEP_SYNTHESIS_ENERGY 0

// When set to 0, we are accurate for frequencies
#define VARIABLE_HANNING 0

// Generation parameters
#define MIX_SAVE_FILE "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.0.0/DeepLearning/mix.dat"
#define MASK_SAVE_FILE "/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.0.0/DeepLearning/mask.dat"

#define NUM_FFT_COLS_CUT 2 //8 //20

// TODO: manage resources on windows
#define MODEL_FILENAME "dnn-model-source0.model"

#define DEBUG_DUMP_PPM 0 //1 //0

#define MONO_PROCESS_TEST 0


#if 0
TODO: adjust the interface (enlarge it, diminish graph height,
                            adjust knobs horiz position) => for visibility
BUG: VST3 does not compile anymore (with C+++11 / OSX7 ?) => "undefined Steinberg::String::assign()" at link
NOTE: switched to C++11 for pocket-tensor & OSX 10.7 + added __SSE4_1__ preprocessor macro
added __SSSE3__ too
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
    
    int mNumCols;
    
    vector<WDL_TypedBuf<double> > mMixCols;
    vector<WDL_TypedBuf<double> > mMaskCols;
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
        WDL_TypedBuf<double> mixBuf;
        WDL_TypedBuf<double> maskBuf;
        
        ColumnsToBuffer(&mixBuf, mMixCols);
        ColumnsToBuffer(&maskBuf, mMaskCols);
        
        Utils::AppendValuesFile(MIX_SAVE_FILE, mixBuf, ',');
        Utils::AppendValuesFile(MASK_SAVE_FILE, maskBuf, ',');
        
#if DEBUG_DUMP_PPM
        static int count = 0;
        if (count++ == 20)
        {
            PPMFile::SavePPM("mix.ppm",
                             mixBuf.Get(),
                             ioBuffer->GetSize()/2, NUM_FFT_COLS_CUT,
                             1, // bpp
                             255.0*128.0 // coeff
                             );
        
            PPMFile::SavePPM("mask.ppm",
                             maskBuf.Get(),
                             ioBuffer->GetSize()/2, NUM_FFT_COLS_CUT,
                             1, // bpp
                             255.0 // coeff
                             );
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
    
    mMixCols.push_back(magnsMix);
    
    // Source
    WDL_TypedBuf<WDL_FFT_COMPLEX> sourceBuffer = *scBuffer;
    Utils::TakeHalf(&sourceBuffer);
    
    WDL_TypedBuf<double> magnsSource;
    Utils::ComplexToMagn(&magnsSource, sourceBuffer);
    
    // Mask
    WDL_TypedBuf<double> mask;
    ComputeMask(&mask, magnsMix, magnsSource);
    
#if 0
    // DEBUG
    for (int i = 0; i < 255; i++)
        mask.Get()[i] = 1.0;
#endif
    
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
    
    void ComputeLineMasks(WDL_TypedBuf<double> *maskMix,
                          WDL_TypedBuf<double> *maskSource,
                          const WDL_TypedBuf<double> &mask,
                          double alpha);
    
    void ComputeRemixedMagns(WDL_TypedBuf<double> *resultMagns,
                             const WDL_TypedBuf<double> &magnsMix,
                             const WDL_TypedBuf<double> &maskMix,
                             const WDL_TypedBuf<double> &maskSource,
                             double mixFactor, double otherFactor);

    
    // Parameters
    double mVocal;
    double mVocalConfidence;
    
    double mBass;
    double mBassConfidence;
    
    double mDrums;
    double mDrumsConfidence;
    
    double mOther;
    double mOtherConficence;
    
    // DNN
    std::unique_ptr<pt::Model> mKerasModel;
    
    deque<WDL_TypedBuf<double> > mMixCols;
};

RebalanceProcessFftObj::RebalanceProcessFftObj(int bufferSize,
                                               const char *resourcePath)
: ProcessObj(bufferSize)
{
    // Parameters
    mVocal = 0.0;
    mVocalConfidence = 0.5;
    
    mBass = 0.0;
    mBassConfidence = 0.5;
    
    mDrums = 0.0;
    mDrumsConfidence = 0.5;
    
    mOther = 0.0;
    mOtherConficence = 0.5;
    
    // DNN
    char modelFileName[2048];
    sprintf(modelFileName, "%s/%s", resourcePath, MODEL_FILENAME);
    mKerasModel = pt::Model::create(modelFileName);
    
    for (int i = 0; i < NUM_FFT_COLS_CUT; i++)
    {
        WDL_TypedBuf<double> col;
        Utils::ResizeFillZeros(&col, bufferSize/2);
        
        mMixCols.push_back(col);
    }
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
    mOtherConficence = otherConfidence;
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
            if (bufIndex >= buf->GetSize())
            {
                int dummy = 0;
            }
            
            buf->Get()[bufIndex] = col.Get()[i];
        }
    }
}

void
RebalanceProcessFftObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                         const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
    // Mix
    WDL_TypedBuf<WDL_FFT_COMPLEX> mixBuffer = *ioBuffer;
    Utils::TakeHalf(&mixBuffer);
    
    WDL_TypedBuf<double> magnsMix;
    WDL_TypedBuf<double> phasesMix;
    Utils::ComplexToMagnPhase(&magnsMix, &phasesMix, mixBuffer);
    
#if 0 // TEST
    //static int count = 0;
    //count++;
    
    //char magns0FileName[512];
    //sprintf(magns0FileName, "magns0-%d.txt", count);
    //Debug::DumpData(magns0FileName, magnsMix);
    
    //Debug::DumpData("magns0.txt", magnsMix);
    
    // mMixCols is filled with zeros at the origin
    mMixCols.push_back(magnsMix);
    mMixCols.pop_front();
    
    WDL_TypedBuf<double> mixBuf;
    ColumnsToBuffer(&mixBuf, mMixCols);
    
    pt::Tensor in(mixBuf.GetSize());
    for (int i = 0; i < mixBuf.GetSize(); i++)
    {
        in(i) = mixBuf.Get()[i];
    }
    
    pt::Tensor out;
    mKerasModel->predict(in, out);

    if (out.getSize() != mixBuf.GetSize())
    {
        int dummy = 0;
    }
        
    WDL_TypedBuf<double> maskBuf;
    maskBuf.Resize(mixBuf.GetSize());
    for (int i = 0; i < maskBuf.GetSize(); i++)
    {
        maskBuf.Get()[i] = out(i);
    }
    
#if 0
    // DEBUG
    WDL_TypedBuf<double> debugBuf;
    debugBuf.Resize(maskBuf.GetSize());
    for (int i = 0; i < ioBuffer->GetSize()/2; i++)
    {
        for (int j = 0; j < NUM_FFT_COLS_CUT; j++)
        {
            double val = maskBuf.Get()[i + j*ioBuffer->GetSize()/2];
            debugBuf.Get()[j + i*NUM_FFT_COLS_CUT] = val;
        }
    }
    maskBuf = debugBuf;
#endif
    
#if DEBUG_DUMP_PPM
    static int count = 0;
    if (count++ == 20)
    {
        PPMFile::SavePPM("mix-proc.ppm",
                         mixBuf.Get(),
                        ioBuffer->GetSize()/2, NUM_FFT_COLS_CUT,
                         1, // bpp
                         255.0*128.0 // coeff
                         );
        
        PPMFile::SavePPM("mask-proc.ppm",
                         maskBuf.Get(),
                        ioBuffer->GetSize()/2, NUM_FFT_COLS_CUT,
                         1, // bpp
                         255.0 // coeff
                         );
    }
#endif
    
    WDL_TypedBuf<double> maskMix;
    WDL_TypedBuf<double> maskSource;
    
    // For the moment, we try only with vocal
    ComputeLineMasks(&maskMix, &maskSource, maskBuf, mVocalConfidence);
    
    WDL_TypedBuf<double> resultMagns;
    ComputeRemixedMagns(&resultMagns, magnsMix, maskMix, maskSource, mVocal, mOther);
    
    //char magns1FileName[512];
    //sprintf(magns1FileName, "magns1-%d.txt", count);
    //Debug::DumpData(magns1FileName, resultMagns);
    
    //Debug::DumpData("magns1.txt", resultMagns);
    
#endif
    
    // Fill the result
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    Utils::MagnPhaseToComplex(&fftSamples, magnsMix/*resultMagns*/, phasesMix); // TEST
    
    fftSamples.Resize(/*fftSamples.GetSize()*2*/ioBuffer->GetSize());
    //Utils::ResizeFillZeros(&fftSamples, ioBuffer->GetSize());
    
    Utils::FillSecondFftHalf(&fftSamples);
    
    if (fftSamples.GetSize() != ioBuffer->GetSize())
    {
        int dummy = 0;
    }
    
    // Result
    *ioBuffer = fftSamples;
}

void
RebalanceProcessFftObj::ComputeLineMasks(WDL_TypedBuf<double> *maskMix,
                                         WDL_TypedBuf<double> *maskSource,
                                         const WDL_TypedBuf<double> &mask,
                                         double alpha)
{
    int numFreqs = BUFFER_SIZE/2;
    int numCols = NUM_FFT_COLS_CUT;

    maskMix->Resize(numFreqs);
    maskSource->Resize(numFreqs);
    
    for (int i = 0; i < numFreqs; i++)
    {
        double avg = 0.0;

        for (int j = 0; j < numCols; j++)
        {
            int idx = i + j*numFreqs;
            
            if (idx >= mask.GetSize())
            {
                int dummy = 0;
            }
            
            double m = mask.Get()[idx];
            avg += m;
        }
        
        avg /= numCols;
        
        double mix = (avg > alpha) ? 1.0 : 0.0;
        maskMix->Get()[i] = mix;
        
        double source = (avg < (1.0 - alpha) ) ? 1.0 : 0.0;
        maskSource->Get()[i] = source;
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
    
    if (maskMix.GetSize() != resultMagns->GetSize())
    {
        int dummy = 0;
    }
    
    if (maskSource.GetSize() != resultMagns->GetSize())
    {
        int dummy = 0;
    }
    
    if (magnsMix.GetSize() != resultMagns->GetSize())
    {
        int dummy = 0;
    }
    
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
        
        if (mix > 0.0)
        {
            magn *= otherFactor;
        }
        
        resultMagns->Get()[i] = magn;
    }
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
  
#if PROFILE_ADD_SLICE
  mCount = 0;
  mTimer.Reset();
#endif
    
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
  double defaultVocalConfidence = 50.0;
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
    double defaultBassConfidence = 50.0;
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
    double defaultDrumsConfidence = 50.0;
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
    double defaultOtherConfidence = 50.0;
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
  
  //Init(OVERSAMPLING_NORMAL, FREQ_RES);
  
  AttachGraphics(pGraphics);
  
  Init(OVERSAMPLING_NORMAL, FREQ_RES);
    
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
    OpenMixFile("/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.0.0/DeepLearning/Audio/Mix.wav");
    OpenSourceFile("/Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/IPlugExamples/Rebalance-v5.0.0/DeepLearning/Audio/Vocal.wav");
    Generate();
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
      
    static int count = 0;
    count++;
      
    char inFileName[512];
    sprintf(inFileName, "in-%d.txt", count);
    Debug::DumpData(inFileName, in[0]);
      
    mFftObj->Process(in, scIn, &out);
    
    char outFileName[512];
    sprintf(outFileName, "out-%d.txt", count);
    Debug::DumpData(outFileName, out[0]);
      
#if MONO_PROCESS_TEST
    out[1] = out[0];
#endif
      
#if PROFILE_ADD_SLICE
      mTimer.Stop();
      
      if (mCount++ > 50)
      {
          long t = mTimer.Get();
          
          char message[1024];
          sprintf(message, "elapsed: %ld ms\n", t);
          
          Debug::AppendMessage("profile.txt", message);
          
          mTimer.Reset();
          
          mCount = 0;
      }
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
    mCurrentMixChannels.clear();
        
    // Open the audio file
    double sampleRate = GetSampleRate();
    AudioFile *audioFile = AudioFile::Load(fileName, &mCurrentMixChannels);
    if (audioFile == NULL)
    {
        return false;
    }
    
    audioFile->Resample(sampleRate);
        
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
    mCurrentSourceChannels.clear();
    
    // Open the audio file
    double sampleRate = GetSampleRate();
    AudioFile *audioFile = AudioFile::Load(fileName, &mCurrentSourceChannels);
    if (audioFile == NULL)
    {
        return false;
    }
    
    audioFile->Resample(sampleRate);
    
    delete audioFile;
    
    return true;
}

void
Rebalance::Generate()
{
    // NOTE: Take onlmy the left channel
    //
    if (mCurrentMixChannels.empty())
        return;
    
    if (mCurrentSourceChannels.empty())
        return;
    
    if (mCurrentMixChannels[0].GetSize() != mCurrentSourceChannels[0].GetSize())
        return;
    
    long pos = 0;
    while(pos < mCurrentMixChannels[0].GetSize() - BUFFER_SIZE)
    {
        vector<WDL_TypedBuf<double> > in;
        in.resize(1);
        in[0].Add(&mCurrentMixChannels[0].Get()[pos], BUFFER_SIZE);
        
        vector<WDL_TypedBuf<double> > sc;
        sc.resize(1);
        sc[0].Add(&mCurrentSourceChannels[0].Get()[pos], BUFFER_SIZE);
        
        // Process
        mFftObj->Process(in, sc, NULL);
        
        pos += BUFFER_SIZE;
    }
}
