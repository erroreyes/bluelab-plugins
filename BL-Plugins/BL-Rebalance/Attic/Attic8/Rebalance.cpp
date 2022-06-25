#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <vector>
#include <deque>
#include <fstream>
#include <string>
using namespace std;

#include <FftProcessObj16.h>

#include <GUIHelper9.h>
#include <SecureRestarter.h>

#include <Utils.h>
#include <Debug.h>

#include <PPMFile.h>

#include <BlaTimer.h>

#include <TrialMode6.h>

#include <SoftMaskingN.h>

// Do not manage data dumping on WIN32
// (more simple for compilation on WIN32)
#ifndef WIN32
#if SA_API
#include <AudioFile.h>
#endif
#endif

#include <DbgSpectrogram.h>

#include <RebalanceMaskPredictor.h>
#include <RebalanceProcessFftObj.h>

#include <RebalanceMaskPredictorComp.h>
#include <RebalanceProcessFftObjComp.h>

#include "Rebalance.h"

#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

// NEW 20200522
//#define EPS 1e-10
#define EPS 1e-15

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

// FIX: Cubase 10, Mac: at startup, the input buffers can be empty,
// just at startup
#define FIX_CUBASE_STARTUP_CRASH 1


#if 0
NOTE: for compiling with VST3 => created config of VST3 "base.xcodeproj" => separate directory

NOTE: switched to C++11 for pocket-tensor & OSX 10.7 + added __SSE4_1__ preprocessor macro
added __SSSE3__ too

NOTE: PocketTensor: PT_LOOP_UNROLLING_ENABLE to 1 => 15% perf gain

NOTE: RNN, 2x256 seems better than 4x128
- no phasing
- no perf slow-down

NOTE: oversampling 32 seems to give same results as oversampling 4
NOTE: with normalized values, the sound seems to bleed more

IDEA: try to use the Deezer algorithm and make an update

SUPPORT: when new version, write email to wheatwilliams1@gmail.com
(remove vocal => pumping effect)

NOTE: by default, it is sompiled with default instruction set (SSE4_2). Tested with AVX => maybe we gain ~5%

NOTE: the buggy implementation gave an interesting sound ! no pumping!
To do so, in SoftMaskingN: make a single mixture history, and it will be changed in the loop

----
TODO: at the end, check with several sample rates

BUG: there was a crash! bounce, set soft hard to 100, re-bounce => sometimes it crashes

TODO: check very well masks addition => summing all the masks should be 1 etc...
TODO: TEST1: take one of the original stem files, compute the mask as when generating the data,
apply DNN, compare the DNN mask => this should be very similar
TEST2: take one of the stem files, compute the mask as when generating the data,
and apply directly this mask to get the final sound => this should be high quality
(otherwise there is a bug in re-synthesis)

TODO: test with the 2 downloaded mp3
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
    
    kUseSoftMasks,
    
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
    kPrecisionX = 450,
    kPrecisionY = 90, //126,
    kKnobPrecisionFrames = 180,

    
#if !HIDE_SENSITIVITY
    // Precisions
    kVocalSensitivityX = 52,
    kVocalSensitivityY = 200,
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
    kGenerateFrames = 3,
    
    kUseSoftMasksX = 460, //440,
    kUseSoftMasksY = 200,
    kUseSoftMasksFrames = 2
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

////

class RebalanceTestMultiObj : public MultichannelProcess
{
public:
    RebalanceTestMultiObj(RebalanceDumpFftObj *rebalanceDumpFftObjs[4],
                          double sampleRate);
    
    virtual ~RebalanceTestMultiObj();
    
    void Reset();
    
    void Reset(int bufferSize, int overlapping, int oversampling, double sampleRate);
    
    //void ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
    //                     const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer);
    
    void ProcessResultFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
                          const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer);
    
protected:
    double mSampleRate;
    
    RebalanceDumpFftObj *mRebalanceDumpFftObjs[4];
};

RebalanceTestMultiObj::RebalanceTestMultiObj(RebalanceDumpFftObj *rebalanceDumpFftObjs[4],
                                             double sampleRate)
{
    for (int i = 0; i < 4; i++)
        mRebalanceDumpFftObjs[i] = rebalanceDumpFftObjs[i];
    
    mSampleRate = sampleRate;
}

