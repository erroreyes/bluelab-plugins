#ifndef __SATURATE__
#define __SATURATE__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"
//#include "IMidiQueue.h"
#include "IPlugMidi.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtilsPlug.h>

using namespace iplug;
using namespace igraphics;

class SaturateOverObj;
class ParamSmoother2;
class GUIHelper12;
class GraphControl12;
class GraphAxis2;
class Bufferizer;
class Saturate final : public Plugin
{
 public:
    Saturate(const InstanceInfo &info);
    virtual ~Saturate();

    void OnHostIdentified() override;
    
    void OnReset() override;
    void OnParamChange(int paramIdx) override;

    void OnUIOpen() override;
    void OnUIClose() override;
    
    void ProcessBlock(iplug::sample **inputs,
                      iplug::sample **outputs, int nFrames) override;
    
 protected:
    IGraphics *MyMakeGraphics();
    void MyMakeLayout(IGraphics *pGraphics);
    
    void CreateControls(IGraphics *pGraphics);
    
    void InitNull();
    void InitParams();
    void Init();
    void ApplyParams();
    
    //
    friend class SaturateOverObj;
    static void ComputeSaturation(double inSample, double *outSample, double ratio);

    void CreateGraphAxis();
    void CreateGraphCurve();
    void UpdateCurve(const WDL_TypedBuf<BL_FLOAT> &buf);
    
    //
    SaturateOverObj *mOversampObjs[2];

    BL_FLOAT mRatio;
    ParamSmoother2 *mRatioSmoother;
    
    // Output gain
    BL_FLOAT mOutGain;
    ParamSmoother2 *mOutGainSmoother;

    DemoModeManager mDemoManager;
  
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;
    
    GUIHelper12 *mGUIHelper;

    GraphControl12 *mGraph;
    GraphAxis2 *mVAxis;
    GraphCurve5 *mSaturateCurve;

    Bufferizer *mGraphBufferizer;

    BLUtilsPlug mBLUtilsPlug;
    
 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf3;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf4;
    
    BL_PROFILE_DECLARE;
};

#endif
