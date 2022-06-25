#ifndef __PRECEDENCE__
#define __PRECEDENCE__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtilsPlug.h>

using namespace iplug;
using namespace igraphics;

class DelayObj4;
class ParamSmoother2;
class GUIHelper12;
class Precedence final : public Plugin
{
 public:
    Precedence(const InstanceInfo &info);
    virtual ~Precedence();

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
    void ComputeDelays(bool updateTextFields);

    //
    // From -1 to 1
    BL_FLOAT mNormDelay;
    ParamSmoother2 *mNormDelaySmoother;
  
    // Left delay
    BL_FLOAT mDelayL;
  
    // Right delay
    BL_FLOAT mDelayR;
  
    DelayObj4 *mDelayObjs[2];
  
    // GUI
    ITextControl *mTextControlMs;
    ITextControl *mTextControlSamples;

    DemoModeManager mDemoManager;
  
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;
    
    GUIHelper12 *mGUIHelper;

    BLUtilsPlug mBLUtilsPlug;
    
 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    
    BL_PROFILE_DECLARE;
};

#endif