RebalanceTestMultiObj::~RebalanceTestMultiObj() {}

void
RebalanceTestMultiObj::Reset()
{
    MultichannelProcess::Reset();
}

void
RebalanceTestMultiObj::Reset(int bufferSize, int overlapping, int oversampling, double sampleRate)
{
    MultichannelProcess::Reset(bufferSize, overlapping, oversampling, sampleRate);
    
    mSampleRate = sampleRate;
}

void
RebalanceTestMultiObj::ProcessResultFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
                                        const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer)
{
    // Similar to ComputeAndDump (a bit)
    vector<RebalanceDumpFftObj::Slice> slices[4];
    for (int k = 0; k < 4; k++)
    {
        // Get spectrogram slice for source
        mRebalanceDumpFftObjs[k]->GetSlices(&slices[k]);
        mRebalanceDumpFftObjs[k]->ResetSlices();
    }
    
    int sliceIdx = slices[0].size() - 1;
    
    vector<WDL_TypedBuf<double> > sourceData[4];
    vector<WDL_TypedBuf<double> > mixData;
    for (int k = 0; k < 4; k++)
    {
        const RebalanceDumpFftObj::Slice &slice = slices[k][sliceIdx];
        slice.GetData(&mixData, &sourceData[k]);
    }
    
    // TEST: downsample before
    //Rebalance::NormalizeSourceSlicesDB(sourceData);
    
    //Rebalance::Downsample(&mixData, mSampleRate);
    //for (int k = 0; k < 4; k++)
    //    Rebalance::Downsample(&sourceData[k], mSampleRate);
    
    // Compute binary mask (normalized vs binary)
    // ORIGIN (very raw)
    //Rebalance::BinarizeSourceSlices(sourceData);
    
    Rebalance::NormalizeSourceSlices(sourceData);
    //Rebalance::NormalizeSourceSlicesDB(sourceData); // PREV
    
    // Now, recale up the binarized data and set it as output
    //
    
    //WIP: test Rebalance::NormalizeSourceSlicesDB with DB inv
    //(maybe there is a bug in bormalization...) this is strange...
    //TODO: test better the masks computing, maybe normalize before downsample...
    //=> must have very clean masks !
    // TODO: mult by mix and see if we get the correct result
    int vocalIdx = 0;
    int lastIdx = sourceData[vocalIdx].size() - 1;
    //RebalanceMaskPredictor::Upsample(&sourceData[vocalIdx][lastIdx], mSampleRate);
    
    if (ioFftSamples->empty())
        return;
    
#if 0 // Output mask
    WDL_TypedBuf<WDL_FFT_COMPLEX> resultData;
    resultData.Resize((*ioFftSamples)[0]->GetSize()/2);
    Utils::FillAllZero(&resultData);
    
    for (int i = 0; i < resultData.GetSize(); i++)
    {
        double val = sourceData[vocalIdx][lastIdx].Get()[i];
        
        // "set magn" and ignore phase
        resultData.Get()[i].re = val;
        // .im is 0
    }
#endif
    
#if 1 // Apply mask
    WDL_TypedBuf<WDL_FFT_COMPLEX> resultData = *(*ioFftSamples)[0];
    resultData.Resize(resultData.GetSize()/2);
    Utils::MultValues(&resultData, sourceData[vocalIdx][lastIdx]);
#endif
    
    // Fill the result
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples = resultData;
    fftSamples.Resize(fftSamples.GetSize()*2);
    Utils::FillSecondFftHalf(&fftSamples);
    
    // Result
    *(*ioFftSamples)[0] = fftSamples;
}

////

