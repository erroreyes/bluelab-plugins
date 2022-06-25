#ifndef __SINE__
#define __SINE__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtilsPlug.h>

using namespace iplug;
using namespace igraphics;


class ParamSmoother2;
class KnobSmoother;

class GUIHelper12;
class IBLSwitchControl;
class Sine final : public Plugin
{
 public:
    Sine(const InstanceInfo &info);
    virtual ~Sine();

    void OnHostIdentified() override;
    
    void OnReset() override;
    void OnParamChange(int paramIdx) override;

    void OnUIOpen() override;
    void OnUIClose() override;
    
    void ProcessBlock(iplug::sample **inputs,
                      iplug::sample **outputs, int nFrames) override;

    // Used for disabling monitor button in the right thread.
    void OnIdle() override;
    
 protected:
    IGraphics *MyMakeGraphics();
    void MyMakeLayout(IGraphics *pGraphics);
    
    void CreateControls(IGraphics *pGraphics);
    
    void InitNull();
    void InitParams();
    void Init();
    void ApplyParams();

    //
    
    // Output gain
    BL_FLOAT mOutGain;
    ParamSmoother2 *mOutGainSmoother;

    BL_FLOAT mFreq;
    ParamSmoother2 *mFreqSmoother;

    KnobSmoother *mFreqKnobSmoother;
    
    long long mTime;
  
    BL_FLOAT mPassThru;
  
    // For fixing clicks when turning the freq knob
    BL_FLOAT mPhase;
  
    //
    DemoModeManager mDemoManager;
  
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;
    
    GUIHelper12 *mGUIHelper;

    IBLSwitchControl *mMonitorControl;
    bool mIsPlaying;
    bool mMonitorEnabled;

    BLUtilsPlug mBLUtilsPlug;
    
 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    
    BL_PROFILE_DECLARE;
};

#endif
