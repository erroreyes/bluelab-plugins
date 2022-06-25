#ifndef __REVERB_DEPTH__
#define __REVERB_DEPTH__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtils.h>

using namespace iplug;
using namespace igraphics;

class USTDepthReverbTest;
class MultiViewer2;
class BLReverbViewer;
class ReverbDepthCustomControl;
class ReverbDepthCustomMouseControl;
class SpectrogramDisplay2;
class GUIHelper12;
class ReverbDepth final : public Plugin
{
 public:
    ReverbDepth(const InstanceInfo &info);
    virtual ~ReverbDepth();
  
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
    friend class ReverbDepthCustomMouseControl;
    void OnMouseUp();
    
    friend class ReverbDepthCustomControl;
    void UpdateTimeZoom(int mouseDelta);
  
    //void ApplyPreset(int presetNum);
    //void ApplyPreset(BL_FLOAT preset[]);

    void SaveConfig();

    //
    struct Config
    {
        BL_FLOAT mUseReverbTail;
        
        BL_FLOAT mDry;
        BL_FLOAT mWet;
        
        BL_FLOAT mRoomSize;
        BL_FLOAT mRevWidth;
        BL_FLOAT mDamping;
        
        BL_FLOAT mUseFilter;
        
        BL_FLOAT mUseEarly;
        BL_FLOAT mEarlyRoomSize;
        BL_FLOAT mEarlyIntermicDist;
        BL_FLOAT mEarlyNormDepth;
        
        BL_FLOAT mEarlyOrder;
        BL_FLOAT mEarlyReflectCoeff;
    };
    
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

    //
    USTDepthReverbTest *mReverb;

    Config mConfig;

    //
    MultiViewer2 *mMultiViewer;
    BLReverbViewer *mReverbViewer;
  
    ReverbDepthCustomControl *mCustomControl;
    ReverbDepthCustomMouseControl *mCustomMouseControl;
    
    BL_FLOAT mViewerTimeDuration;

 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    
    BL_PROFILE_DECLARE;
};

#endif
