#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <FftProcessObj16.h>

#include <GUIHelper12.h>
#include <SecureRestarter.h>

#include <DenoiserObj.h>

#include <BLUtils.h>
#include <BLUtilsPlug.h>

#include <BLDebug.h>
#include <BlaTimer.h>

#include <GraphControl12.h>
#include <GraphFreqAxis2.h>
#include <GraphAmpAxis.h>
#include <GraphAxis2.h>

#include <GraphCurve5.h>
#include <SmoothCurveDB.h>

#include <Scale.h>

#include <PostTransientFftObj3.h>
#include <TransientShaperFftObj3.h>
#include <DenoiserPostTransientObj.h>
#include <DenoiserObj.h>

#include <ParamSmoother2.h>

#include "IControl.h"
#include "config.h"
#include "bl_config.h"

#include "Denoiser.h"

#include "IPlug_include_in_plug_src.h"

#define BUFFER_SIZE 2048

#define OVERLAPPING_0 4
#define OVERLAPPING_1 8
#define OVERLAPPING_2 16
#define OVERLAPPING_3 32

#define FREQ_RES 1
#define KEEP_SYNTHESIS_ENERGY 0
#define VARIABLE_HANNING 0 // When set to 0, we are accurate for frequencies

#define DEFAULT_VALUE_RATIO 1.0

// FIX: Flat curve at the end of the graph with high sample rates
// Set a very low value, so if we increase the gain,
// we won't have flat values at the end of the noise profile curve
//
// WARNING: modify slightly the result sound
//
#define MIN_DB2 -200.0

#define MIN_SIGNAL -1.0
#define MAX_SIGNAL 1.0

#define SIGNAL_SMOOTH_COEFF 0.5
#define NOISE_SMOOTH_COEFF 0.9

#define GRAPH_CURVE_NUM_VALUES 256 //512

// See Air for details
//#define CURVE_SMOOTH_COEFF 0.95
#define CURVE_SMOOTH_COEFF_MS 1.4

#define CURVE_SMOOTH_COEFF_NOISE_PROFILE_MS 0.5

//#define CURVE_FILL_ALPHA 0.2
//#define SIGNAL_SMOOTH_COEFF 0.5
//#define NOISE_SMOOTH_COEFF 0.9

// Use log before decimate => more accurate for low frequencies
//#define HIGH_RES_LOG_CURVES 1

// NOTE: We use curve fixed number of points, and we decimate more
// with higher buffer sizes

// Half "s", half "p"
#define TRANSIENT_FREQ_AMP_RATIO 0.5 // Original

// NOTE: the "p" are more increased, but the sound looks a bit
// less good (we don't hear "s" very much)
//
// Take a maximum of "p"
// because we process a noisy signal, and if we take "s",
// everything will be increased
//#define TRANSIENT_FREQ_AMP_RATIO 0.0

// VERY GOOD! : with this we have exactly the same sound
// as if we denoise only and put a Shaper plugin just after
//
// Very good sound: transients "s" and "p" are increased, and the
// global volume stays almost untouched
//
// Since DENOISER_OPTIM11
#define TRANSIENT_BOOST_FACTOR 1.0

#define USE_DROP_DOWN_MENU 1

#if 0
mail: jb.diogon@gmail.com - not working on Big Sur
niko => update end of April

BUG: when toggeling "soft denoise", the version number label gets a bit covered by a black area
(just 2 pixels on Mac, but half of the last number on windows)
=> seems hard to fix, display control bounds doesn't help

Concurrent: Bertom Denoiser.

IDEA: maybe use PeakDetector2D() for musical noise removal
(better sound quality ?, better performances ?)

IDEA: when ratio is 0%, the transient knob has no effect ? (maybe...)

BUG (not repro): sometimes the left channel seem to no be denoised
(since DENOISER_OPTIM0 ?)
Have to launch several times BlueLabAudio-Denoiser-TestReaper2-UnitTest-0/1
to see the problem sometimes
(was compiled in DEBUG when the problem appened)
This problem happens more often in debug !

BUG(not repro !): when res noise is at the maximum, with stereo, it mutes only one channel (we still have one of the channels with full volume).

- axis: "-100dB" instead of "100dB" ?

Site comment: Worked pretty good with VMix 22 x64, but crashed only a few seconds in to each session.
steve@thedoghouse.asia

//
- TODO: check strange comment with default preset

- NOTE: On PT, when bouncing, latency with bypassed plug, and without plug is different
(we considered without plug to setup the latency)