Rebalance::Rebalance(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;
    
  GUIHelper9 guiHelper;
  
  // Init WDL FFT
  FftProcessObj16::Init();

  for (int i = 0; i < 4; i++)
    mFftObjs[i] = NULL;

#if SA_API
  for (int i = 0; i < 4; i++)
    mRebalanceDumpFftObjs[i] = NULL;
#else
  mMaskPred = NULL;
#endif
    
  mRebalanceTestMultiObj = NULL;
    
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
    
#if USE_SOFT_MASK_N
   int defaultUseSoftMasks = 1;
   GetParam(kUseSoftMasks)->InitInt("UseBlendOptim", defaultUseSoftMasks, 0, 1);
    
   guiHelper.CreateToggleButton(this, pGraphics,
                                CHECKBOX_USE_SOFT_MASKS_ID, CHECKBOX_USE_SOFT_MASKS_FN,
                                kUseSoftMasksFrames,
                                kUseSoftMasksX, kUseSoftMasksY, kUseSoftMasks,
                                "BLEND OPTIM.",
                                CHECKBOX_DIFFUSE_ID, CHECKBOX_DIFFUSE_FN,
                                GUIHelper9::SIZE_SMALL);
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
    
    if (mRebalanceTestMultiObj != NULL)
        delete mRebalanceTestMultiObj;
    
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
#if TEST_INPUT
    InitTest(oversampling, freqRes);
    
    return;
#endif
    
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

            mFftObjs[i] = new FftProcessObj16(processObjs[i],
                                              numChannels, numScInputs,
                                              BUFFER_SIZE, oversampling, freqRes,
                                              sampleRate);
            
#if !VARIABLE_HANNING
            mFftObjs[i]->SetAnalysisWindow(FftProcessObj16::ALL_CHANNELS,
                                           FftProcessObj16::WindowHanning);
            mFftObjs[i]->SetSynthesisWindow(FftProcessObj16::ALL_CHANNELS,
                                            FftProcessObj16::WindowHanning);
#else
            mFftObjs[i]->SetAnalysisWindow(FftProcessObj16::ALL_CHANNELS,
                                           FftProcessObj16::WindowVariableHanning);
            mFftObjs[i]->SetSynthesisWindow(FftProcessObj16::ALL_CHANNELS,
                                            FftProcessObj16::WindowVariableHanning);
#endif
        
            mFftObjs[i]->SetKeepSynthesisEnergy(FftProcessObj16::ALL_CHANNELS,
                                                KEEP_SYNTHESIS_ENERGY);
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            mFftObjs[i]->Reset(BUFFER_SIZE, oversampling, freqRes, mSampleRate);
        }
    }
    
#if DUMP_DATASET_FILES
    GenerateMSD100();
#endif
}

