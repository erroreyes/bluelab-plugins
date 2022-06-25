#ifndef __REVERB__
#define __REVERB__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtils.h>

using namespace iplug;
using namespace igraphics;

class BLReverb;
class BLReverbSndF;
class MultiViewer2;
class BLReverbViewer;
class ReverbCustomControl;
class ReverbCustomMouseControl;
class SpectrogramDisplay2;
class GUIHelper12;
class Reverb final : public Plugin
{
 public:
    Reverb(const InstanceInfo &info);
    virtual ~Reverb();
  
    void OnReset() override;
    void OnParamChange(int paramIdx) override;
    
    void OnUIOpen() override;
    void OnUIClose() override;
    
    void ProcessBlock(iplug::sample **inputs,
                      iplug::sample **outputs, int nFrames) override;

    int UnserializeState(const IByteChunk& pChunk, int startPos) override;
    
 protected:
    IGraphics *MyMakeGraphics();
    void MyMakeLayout(IGraphics *pGraphics);

    void CreateControls(IGraphics *pGraphics);
    
    void InitNull();
    void InitParams();
    void ApplyParams();
    
    void Init();
    
    //
    friend class ReverbCustomMouseControl;
    void OnMouseUp();
    
    friend class ReverbCustomControl;
    void UpdateTimeZoom(int mouseDelta);
  
    void ApplyPreset(int presetNum);
    void ApplyPreset(BL_FLOAT preset[]);
    
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    GUIHelper12 *mGUIHelper;
    
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;
    bool mIsInitialized;

    GraphControl12 *mGraph;
    SpectrogramDisplay2 *mSpectrogramDisplay;
    SpectrogramDisplay2::SpectrogramDisplayState *mSpectrogramDisplayState;
    
    BLReverbSndF *mReverb;
  
    MultiViewer2 *mMultiViewer;
    BLReverbViewer *mReverbViewer;
  
    ReverbCustomControl *mCustomControl;
    ReverbCustomMouseControl *mCustomMouseControl;
    
    BL_FLOAT mViewerTimeDuration;

    //
    int mOversampFactor;
    BL_FLOAT mEarlyAmount;
    BL_FLOAT mEarlyWet;
    BL_FLOAT mEarlyDry;
    BL_FLOAT mEarlyFactor;
    BL_FLOAT mEarlyWidth;
    BL_FLOAT mRevWidth;
    BL_FLOAT mWet;
    BL_FLOAT mWander;
    BL_FLOAT mBassBoost;
    BL_FLOAT mSpin;
    BL_FLOAT mInputLPF;
    BL_FLOAT mBassLPF;
    BL_FLOAT mDampLPF;
    BL_FLOAT mOutputLPF;
    BL_FLOAT mRT60;
    BL_FLOAT mDelay;    

 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    
    BL_PROFILE_DECLARE;
};

#endif