- NOTE: increasing quality can make appear residual noise
(but it was tested that with constant hanning factor, the result didn't change)

- NOTE: residual denoise makes a small latency
   EXAMPLE: cri de mouette au Jardin de la Victoire du Languedoc
- NOTE: extracted noise on "Morning" sounds strange (hacked),
This is normal, this is due to the recorder !
#endif

static char *tooltipHelp = "Help - Display Help";
static char *tooltipLearn = "Learn Mode - Learn the noise profile";
static char *tooltipNoiseOnly =
    "Noise Only - Output the suppressed noise instead of the signal";
static char *tooltipThreshold = "Threshold - Noise suppression threshold";
static char *tooltipRatio = "Ratio - Noise suppression ratio";
static char *tooltipQuality = "Quality - Processing quality";
static char *tooltipResNoiseThrs = "Residual Noise - Residual denoise threshold";
static char *tooltipTransBoost = "Transient Boost - Boost output transients";
static char *tooltipAutoResNoise =
    "Soft Denoise - Automatically remove residual noise";


enum EParams
{
    kLearn = 0,
    kNoiseOnly,
    kThreshold,
    kRatio,

#if USE_RESIDUAL_DENOISE
    kResNoiseThrs,
#endif
    
#if USE_AUTO_RES_NOISE
    kAutoResNoise,
#endif

    kTransBoost,
    
    kQuality,
    
#if 0
    kGraph,
#endif
    
    kNumParams
};
 
const int kNumPresets = 1;

enum ELayout
{
    kWidth = PLUG_WIDTH,
    kHeight = PLUG_HEIGHT,

    kKnobWidth = 72,
    kKnobHeight = 72,

    kKnobSmallWidth = 36,
    kKnobSmallHeight = 36,
    
    kGraphX = 0,
    kGraphY = 0,

    kLearnX = 32,
    kLearnY = 234,
  
    kNoiseOnlyX = 32,
    kNoiseOnlyY = 283,
  
    kThresholdX = 281,
    kThresholdY = 316,
  
    kRatioX = 172,
    kRatioY = 282,

#if !USE_DROP_DOWN_MENU
    kRadioButtonsQualityX = 430,
    kRadioButtonsQualityY = 260,
    kRadioButtonsQualityVSize = 83,
    kRadioButtonsQualityNumButtons = 4,
#else
    kQualityX = 348,
    kQualityY = 255,
    kQualityWidth = 80,
#endif
    
    kTransBoostX = 281,
    kTransBoostY = 218,
  
#if USE_RESIDUAL_DENOISE
    kResNoiseThrsX = 372,
    kResNoiseThrsY = 316,
#endif
  
#if USE_AUTO_RES_NOISE
    kAutoResNoiseX = 32,
    kAutoResNoiseY = 332,
#endif
};


//
Denoiser::Denoiser(const InstanceInfo &info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
    TRACE;
    
    InitNull();
    InitParams();

    Init(mOverlapping, mFreqRes);
  
#if IPLUG_EDITOR // http://bit.ly/2S64BDd
    mMakeGraphicsFunc = [&]() { return this->MyMakeGraphics(); };
    
    mLayoutFunc = [&](IGraphics* pGraphics) { this->MyMakeLayout(pGraphics); };
#endif
    
    BL_PROFILE_RESET;
}

Denoiser::~Denoiser()
{
    if (mGUIHelper != NULL)
        delete mGUIHelper;
    
    if (mFftObj != NULL)
        delete mFftObj;

    for (int i = 0; i < mDenoiserObjs.size(); i++)
    {
        if (mDenoiserObjs[i] != NULL)
            delete mDenoiserObjs[i];
    }
    
    //
    if (mFreqAxis != NULL)
        delete mFreqAxis;
    
    if (mHAxis != NULL)
        delete mHAxis;
    
    if (mVAxis != NULL)
        delete mVAxis;
    
    if (mAmpAxis != NULL)
        delete mAmpAxis;
    
    if (mSignalCurve != NULL)
        delete mSignalCurve;
    if (mSignalCurveSmooth != NULL)
        delete mSignalCurveSmooth;
    
    if (mNoiseCurve != NULL)
        delete mNoiseCurve;
    if (mNoiseCurveSmooth != NULL)
        delete mNoiseCurveSmooth;
    
    if (mNoiseProfileCurve != NULL)
        delete mNoiseProfileCurve;
    if (mNoiseProfileCurveSmooth != NULL)
        delete mNoiseProfileCurveSmooth;
}

IGraphics *
Denoiser::MyMakeGraphics()
{
    int fps = BLUtilsPlug::GetPlugFPS(PLUG_FPS);
    
    IGraphics *graphics = MakeGraphics(*this,
                                       PLUG_WIDTH, PLUG_HEIGHT,
                                       fps,
                                       GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
    
    return graphics;
}

void
Denoiser::MyMakeLayout(IGraphics *pGraphics)
{
    ENTER_PARAMS_MUTEX;

    // IGraphics: DEFAULT_FONT
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    
    pGraphics->LoadFont("font-regular", FONT_REGULAR_FN);
    pGraphics->LoadFont("font-light", FONT_LIGHT_FN);
    pGraphics->LoadFont("font-bold", FONT_BOLD_FN);

    // Style: BLULAB_V3
    pGraphics->LoadFont("OpenSans-ExtraBold", FONT_OPENSANS_EXTRA_BOLD_FN);
    pGraphics->LoadFont("Roboto-Bold", FONT_ROBOTO_BOLD_FN);
    
    pGraphics->AttachBackground(BACKGROUND_FN);
    
#if 0 // Debug
    pGraphics->ShowControlBounds(true);
#endif
    
    // For rollover buttons
    pGraphics->EnableMouseOver(true);

    pGraphics->EnableTooltips(true);
    pGraphics->SetTooltipsDelay(TOOLTIP_DELAY);
    
    CreateControls(pGraphics);

    ApplyParams();
    
    // Demo mode
    mDemoManager.Init(this, pGraphics);
    
    mUIOpened = true;
    
    LEAVE_PARAMS_MUTEX;
}

void
Denoiser::InitNull()
{
    BLUtilsPlug::PlugInits();
    
    mUIOpened = false;
    mControlsCreated = false;
    mIsInitialized = false;
    
    // Init WDL FFT
    FftProcessObj16::Init();
    
    mFftObj = NULL;
    
    mDenoiserObjs.clear();
    
    mGUIHelper = NULL;
    
    mGraph = NULL;
    
    mAmpAxis = NULL;
    mHAxis = NULL;
    
    mFreqAxis = NULL;
    mVAxis = NULL;
    
    // 
    mSignalCurve = NULL;
    mSignalCurveSmooth = NULL;
    
    mNoiseCurve = NULL;
    mNoiseCurveSmooth = NULL;
    
    mNoiseProfileCurve = NULL;
    mNoiseProfileCurveSmooth = NULL;

    mResNoiseKnob = NULL;
 
    mOverlapping = OVERLAPPING_0;
    mFreqRes = FREQ_RES;
}

void
Denoiser::InitParams()
{
    mOutputNoiseOnly = false;
    //GetParam(kNoiseOnly)->InitInt("NoiseOnly", 0, 0, 1);
    GetParam(kNoiseOnly)->InitEnum("NoiseOnly", 0, 2,
                                   "", IParam::kFlagsNone, "",
                                   "Off", "On");

    mLearnFlag = false;
    GetParam(kLearn)->InitEnum("Learn", 0, 2,
                               "", IParam::kFlagsNone, "",
                               "Off", "On");

    // FIX: StudioOne4 - Win 7 - VST3 (and some other VST3): 
    // start and set res noise to max => we still heared the sound 
    //mRatio = 0.5;
    mRatio = 1.0;

    BL_FLOAT defaultRatio = 100.0;
    GetParam(kRatio)->InitDouble("Ratio", defaultRatio, 0.0, 100.0, 0.1, "%");

    //BL_FLOAT defaultThreshold = 0.001;
    BL_FLOAT defaultThreshold = 0.1;
    mThreshold = defaultThreshold*0.01;
  
    GetParam(kThreshold)->InitDouble("Threshold", defaultThreshold,
                                     //0.0, 1.0, 0.001, "",
                                     //0.0, 100.0, 0.1, "%",
                                     0.0, 100.0, 0.01, "%",
                                     0, "", IParam::ShapePowCurve(6.0));
  
#if USE_RESIDUAL_DENOISE
    BL_FLOAT defaultResNoiseThrs = 0.0;
    mResNoiseThrs = defaultResNoiseThrs;
    GetParam(kResNoiseThrs)->InitDouble("ResNoiseThrs", defaultResNoiseThrs,
                                        //0.0, 1.0, 0.01, "");
                                        0.0, 100.0, 0.1, "%");
#endif

    BL_FLOAT defaultTransBoost = 0.0;
    mTransBoost = defaultTransBoost;
    GetParam(kTransBoost)->InitDouble("TransBoost", defaultTransBoost,
                                      0.0, 100.0, 0.1, "%");

    // Quality
    Quality defaultQuality = STANDARD;
    mQuality = defaultQuality;
#if !USE_DROP_DOWN_MENU
    GetParam(kQuality)->InitInt("Quality", (int)defaultQuality, 0, 3);
#else
    GetParam(kQuality)->InitEnum("Quality", 0, 4, "", IParam::kFlagsNone,
                                 "", "1 - Fast", "2", "3", "4 - Best");
#endif
    
#if USE_AUTO_RES_NOISE
    // Auto res noise
    //GetParam(kAutoResNoise)->InitInt("SoftDeNoise", 0, 0, 1);
    GetParam(kAutoResNoise)->InitEnum("SoftDeNoise", 0, 2,
                                      "", IParam::kFlagsNone, "",
                                      "Off", "On");
#endif
}

void
Denoiser::ApplyParams()
{	
    for (int i = 0; i < mDenoiserObjs.size(); i++)
    {
        if (mDenoiserObjs[i] != NULL)
            mDenoiserObjs[i]->SetThreshold(mThreshold);
    }
      
    // Threshold has changed.
    // Update the noise pattern curve
    UpdateGraphNoisePattern();
      
    for (int i = 0; i < mDenoiserObjs.size(); i++)
    {
        if (mDenoiserObjs[i] != NULL)
            mDenoiserObjs[i]->SetBuildingNoiseStatistics(mLearnFlag);
    }
            
    QualityChanged();
      
#if USE_RESIDUAL_DENOISE
    for (int i = 0; i < mDenoiserObjs.size(); i++)
    {
        if (mDenoiserObjs[i] != NULL)
            mDenoiserObjs[i]->SetResNoiseThrs(mResNoiseThrs);
    }
#endif
         
#if USE_AUTO_RES_NOISE  
    for (int i = 0; i < mDenoiserObjs.size(); i++)
    {
        if (mDenoiserObjs[i] != NULL)
            mDenoiserObjs[i]->SetAutoResNoise(mAutoResNoise);
    }

    if (mResNoiseKnob != NULL)
    {
        // Also disable knob value automatically
        IGraphics *graphics = GetUI();
        if (graphics != NULL)
            graphics->DisableControl(kResNoiseThrs, mAutoResNoise);
    }
#endif
}

void
Denoiser::Init(int oversampling, int freqRes)
{
    if (mIsInitialized)
        return;
    
    BL_FLOAT sampleRate = GetSampleRate();
    
    int bufferSize = BUFFER_SIZE;
#if USE_VARIABLE_BUFFER_SIZE
    bufferSize = BLUtilsPlug::PlugComputeBufferSize(BUFFER_SIZE, sampleRate);
#endif
    
    if (mFftObj == NULL)
    {
        mDenoiserObjs.resize(4);

        // Must use a second array
        // because the heritage doesn't convert autmatically from
        // WDL_TypedBuf<PostTransientFftObj2 *> to WDL_TypedBuf<ProcessObj *>
        // (tested with std vector too)
        vector<ProcessObj *> processObjs;
    
        // Signal
        for (int i = 0; i < 2; i++)
        {
            DenoiserObj *denoiserObj = new DenoiserObj(this, bufferSize,
                                                       mOverlapping, mFreqRes, mThreshold, false);
	
            mDenoiserObjs[i] = denoiserObj;
            processObjs.push_back(denoiserObj);
        }
    
        // Noise
        for (int i = 2; i < 4; i++)
        {
            DenoiserObj *denoiserObj = new DenoiserObj(this, bufferSize,
                                                       mOverlapping, mFreqRes,
                                                       mThreshold, true);
      
            mDenoiserObjs[i] = denoiserObj;
            processObjs.push_back(denoiserObj);
	
            mDenoiserObjs[i - 2]->SetTwinNoiseObj(mDenoiserObjs[i]);
        }
      
        int numChannels = 4;
        int numScInputs = 0;

        mFftObj = new DenoiserPostTransientObj(processObjs,
                                               numChannels, numScInputs,
                                               bufferSize, oversampling, freqRes,
                                               sampleRate,
                                               TRANSIENT_FREQ_AMP_RATIO,
                                               TRANSIENT_BOOST_FACTOR);
    
        // Noise
        mFftObj->SetSkipFftProcess(2, true);
        mFftObj->SetSkipFftProcess(3, true);
      
        // Trans
        mFftObj->SetSkipFftProcess(4, true);
        mFftObj->SetSkipFftProcess(5, true);
    
#if !VARIABLE_HANNING
        mFftObj->SetAnalysisWindow(FftProcessObj16::ALL_CHANNELS,
                                   FftProcessObj16::WindowHanning);
      
        mFftObj->SetSynthesisWindow(FftProcessObj16::ALL_CHANNELS,
                                    FftProcessObj16::WindowHanning);
#else
        mFftObj->SetAnalysisWindow(FftProcessObj16::ALL_CHANNELS,
                                   FftProcessObj16::WindowVariableHanning);
        mFftObj->SetSynthesisWindow(FftProcessObj16::ALL_CHANNELS,
                                    FftProcessObj16::WindowVariableHanning);
#endif
      
        mFftObj->SetKeepSynthesisEnergy(FftProcessObj16::ALL_CHANNELS,
                                        KEEP_SYNTHESIS_ENERGY);
    }
    else
    {
        mFftObj->Reset(bufferSize, oversampling, freqRes, sampleRate);
    }
    
    UpdateLatency();
    
    ApplyParams();
    
    mIsInitialized = true;
}

void
Denoiser::ProcessBlock(iplug::sample **inputs, iplug::sample **outputs, int nFrames)
{
    // Mutex is already locked for us.

    //if (BLDebug::ExitAfter(this, 10))
    //    return;
    
    // Be sure to have sound even when the UI is closed
    BLUtilsPlug::BypassPlug(inputs, outputs, nFrames);

    mBLUtilsPlug.CheckReset(this);
    
    if (!mIsInitialized)
        return;

    if (mQualityChanged)
        OnReset();
    mQualityChanged = false;
  
    BL_PROFILE_BEGIN;
    
    FIX_FLT_DENORMAL_INIT()
    
    vector<WDL_TypedBuf<BL_FLOAT> > &in = mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > &scIn = mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > &out = mTmpBuf2;
    BLUtilsPlug::GetPlugIOBuffers(this, inputs, outputs, nFrames,
                              &in, &scIn, &out);
  
    // Cubase 10, Sierra
    // in or out can be empty...
    if (in.empty() || out.empty())
    {
        if (mGraph != NULL)
            mGraph->PushAllData();
        
        return;
    }
 
    // Warning: there is a bug in Logic EQ plugin:
    // - when not playing, ProcessDoubleReplacing is still called continuously
    // - and the values are not zero ! (1e-5 for example)
    // This is the same for Protools, and if the plugin consumes,
    // this slows all without stop
    // For example when selecting "offline"
    // Can be the case if we switch to the offline quality option:
    // All slows down, and Protools or Logix doesn't prompt for insufficient resources
    mSecureRestarter.Process(in);
    
    // Set the outputs to 0
    //
    // FIX: on Protools, in render mode, after play is finished,
    // there is a buzz sound
    for (int i = 0; i < out.size(); i++)
    {
        WDL_TypedBuf<BL_FLOAT> &out0 = out[i];
        BLUtils::FillAllZero(&out0);
    }

    // We need duplicated data (one for signal, one for noise)
    // Duplicate the input
    vector<WDL_TypedBuf<BL_FLOAT> > &in4 = mTmpBuf3;
    in4.resize(4);
    if (in.size() > 0)
    {
        in4[0] = in[0];
        in4[2] = in[0];
    }
    
    if (in.size() > 1)
    {
        in4[1] = in[1];
        in4[3] = in[1];
    }

    // Duplicate the output
    vector<WDL_TypedBuf<BL_FLOAT> > &out4 = mTmpBuf4;
    out4.resize(4);
    if (out.size() > 0)
    {
        out4[0] = out[0];
        // Noise buffer
        out4[2] = out[0];
    }
    
    if (out.size() > 1)
    {
        out4[1] = out[1];
        // Noise buffer
        out4[3] = out[1];
    }

    //
    mFftObj->SetTransBoost(mTransBoost);
    
    // Process signal and noise
    mFftObj->Process(in4, scIn, &out4);

    // Apply attenuation
    for (int i = 0; i < 2; i++)
    {
        if (out4[i].GetSize() == nFrames)
        {
            BL_FLOAT *outBuf = out4[i].Get();
            BL_FLOAT *noiseBuf = out4[i + 2].Get();
        
            ApplyAttenuation(outBuf, noiseBuf, nFrames, mRatio);
        }
    }
    
    if (mOutputNoiseOnly)
        // Output noise instead of signal
    {
        for (int i = 0; i < 2; i++)
        {
            if (out4[i].GetSize() == nFrames)
            {
                if (out4[i].Get() != NULL)
                    memcpy(out4[i].Get(), out4[i + 2].Get(),
                           nFrames*sizeof(BL_FLOAT));
            }
        }
    }

    // Update the graph
    //WDL_TypedBuf<BL_FLOAT> sbuf[2];
    WDL_TypedBuf<BL_FLOAT> *sbuf = mTmpBuf5;
    for (int i = 0; i < 2; i++)
    {
        // The two signal objs
        mDenoiserObjs[i]->GetSignalBuffer(&sbuf[i]);
    }
    
    // Take the noise buffer in the frequency domain !
    WDL_TypedBuf<BL_FLOAT> *nbuf = mTmpBuf6;
    for (int i = 0; i < 2; i++)
    {
        // The two signal objs
        mDenoiserObjs[i]->GetNoiseBuffer(&nbuf[i]);
    }
    
    WDL_TypedBuf<BL_FLOAT> &signalBuf = mTmpBuf7;
    BLUtils::StereoToMono(&signalBuf, sbuf[0].Get(),
                          sbuf[1].Get(), sbuf[0].GetSize());
  
    WDL_TypedBuf<BL_FLOAT> &noiseBuf = mTmpBuf8;
    BLUtils::StereoToMono(&noiseBuf, nbuf[0].Get(),
                          nbuf[1].Get(), nbuf[0].GetSize());
    
    UpdateGraphNoisePattern();

    if (mUIOpened)
    { 
        if (!mLearnFlag)
            // We are removing the noise, so display the suppressed noise curve
            UpdateGraph(&signalBuf, &noiseBuf);
        else
            // We are learning the noise profile,
            // don't display the extracted noise (nonsense)
            UpdateGraph(&signalBuf, NULL);
    }
    
    vector<WDL_TypedBuf<BL_FLOAT> > &resultOut = mTmpBuf14;
    resultOut.resize(2);
    resultOut[0] = out4[0];
    resultOut[1] = out4[1];
    
    BLUtilsPlug::PlugCopyOutputs(resultOut, outputs, nFrames);
    
    // Demo mode
    if (mDemoManager.MustProcess())
    {
        mDemoManager.Process(outputs, nFrames);
    }

    if (mGraph != NULL)
        mGraph->PushAllData();
    
    BL_PROFILE_END;
}

void
Denoiser::CreateControls(IGraphics *pGraphics)
{
    if (mGUIHelper == NULL)
        mGUIHelper = new GUIHelper12(GUIHelper12::STYLE_BLUELAB_V3);

    mGUIHelper->AttachToolTipControl(pGraphics);
    mGUIHelper->AttachTextEntryControl(pGraphics);
    
    mGraph = mGUIHelper->CreateGraph(this, pGraphics,
                                     kGraphX, kGraphY,
                                     GRAPH_FN /*kGraph*/);
    // Separator
    IColor sepIColor;
    mGUIHelper->GetGraphSeparatorColor(&sepIColor);
    int sepColor[4] = { sepIColor.R, sepIColor.G, sepIColor.B, sepIColor.A };
    mGraph->SetSeparatorY0(2.0, sepColor);
    
    CreateGraphAxes();
    CreateGraphCurves();

    mGUIHelper->CreateToggleButton(pGraphics,
                                   kNoiseOnlyX,
                                   kNoiseOnlyY,
                                   CHECKBOX_FN, kNoiseOnly, "NOISE ONLY",
                                   GUIHelper12::SIZE_SMALL,
                                   true,
                                   tooltipNoiseOnly);

    mGUIHelper->CreateToggleButton(pGraphics,
                                   kLearnX,
                                   kLearnY,
                                   CHECKBOX_FN, kLearn, "LEARN",
                                   GUIHelper12::SIZE_SMALL,
                                   true,
                                   tooltipLearn);
   
    mGUIHelper->CreateKnobSVG(pGraphics,
                              kRatioX, kRatioY,
                              kKnobWidth, kKnobHeight,
                              KNOB_FN,
                              kRatio,
                              TEXTFIELD_FN,
                              "RATIO",
                              GUIHelper12::SIZE_DEFAULT,
                              NULL, true,
                              tooltipRatio);
    
    mGUIHelper->CreateKnobSVG(pGraphics,
                              kThresholdX, kThresholdY,
                              kKnobSmallWidth, kKnobSmallHeight,
                              KNOB_SMALL_FN,
                              kThreshold,
                              TEXTFIELD_FN,
                              "THRS",
                              GUIHelper12::SIZE_SMALL,
                              NULL, true,
                              tooltipThreshold);
    
    mResNoiseKnob = mGUIHelper->CreateKnobSVG(pGraphics,
                                              kResNoiseThrsX, kResNoiseThrsY,
                                              kKnobSmallWidth, kKnobSmallHeight,
                                              KNOB_SMALL_FN,
                                              kResNoiseThrs,
                                              TEXTFIELD_FN,
                                              "RES NOISE",
                                              GUIHelper12::SIZE_SMALL,
                                              NULL, true,
                                              tooltipResNoiseThrs);

    mGUIHelper->CreateKnobSVG(pGraphics,
                              kTransBoostX, kTransBoostY,
                              kKnobSmallWidth, kKnobSmallHeight,
                              KNOB_SMALL_FN,
                              kTransBoost,
                              TEXTFIELD_FN,
                              "TR BOOST",
                              GUIHelper12::SIZE_SMALL,
                              NULL, true,
                              tooltipTransBoost);

#if !USE_DROP_DOWN_MENU
    const char *radioLabels[kRadioButtonsQualityNumButtons] =
        { "FAST 1", "2", "3", "BEST 4" };
    mGUIHelper->CreateRadioButtons(pGraphics,
                                   kRadioButtonsQualityX,
                                   kRadioButtonsQualityY,
                                   RADIOBUTTON_FN,
                                   kRadioButtonsQualityNumButtons,
                                   kRadioButtonsQualityVSize,
                                   kQuality,
                                   false,
                                   "QUALITY",
                                   EAlign::Far,
                                   EAlign::Far,
                                   radioLabels);
#else
    mGUIHelper->CreateDropDownMenu(pGraphics,
                                   kQualityX, kQualityY,
                                   kQualityWidth,
                                   kQuality,
                                   "QUALITY",
                                   GUIHelper12::SIZE_DEFAULT,
                                   tooltipQuality);
#endif
    
#if USE_AUTO_RES_NOISE
    mGUIHelper->CreateToggleButton(pGraphics,
                                   kAutoResNoiseX,
                                   kAutoResNoiseY,
                                   CHECKBOX_FN, kAutoResNoise, "SOFT DENOISE",
                                   GUIHelper12::SIZE_SMALL,
                                   true,
                                   tooltipAutoResNoise);
#endif
    
    // Version
    mGUIHelper->CreateVersion(this, pGraphics, PLUG_VERSION_STR);
    
    // Logo
    //mGUIHelper->CreateLogoAnim(this, pGraphics, LOGO_FN,
    //                           kLogoAnimFrames, GUIHelper12::BOTTOM);
    
    // Plugin name
    mGUIHelper->CreatePlugName(this, pGraphics, PLUGNAME_FN, GUIHelper12::BOTTOM);
    
    // Help button
    mGUIHelper->CreateHelpButton(this, pGraphics,
                                 HELP_BUTTON_FN, MANUAL_FN,
                                 GUIHelper12::BOTTOM,
                                 tooltipHelp);
    
    mGUIHelper->CreateDemoMessage(pGraphics);
    
    mControlsCreated = true;
}

void
Denoiser::OnHostIdentified()
{
    BLUtilsPlug::SetPlugResizable(this, false);
}

void
Denoiser::OnReset()
{
    TRACE;
    ENTER_PARAMS_MUTEX;

    BL_FLOAT sampleRate = GetSampleRate();
   
    // Logic X has a bug: when it restarts after stop,
    // sometimes it provides full volume directly, making a sound crack
    mSecureRestarter.Reset();
        
    int bufferSize = BUFFER_SIZE;
    
#if USE_VARIABLE_BUFFER_SIZE
    bufferSize = BLUtilsPlug::PlugComputeBufferSize(BUFFER_SIZE, sampleRate);
#endif

    for (int i = 0; i < mDenoiserObjs.size(); i++)
        mDenoiserObjs[i]->Reset(bufferSize, mOverlapping, mFreqRes, sampleRate);
    
    if (mFftObj != NULL)
    {
        // Called when we restart the playback
        // The cursor position may have changed
        // Then we must reset
        mFftObj->Reset(bufferSize, mOverlapping, mFreqRes, sampleRate);
    }
    
    if (mFreqAxis != NULL)
    {
        // Must not reset directly, otherwise it will make a data race
        // when we change quality.
        // Instead, trigger a reset, and apply it in OnIdle()
        mFreqAxis->SetResetParams(BUFFER_SIZE, sampleRate);
    }
    
    if (mSignalCurve != NULL)
        mSignalCurve->SetXScale(Scale::LOG, 0.0, sampleRate*0.5);
    if (mNoiseCurve != NULL)
        mNoiseCurve->SetXScale(Scale::LOG, 0.0, sampleRate*0.5);
    if (mNoiseProfileCurve != NULL)
        mNoiseProfileCurve->SetXScale(Scale::LOG, 0.0, sampleRate*0.5);

    if (mSignalCurveSmooth != NULL)
        mSignalCurveSmooth->Reset(sampleRate);
    if (mNoiseCurveSmooth != NULL)
        mNoiseCurveSmooth->Reset(sampleRate);
    if (mNoiseProfileCurveSmooth != NULL)
        mNoiseProfileCurveSmooth->Reset(sampleRate);
    
    UpdateLatency();
    
    LEAVE_PARAMS_MUTEX;
    
    BL_PROFILE_RESET;
}

// NOTE: OnIdle() is called from the GUI thread
void
Denoiser::OnIdle()
{
    if (!mUIOpened)
        return;
    
    ENTER_PARAMS_MUTEX;

    mNoiseProfileCurveSmooth->PullData();
    mSignalCurveSmooth->PullData();
    mNoiseCurveSmooth->PullData();

    // Special case for freq axis
    if (mFreqAxis != NULL)
    {
        // Check if a reset has been previously triggered, and apply it so.
        if (mFreqAxis->MustReset())
            mFreqAxis->Reset();
    }
    
    LEAVE_PARAMS_MUTEX;

    // We don't need mutex anymore now
    mNoiseProfileCurveSmooth->ApplyData();
    mSignalCurveSmooth->ApplyData();
    mNoiseCurveSmooth->ApplyData();
}

void
Denoiser::OnParamChange(int paramIdx)
{
    if (!mIsInitialized)
        return;
  
    ENTER_PARAMS_MUTEX;
    
    switch (paramIdx)
    {
        case kThreshold:
        {
            BL_FLOAT threshold = GetParam(kThreshold)->Value();

            threshold = threshold * 0.01;
            
            mThreshold = threshold;
	
            for (int i = 0; i < mDenoiserObjs.size(); i++)
            {
                if (mDenoiserObjs[i] != NULL)
                    mDenoiserObjs[i]->SetThreshold(mThreshold);
            }
      
            // Threshold has changed.
            // Update the noise pattern curve
            UpdateGraphNoisePattern();
        }
        break;
    
        case kRatio:
        {
            BL_FLOAT ratio = GetParam(kRatio)->Value();
            mRatio = ratio/100.0;
        }
        break;
      
        case kNoiseOnly:
        {
            int value = GetParam(kNoiseOnly)->Value();
            mOutputNoiseOnly = (value == 1);
        }
        break;
    
        case kLearn:
        {
            int value = GetParam(kLearn)->Value();
            bool learnFlag = (value == 1);
      
            mLearnFlag = learnFlag;
      
            for (int i = 0; i < mDenoiserObjs.size(); i++)
            {
                if (mDenoiserObjs[i] != NULL)
                    mDenoiserObjs[i]->SetBuildingNoiseStatistics(mLearnFlag);
            }
        }
        break;
      
        case kQuality:
        {
            Quality qual = (Quality)GetParam(paramIdx)->Int();
      
            if (qual != mQuality)
            {
                mQuality = qual;
        
                QualityChanged();
            }
        }
        break;
      
#if USE_RESIDUAL_DENOISE
        case kResNoiseThrs:
        {
            BL_FLOAT threshold = GetParam(kResNoiseThrs)->Value();

            threshold = threshold * 0.01;
            
            mResNoiseThrs = threshold;
      
            for (int i = 0; i < mDenoiserObjs.size(); i++)
            {
                if (mDenoiserObjs[i] != NULL)
                    mDenoiserObjs[i]->SetResNoiseThrs(mResNoiseThrs);
            }
        }
        break;
#endif
      
        case kTransBoost:
        {
            BL_FLOAT transBoost = GetParam(kTransBoost)->Value(); 
            mTransBoost = transBoost/100.0;
        }
      
#if USE_AUTO_RES_NOISE
        case kAutoResNoise:
        {
            int value = GetParam(kAutoResNoise)->Value();
            bool autoResNoiseFlag = (value == 1);
            mAutoResNoise = autoResNoiseFlag;
      
            for (int i = 0; i < mDenoiserObjs.size(); i++)
            {
                if (mDenoiserObjs[i] != NULL)
                    mDenoiserObjs[i]->SetAutoResNoise(mAutoResNoise);
            }

            if (mResNoiseKnob != NULL)
            {
                mResNoiseKnob->SetDisabled(mAutoResNoise);
                
                // Also disable knob value automatically
                IGraphics *graphics = GetUI();
                if (graphics != NULL)
                    graphics->DisableControl(kResNoiseThrs, mAutoResNoise);   
            }
        }
        break;
#endif
        
        default:
            break;
    }
    
    LEAVE_PARAMS_MUTEX;
}

void
Denoiser::OnUIOpen()
{
    IEditorDelegate::OnUIOpen();
    
    // Make sure we are not currently processing block
    // (process block send stuff to UI)
    ENTER_PARAMS_MUTEX;
    
    LEAVE_PARAMS_MUTEX;
}

void
Denoiser::OnUIClose()
{
    // Make sure we are not currently processing block
    // (process block send stuff to UI)
    ENTER_PARAMS_MUTEX;
    
    // Make sure we won't go to ProcessBlock() again
    mUIOpened = false;

    mGraph = NULL;
    
    mResNoiseKnob = NULL;
    
    LEAVE_PARAMS_MUTEX;
}

void
Denoiser::CreateGraphAxes()
{
    // Create
    if (mHAxis == NULL)
    {
        mHAxis = new GraphAxis2();
        mFreqAxis = new GraphFreqAxis2(true, Scale::LOG);
    }
    
    if (mVAxis == NULL)
    {
        mVAxis = new GraphAxis2();
        mAmpAxis = new GraphAmpAxis();
    }
    
    // Update
    mGraph->SetHAxis(mHAxis);
    mGraph->SetVAxis(mVAxis);
    
    BL_FLOAT sampleRate = GetSampleRate();
    int graphWidth = mGraph->GetRECT().W();
    
    bool horizontal = true;
    mFreqAxis->Init(mHAxis, mGUIHelper, horizontal, BUFFER_SIZE,
                    sampleRate, graphWidth);
    mFreqAxis->Reset(BUFFER_SIZE, sampleRate);
    
    mAmpAxis->Init(mVAxis, mGUIHelper, DENOISER_MIN_DB, DENOISER_MAX_DB, graphWidth);
}

void
Denoiser::CreateGraphCurves()
{
#define REF_SAMPLERATE 44100.0
    BL_FLOAT curveSmoothCoeff =
        ParamSmoother2::ComputeSmoothFactor(CURVE_SMOOTH_COEFF_MS, REF_SAMPLERATE);
    
    if (mSignalCurve == NULL)
        // Not yet created
    {
        BL_FLOAT sampleRate = GetSampleRate();
        
        int descrColor[4];
        mGUIHelper->GetGraphCurveDescriptionColor(descrColor);
    
        float fillAlpha = mGUIHelper->GetGraphCurveFillAlpha();
        
        // Denoiser curve
        int signalColor[4];
        mGUIHelper->GetGraphCurveColorBlue(signalColor);
        
        mSignalCurve = new GraphCurve5(GRAPH_CURVE_NUM_VALUES);
        mSignalCurve->SetDescription("signal", descrColor);
        mSignalCurve->SetXScale(Scale::LOG, 0.0, sampleRate*0.5);
        mSignalCurve->SetYScale(Scale::DB, DENOISER_MIN_DB, DENOISER_MAX_DB);
        mSignalCurve->SetFill(true);
        mSignalCurve->SetFillAlpha(fillAlpha);
        mSignalCurve->SetColor(signalColor[0], signalColor[1], signalColor[2]);
        mSignalCurve->SetLineWidth(2.0);
        
        mSignalCurveSmooth = new SmoothCurveDB(mSignalCurve,
                                               //CURVE_SMOOTH_COEFF,
                                               curveSmoothCoeff,
                                               GRAPH_CURVE_NUM_VALUES,
                                               DENOISER_MIN_DB,
                                               DENOISER_MIN_DB, DENOISER_MAX_DB,
                                               sampleRate);
	
        // Noise curve
        int noiseColor[4];
        mGUIHelper->GetGraphCurveColorLightBlue(noiseColor);
        
        mNoiseCurve = new GraphCurve5(GRAPH_CURVE_NUM_VALUES);
        mNoiseCurve->SetDescription("noise", descrColor);
        mNoiseCurve->SetXScale(Scale::LOG, 0.0, sampleRate*0.5);
        mNoiseCurve->SetYScale(Scale::DB, DENOISER_MIN_DB, DENOISER_MAX_DB);
        mNoiseCurve->SetFillAlpha(fillAlpha);
        mNoiseCurve->SetColor(noiseColor[0], noiseColor[1], noiseColor[2]);
        
        mNoiseCurveSmooth = new SmoothCurveDB(mNoiseCurve,
                                              //CURVE_SMOOTH_COEFF,
                                              curveSmoothCoeff,
                                              GRAPH_CURVE_NUM_VALUES,
                                              DENOISER_MIN_DB,
                                              DENOISER_MIN_DB, DENOISER_MAX_DB,
                                              sampleRate);
        
        // Noise profile curve
        int noiseProfileColor[4];
        //mGUIHelper->GetGraphCurveColorGreen(noiseProfileColor);
        mGUIHelper->GetGraphCurveColorOrange(noiseProfileColor);
        
        mNoiseProfileCurve = new GraphCurve5(GRAPH_CURVE_NUM_VALUES);
        mNoiseProfileCurve->SetDescription("noise profile", descrColor);
        mNoiseProfileCurve->SetXScale(Scale::LOG, 0.0, sampleRate*0.5);
        mNoiseProfileCurve->SetYScale(Scale::DB, DENOISER_MIN_DB, DENOISER_MAX_DB);
        mNoiseProfileCurve->SetFillAlpha(fillAlpha);
        mNoiseProfileCurve->SetColor(noiseProfileColor[0],
                                     noiseProfileColor[1],
                                     noiseProfileColor[2]);

        BL_FLOAT curveSmoothCoeffNoiseProfile =
            ParamSmoother2::ComputeSmoothFactor(CURVE_SMOOTH_COEFF_NOISE_PROFILE_MS,
                                                REF_SAMPLERATE);
        mNoiseProfileCurveSmooth = new SmoothCurveDB(mNoiseProfileCurve,
                                                     //CURVE_SMOOTH_COEFF,
                                                     //curveSmoothCoeff,
                                                     curveSmoothCoeffNoiseProfile,
                                                     GRAPH_CURVE_NUM_VALUES,
                                                     DENOISER_MIN_DB,
                                                     DENOISER_MIN_DB,
                                                     DENOISER_MAX_DB,
                                                     sampleRate);
    }

    int graphSize[2];
    mGraph->GetSize(&graphSize[0], &graphSize[1]);
    
    mSignalCurve->SetViewSize(graphSize[0], graphSize[1]);
    mGraph->AddCurve(mSignalCurve);
    
    mNoiseCurve->SetViewSize(graphSize[0], graphSize[1]);
    mGraph->AddCurve(mNoiseCurve);
    
    mNoiseProfileCurve->SetViewSize(graphSize[0], graphSize[1]);
    mGraph->AddCurve(mNoiseProfileCurve);
}

void
Denoiser::UpdateLatency()
{
    if (mFftObj == NULL)
        return;
    
    int bufferSize = BUFFER_SIZE;
    BL_FLOAT sampleRate = GetSampleRate();
  
#if USE_VARIABLE_BUFFER_SIZE
    bufferSize = BLUtilsPlug::PlugComputeBufferSize(BUFFER_SIZE, sampleRate);
#endif
  
    int blockSize = GetBlockSize();
    int latency = mFftObj->ComputeLatency(blockSize);

    // Correct version for latency
    if (mDenoiserObjs[0] != NULL)
    {
        int objLatency = mDenoiserObjs[0]->GetLatency();
        latency += objLatency;
    }
    
    SetLatency(latency);
}

void
Denoiser::ApplyAttenuation(BL_FLOAT *buf, BL_FLOAT *residue,
                           int numSamples, BL_FLOAT attenuation)
{
    // Residue level if the inverse of attenuation
    for (int i = 0; i < numSamples; i++)
        buf[i] += (1.0 - attenuation)*residue[i];
}

BL_FLOAT
Denoiser::ComputeRMSAvg(const BL_FLOAT *output, int nFrames)
{
    BL_FLOAT avg = 0.0;
  
    for (int i = 0; i < nFrames; i++)
    {
        BL_FLOAT value = output[i];
        avg += value*value;
    }
  
    avg = sqrt(avg/nFrames);
  
    return avg;
}

void
Denoiser::UpdateGraph(WDL_TypedBuf<BL_FLOAT> *signal,
                      WDL_TypedBuf<BL_FLOAT> *noise)
{
    if (!mUIOpened)
        return;
    
    // input
    if (signal != NULL)
    {
        mSignalCurveSmooth->SetValues(*signal);
    }
  
    // Noise
    if (noise != NULL)
    {
        mNoiseCurveSmooth->SetValues(*noise);
    }
    else
        // Clear the curve
        mNoiseCurveSmooth->ClearValues();

    mSignalCurveSmooth->PushData();
    mNoiseCurveSmooth->PushData();
}

void
Denoiser::UpdateGraphNoisePattern(WDL_TypedBuf<BL_FLOAT> *noiseStats)
{
    BL_FLOAT bufferSizeCoeff = 1.0;
  
#if USE_VARIABLE_BUFFER_SIZE
    bufferSizeCoeff = BLUtilsPlug::GetBufferSizeCoeff(this, BUFFER_SIZE);
#endif

    if (mUIOpened)
    {
        // noise statistics
        if (noiseStats != NULL)
        {
            mNoiseProfileCurveSmooth->SetValues(*noiseStats);
        }

        mNoiseProfileCurveSmooth->PushData();
    }
}

void
Denoiser::UpdateGraphNoisePattern()
{
    if (mDenoiserObjs.empty())
        return;
    
    // Noise statistics for graph
    WDL_TypedBuf<BL_FLOAT> &noisePattern0 = mTmpBuf9;
    if (mDenoiserObjs[0] != NULL)
        mDenoiserObjs[0]->GetNoisePattern(&noisePattern0);
    else
        noisePattern0.Resize(0);
    
    WDL_TypedBuf<BL_FLOAT> &noisePattern1 = mTmpBuf10;
    if (mDenoiserObjs[1] != NULL)
        mDenoiserObjs[1]->GetNoisePattern(&noisePattern1);
    else
        noisePattern1.Resize(0);
  
    WDL_TypedBuf<BL_FLOAT> &noiseStatsBuf = mTmpBuf11;
    BLUtils::StereoToMono(&noiseStatsBuf, noisePattern0, noisePattern1);
  
    if (!mLearnFlag)
    {
        DenoiserObj::ApplyThresholdToNoiseCurve(&noiseStatsBuf, mThreshold);
    }
  
    UpdateGraphNoisePattern(&noiseStatsBuf);
}

void
Denoiser::QualityChanged()
{
    mOverlapping = OVERLAPPING_0;
  
    switch(mQuality)
    {
        case STANDARD:
            mOverlapping = OVERLAPPING_0;
            break;
      
        case HIGH:
            mOverlapping = OVERLAPPING_1;
            break;
      
        case VERY_HIGH:
            mOverlapping = OVERLAPPING_2;
            break;
      
        case OFFLINE:
            mOverlapping = OVERLAPPING_3;
            break;
      
        default:
            break;
    }
  
    mQualityChanged = true;
}

bool
Denoiser::SerializeState(IByteChunk &pChunk) const
{
    TRACE;
    ((Denoiser *)this)->ENTER_PARAMS_MUTEX;
    
    for (int i = 0; i < mDenoiserObjs.size(); i++)
    {
        DenoiserObj *denoiser = mDenoiserObjs[i];
        WDL_TypedBuf<BL_FLOAT> &noisePattern = ((Denoiser *)this)->mTmpBuf13;
        
        if (denoiser != NULL)
        {
#if !USE_VARIABLE_BUFFER_SIZE
            denoiser->GetNoisePattern(&noisePattern);
#else
            denoiser->GetNativeNoisePattern(&noisePattern);
#endif
            
            int size = noisePattern.GetSize();
            pChunk.Put(&size);
            
            for (int k = 0; k < size; k++)
            {
                BL_FLOAT v = noisePattern.Get()[k];
                pChunk.Put(&v);
            }
        }
        else
            noisePattern.Resize(0);
    }
    
    bool res = /*IPluginBase::*/SerializeParams(pChunk);
    
    ((Denoiser *)this)->LEAVE_PARAMS_MUTEX;
    
    return res;
}
 
int
Denoiser::UnserializeState(const IByteChunk &pChunk, int startPos)
{    
    TRACE;
    ENTER_PARAMS_MUTEX;
    
    for (int i = 0; i < mDenoiserObjs.size(); i++)
    {
        DenoiserObj *denoiser = mDenoiserObjs[i];
        WDL_TypedBuf<BL_FLOAT> &noisePattern = mTmpBuf13;
        
        int size;
        startPos = pChunk.Get(&size, startPos);
        
        noisePattern.Resize(size);
        
        for (int k = 0; k < size; k++)
        {
            BL_FLOAT v;
            startPos = pChunk.Get(&v, startPos);
            
            BL_FLOAT *buf = noisePattern.Get();
            buf[k] = v;
        }
        
        if (denoiser != NULL)
        {
#if !USE_VARIABLE_BUFFER_SIZE
            denoiser->SetNoisePattern(noisePattern);
#else
            denoiser->SetNativeNoisePattern(noisePattern);
#endif
        }
    }
    
    UpdateGraphNoisePattern();
    
    bool res = /*IPluginBase::*/UnserializeParams(pChunk, startPos);
    
    LEAVE_PARAMS_MUTEX;
    
    return res;
}