void
Rebalance::InitTest(int oversampling, int freqRes)
{
    double sampleRate = GetSampleRate();
    
    if (mFftObjs[0] == NULL)
    {
        int numChannels = 4; // 1;
        // => must  not have less inputs than sidechain for FftProcessObj16
        
        int numScInputs = 4; // 1
        
        // Source objs
        vector<ProcessObj *> processObjs;
        for (int i = 0; i < 4; i++)
        {
            mRebalanceDumpFftObjs[i] = new RebalanceDumpFftObj(BUFFER_SIZE);
            
            processObjs.push_back(mRebalanceDumpFftObjs[i]);
        }
        
        mFftObjs[0] = new FftProcessObj16(processObjs,
                                          numChannels, numScInputs,
                                          BUFFER_SIZE, oversampling, freqRes,
                                          sampleRate);
            
#if !VARIABLE_HANNING
        mFftObjs[0]->SetAnalysisWindow(FftProcessObj16::ALL_CHANNELS,
                                       FftProcessObj16::WindowHanning);
        mFftObjs[0]->SetSynthesisWindow(FftProcessObj16::ALL_CHANNELS,
                                        FftProcessObj16::WindowHanning);
#else
        mFftObjs[0]->SetAnalysisWindow(FftProcessObj16::ALL_CHANNELS,
                                       FftProcessObj16::WindowVariableHanning);
        mFftObjs[0]->SetSynthesisWindow(FftProcessObj16::ALL_CHANNELS,
                                        FftProcessObj16::WindowVariableHanning);
#endif
            
        mFftObjs[0]->SetKeepSynthesisEnergy(FftProcessObj16::ALL_CHANNELS,
                                            KEEP_SYNTHESIS_ENERGY);
        
        // Multichannel
        mRebalanceTestMultiObj = new RebalanceTestMultiObj(mRebalanceDumpFftObjs,
                                                           mSampleRate);
        mFftObjs[0]->AddMultichannelProcess(mRebalanceTestMultiObj);
    }
    else
    {
        mFftObjs[0]->Reset(BUFFER_SIZE, oversampling, freqRes, mSampleRate);
    }
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
      
#if !USE_COMPLEX_PROCESSING
    mMaskPred = new RebalanceMaskPredictor(BUFFER_SIZE, oversampling,
                                           freqRes, sampleRate,
                                           graphics);
#else
    mMaskPred = new RebalanceMaskPredictorComp(BUFFER_SIZE, oversampling,
                                               freqRes, sampleRate,
                                               graphics);
#endif
      
#if FORCE_SAMPLE_RATE
    double plugSampleRate = GetSampleRate();
    mMaskPred->SetPlugSampleRate(plugSampleRate);
#endif
      
    for (int i = 0; i < numChannels; i++)
    {
#if !USE_COMPLEX_PROCESSING
        RebalanceProcessFftObj *obj = new RebalanceProcessFftObj(BUFFER_SIZE, mMaskPred);
#else
        RebalanceProcessFftObjComp *obj = new RebalanceProcessFftObjComp(BUFFER_SIZE, mMaskPred);
#endif
        
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
      
    mFftObjs[0] = new FftProcessObj16(processObjs,
                                      numChannels, numScInputs,
                                      BUFFER_SIZE, oversampling, freqRes,
                                      sampleRate);
#if !VARIABLE_HANNING
    mFftObjs[0]->SetAnalysisWindow(FftProcessObj16::ALL_CHANNELS,
                                   FftProcessObj16::WindowHanning);
    mFftObjs[0]->SetSynthesisWindow(FftProcessObj16::ALL_CHANNELS,
                                    FftProcessObj16::WindowHanning);
#else
    mFftObjs[0]->SetAnalysisWindow(FftProcessObj16::ALL_CHANNELS,
                                   FftProcessObj16::WindowVariableHanning);
    mFftObj[0]->SetSynthesisWindow(FftProcessObj16::ALL_CHANNELS,
                                   FftProcessObj16::WindowVariableHanning);
#endif
      
    mFftObjs[0]->SetKeepSynthesisEnergy(FftProcessObj16::ALL_CHANNELS,
                                        KEEP_SYNTHESIS_ENERGY);
      
    // For plugin processing
    mFftObjs[0]->AddMultichannelProcess(mMaskPred);
  }
  else
  {
    mFftObjs[0]->Reset(BUFFER_SIZE, oversampling, freqRes, mSampleRate);
      
#if FORCE_SAMPLE_RATE
      double plugSampleRate = GetSampleRate();
      mMaskPred->SetPlugSampleRate(plugSampleRate);
      
      for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
      {
          mRebalanceProcessFftObjs[i]->SetPlugSampleRate(plugSampleRate);
      }
#endif
  }
    
  UpdateLatency();
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
  
    // Cubase 10, Sierra
    // in or out can be empty...
#if FIX_CUBASE_STARTUP_CRASH
    if (in.empty() || out.empty())
        return;
#endif
    
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
     
#if TEST_INPUT
    // Need 4 inputs because we have 4 side chains
    if (in.size() != 4)
    {
        in.resize(4);
        
        in[2] = in[0];
        in[3] = in[1];
    }
#endif
      
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
          mFftObjs[i]->Reset(BUFFER_SIZE, OVERSAMPLING, FREQ_RES, sampleRate); //
  }
    
#if SA_API
  for (int i = 0; i < 4; i++)
  {
      if (mRebalanceDumpFftObjs[i] != NULL)
          mRebalanceDumpFftObjs[i]->Reset(BUFFER_SIZE, OVERSAMPLING, FREQ_RES, sampleRate); //
  }
#else
  for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
  {
        if (mRebalanceProcessFftObjs[i] != NULL)
            mRebalanceProcessFftObjs[i]->Reset(BUFFER_SIZE, OVERSAMPLING, FREQ_RES, sampleRate); //
  }
#endif
    
#if FORCE_SAMPLE_RATE
    double plugSampleRate = GetSampleRate();
    
#if !TEST_INPUT
    mMaskPred->SetPlugSampleRate(plugSampleRate);
    
    for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
    {
        mRebalanceProcessFftObjs[i]->SetPlugSampleRate(plugSampleRate);
    }
#else
    mRebalanceTestMultiObj->Reset(BUFFER_SIZE, OVERSAMPLING, FREQ_RES, sampleRate);
