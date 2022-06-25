#ifndef __AUTOGAIN__
#define __AUTOGAIN__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>
#include <AutoGainObj2.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtilsPlug.h>

using namespace iplug;
using namespace igraphics;


class FftProcessObj16;
class AutoGainObj2;
class GraphControl12;
class GraphFreqAxis2;
class GraphAmpAxis;
class GraphAxis2;
class GUIHelper12;
class SmoothCurveDB;
class BLVumeter2SidesControl;
class AutoGain final : public Plugin
{
 public:
    AutoGain(const InstanceInfo &info);
    virtual ~AutoGain();

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
    void ApplyParams();
    
    void Init(int oversampling, int freqRes);
    
    void CreateGraphAxes();
    void CreateGraphCurves();
    
    void UpdateCurves();
    
    void SetMode(AutoGainObj2::Mode mode);

    void UpdateVumeterGain();

    void RefreshGainForAutomation();
        
    //
    BL_FLOAT mThreshold;
    BL_FLOAT mPrecision;
    BL_FLOAT mDryWet;
    
    // Sidechain input gain
    // To set the sc and input almost at the same level
    //
    // Othewise, if side chain and current track level are too different,
    // the plugin could amplify a lot, in continuous,
    // to try to make the two curves match
    BL_FLOAT mScGain;
    
    // Smooth the gain, to have more blurry automations
    BL_FLOAT mGainSmooth;
    
    AutoGainObj2::Mode mMode;
    
    BL_FLOAT mGain;
    
    // Num samples processed since the last automation dot
    long mNumSamplesSinceLastDot;
    
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    BLVumeter2SidesControl *mGainVumeter;

    IControl *mScGainControl;
    IControl *mSpeedControl;
    
    int mPrevNFrames;
        
    FftProcessObj16 *mFftObj;
    AutoGainObj2 *mAutoGainObj;
    
    GraphControl12 *mGraph;
    
    GraphAmpAxis *mAmpAxis;
    GraphAxis2 *mHAxis;
    
    GraphFreqAxis2 *mFreqAxis;
    GraphAxis2 *mVAxis;
    
    // Curves
    GraphCurve5 *mSignal0Curve;
    SmoothCurveDB *mSignal0CurveSmooth;
    
    GraphCurve5 *mSignal1Curve;
    SmoothCurveDB *mSignal1CurveSmooth;
    
    GraphCurve5 *mResultCurve;
    SmoothCurveDB *mResultCurveSmooth;
    
    GUIHelper12 *mGUIHelper;
    
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;
    bool mIsInitialized;
    
    // Specific to AutoGain
    bool mCreatingControls;

    BLUtilsPlug mBLUtilsPlug;
    
 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf3;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf4;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf5;
    
    BL_PROFILE_DECLARE;
};

#endif
