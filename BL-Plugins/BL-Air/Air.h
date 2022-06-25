#ifndef __AIR__
#define __AIR__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtilsPlug.h>

using namespace iplug;
using namespace igraphics;

// It now avoids clicks, except if we turn the knob very quickly
#define USE_SPLIT_FREQ_SMOOTHER 1 //0

class FftProcessObj16;
class AirProcess3;
class GraphControl12;
class GraphFreqAxis2;
class GraphAmpAxis;
class GraphAxis2;
class GUIHelper12;
class SmoothCurveDB;
class CrossoverSplitterNBands4;
class DelayObj4;
class FftProcessBufObj;
class Air final : public Plugin
{
public:
    Air(const InstanceInfo &info);
    virtual ~Air();

    void OnHostIdentified() override;
    
    void OnReset() override;
    void OnParamChange(int paramIdx) override;

    void OnUIOpen() override;
    void OnUIClose() override;
    
    void ProcessBlock(iplug::sample **inputs,
                      iplug::sample **outputs, int nFrames) override;

    void OnIdle() override;
    
protected:
    IGraphics *MyMakeGraphics();
    void MyMakeLayout(IGraphics *pGraphics);

    void CreateControls(IGraphics *pGraphics);
    
    void InitNull();
    void InitParams();
    void Init(int oversampling, int freqRes);
    void ApplyParams();
    
    void UpdateMixTextColor(ITextControl *textControl, BL_FLOAT param);
    
    void UpdateLatency();
    
    void CreateGraphAxes();
    void CreateGraphCurves();
    void UpdateCurves();
    
    void UpdateCurvesMixAlpha();

    // Improvement
    void SetSplitFreq(BL_FLOAT freq);

    void UpdateCurvesSmoothFactor();
        
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    FftProcessObj16 *mFftObj;
    AirProcess3 *mAirProcessObjs[2];
    
    GraphControl12 *mGraph;
    
    GraphAmpAxis *mAmpAxis;
    GraphAxis2 *mHAxis;
    
    GraphFreqAxis2 *mFreqAxis;
    GraphAxis2 *mVAxis;
    
    //
    GraphCurve5 *mAirCurve;
    SmoothCurveDB *mAirCurveSmooth;
    
    GraphCurve5 *mHarmoCurve;
    SmoothCurveDB *mHarmoCurveSmooth;
    
    GraphCurve5 *mSumCurve;
    SmoothCurveDB *mSumCurveSmooth;
    
    //
    BL_FLOAT mThreshold;
    
    //
    BL_FLOAT mDetectThreshold;
    BL_FLOAT mMix;
    
    ParamSmoother2 *mOutGainSmoother;
    BL_FLOAT mOutGain;

    bool mUseSoftMasks;
    bool mLatencyChanged;
    
    ICaptionControl *mMixTextControl;
    GUIHelper12 *mGUIHelper;

    //
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;

    // Improvements
    BL_FLOAT mSplitFreq;
    CrossoverSplitterNBands4 *mBandSplittersIn[2];
    CrossoverSplitterNBands4 *mBandSplittersOut[2];
#if USE_SPLIT_FREQ_SMOOTHER
    ParamSmoother2 *mSplitFreqSmoother;
#endif
    ParamSmoother2 *mWetGainSmoother;
    BL_FLOAT mWetGain;

    DelayObj4 *mInputDelays[2];

    // Used just to have the fft corresponding to the output
    FftProcessObj16 *mFftObjOut;
    FftProcessBufObj *mBufObjOut;

    BLUtilsPlug mBLUtilsPlug;
    
 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;

    WDL_TypedBuf<BL_FLOAT> mTmpBuf3;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf4;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf5;

    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf6;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf7;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf8[2];

    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf9;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf10;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf11[2];

    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf12;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf13;
    
    BL_PROFILE_DECLARE;
};

#endif