#endif
    
#endif

    // NEW: (moved)
    UpdateLatency();
    
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
       
#if USE_SOFT_MASK_N
    case kUseSoftMasks:
    {
        int value = GetParam(kUseSoftMasks)->Int();

        bool useSoftMasks = (value == 1);
        
        for (int i = 0; i < mRebalanceProcessFftObjs.size(); i++)
        {
            if (mRebalanceProcessFftObjs[i] != NULL)
                mRebalanceProcessFftObjs[i]->SetUseSoftMasks(useSoftMasks);
        }
        
       GUIHelper9::UpdateText(this, paramIdx);
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
#if SA_API
  
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

#endif //SA_API
#endif // WIN32

    return true;
}

bool
Rebalance::OpenSourceFile(const char *fileName, WDL_TypedBuf<double> *result)
{
#ifndef WIN32
#if SA_API
  
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
    
#endif // SA_API
#endif //WIN32

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
        /*long numProcessed =*/ Generate();
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
        
        Downsample(&mixData, mSampleRate);
        
        for (int k = 0; k < 4; k++)
            Downsample(&sourceData[k], mSampleRate);
        
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
            else // NEW
            {
                for (int k = 0; k < 4; k++)
                {
                    sourceData[k][i].Get()[j] = 0.0;
                }
            }
        }
    }
}

void
Rebalance::NormalizeSourceSlicesDB(vector<WDL_TypedBuf<double> > sourceData[4])
{
#define DB_EPS 1e-15
#define MIN_DB -200.0
    
    for (int i = 0; i < sourceData[0].size(); i++)
    {
        for (int j = 0; j < sourceData[0][i].GetSize(); j++)
        {
            double sum = 0.0;
            for (int k = 0; k < 4; k++)
            {
                double val = sourceData[k][i].Get()[j];
                
                //val = Utils::AmpToDBNorm(val, DB_EPS, MIN_DB);
                val = Utils::DBToAmpNorm(val, DB_EPS, MIN_DB);
                
                sum += val;
            }
            
            if (fabs(sum) > EPS)
            {
                for (int k = 0; k < 4; k++)
                {
                    double val = sourceData[k][i].Get()[j];
                    
                    //val = Utils::AmpToDBNorm(val, DB_EPS, MIN_DB);
                    val = Utils::DBToAmpNorm(val, DB_EPS, MIN_DB);
                    
                    val /= sum;
                    
                    sourceData[k][i].Get()[j] = val;
                }
            }
            else
            {
                for (int k = 0; k < 4; k++)
                {
                    sourceData[k][i].Get()[j] = 0.0;
                }
            }
        }
    }
}

// NOT TESTED ? (not really sure at all it could work...)
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
Rebalance::Downsample(vector<WDL_TypedBuf<double> > *lines,
                      double sampleRate)
{
    for (int i = 0; i < lines->size(); i++)
    {
        WDL_TypedBuf<double> &data = (*lines)[i];
        RebalanceMaskPredictor::Downsample(&data, sampleRate);
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

// GOOD: fixes latency correctly !
void
Rebalance::UpdateLatency()
{
    if (mFftObjs[0] == NULL)
        return;
    
    // Compute the theorical default latency
    double coeff = mSampleRate/SAMPLE_RATE;
    int latency = ceil(coeff*BUFFER_SIZE);
    
    // FIX: Rebalance test 04, block size 447 or 999
    // => there was a missing buffer making a click
    // (this is a hack...)
    //
    // NOTES: this passes autotests
#if 0
    latency *= 2.0;
#endif

    // Maybe latency x 2 is too much
    // This will lead to a big plugins latency
    //
    // With one overlap in advance, this should be ok too
#if 1
    latency = latency + latency/OVERSAMPLING;
#endif
    
    for (int i = 0; i < 4; i++)
    {
        if (mFftObjs[i] != NULL)
            mFftObjs[i]->SetDefaultLatency(latency);
    }
    
    // Get the computed latency from the object and set it to the plugin
    if (mFftObjs[0] != NULL)
    {
        int blockSize = GetBlockSize();
        int plugLatency = mFftObjs[0]->ComputeLatency(blockSize);
        SetLatency(plugLatency);
    }
}
