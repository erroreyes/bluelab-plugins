#ifndef __ONSET_DETECT__
#define __ONSET_DETECT__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"
//#include "IMidiQueue.h"
#include "IPlugMidi.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#define GAIN_VALUE 12.0

using namespace iplug;
using namespace igraphics;

class GUIHelper12;
class OnsetDetect final : public Plugin
{
 public:
    OnsetDetect(const InstanceInfo &info);
    virtual ~OnsetDetect();
  
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
    void Init(int oversampling, int freqRes);
    void ApplyParams();

    //
    BL_FLOAT mThreshold;
    
    FftProcessObj16 *mFftObj;
    OnsetDetectProcess *mOnsetDetectProcessObj;
  
    //
    DemoModeManager mDemoManager;
  
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;
    
    GUIHelper12 *mGUIHelper;

 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    
    BL_PROFILE_DECLARE;
};

#endif
