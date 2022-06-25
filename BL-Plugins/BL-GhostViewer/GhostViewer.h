#ifndef __GHOST_VIEWER__
#define __GHOST_VIEWER__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtils.h>
#include <BLUtilsPlug.h>

#include <SpectroMeter.h>

#include <ResizeGUIPluginInterface.h>

#include <GraphControl12.h>

using namespace iplug;
using namespace igraphics;

class FftProcessObj16;
class SpectrogramDisplayScroll4;
class GhostViewerFftObj;
class GraphControl12;
class IGUIResizeButtonControl;
class GraphTimeAxis6;
class GraphFreqAxis2;
class GraphAxis2;
class GUIHelper12;
class PlugBypassDetector;
class BLTransport;
class IBLSwitchControl;

// New version: re-create the graph each time we close/open the windows, or resize gui
// and keep the data in other objects, not in the graph.
class GhostViewer final : public Plugin,
    public ResizeGUIPluginInterface,
    public GraphCustomControl
{
 public:
    GhostViewer(const InstanceInfo &info);
    virtual ~GhostViewer();

    void OnHostIdentified() override;
    
    void OnReset() override;
    void OnParamChange(int paramIdx) override;
    
    void OnUIOpen() override;
    void OnUIClose() override;
    
    void ProcessBlock(iplug::sample **inputs,
                      iplug::sample **outputs, int nFrames) override;
  
    void PreResizeGUI(int guiSizeIdx,
                      int *outNewGUIWidth, int *outNewGUIHeight) override;

    // Used for disabling monitor button in the right thread.
    void OnIdle() override;

    void OnMouseOver(float x, float y, const IMouseMod &mod) override;
    void OnMouseDrag(float x, float y, float dX, float dY,
                     const IMouseMod &mod) override;
    void OnMouseOut() override;
        
 protected:
    IGraphics *MyMakeGraphics();
    void MyMakeLayout(IGraphics *pGraphics);

    void CreateControls(IGraphics *pGraphics, int offset);
    
    void InitNull();
    void InitParams();
    void ApplyParams();
    
    void Init(int oversampling, int freqRes);
    void SetColorMap(int colorMapNum);
    void SetScrollSpeed(int scrollSpeedNum);
    void UpdateSpectrogramData();
    
    void GUIResizeParamChange(int guiSizeIdx);
    void GetNewGUISize(int guiSizeIdx, int *width, int *height);
    
    // Time axis
    void UpdateTimeAxis();
    
    void CreateSpectrogramDisplay(bool createFromInit);
    void CreateGraphAxes();

    // For SpectroMeter
    BL_FLOAT GraphNormXToTime(BL_FLOAT normX);
    BL_FLOAT GraphNormYToFreq(BL_FLOAT normY);

    void CursorMoved(BL_FLOAT x, BL_FLOAT y);

    void UpdatePrevMouse(float newGUIWidth, float newGUIHeight);
        
    //
    GraphControl12 *mGraph;
    
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    FftProcessObj16 *mDispFftObj;
    GhostViewerFftObj *mGhostViewerObj;
    
    BLSpectrogram4 *mSpectrogram;
    SpectrogramDisplayScroll4 *mSpectrogramDisplay;
    SpectrogramDisplayScroll4::SpectrogramDisplayScrollState *
        mSpectroDisplayScrollState;
    
    bool mNeedRecomputeData;
    
    bool mMustUpdateSpectrogram;
    
    // Graph size (when GUI is small)
    int mGraphWidthSmall;
    int mGraphHeightSmall;
    
    BL_FLOAT mPrevSampleRate;
    
    // "Hack" for avoiding infinite loop + restoring correct size at initialization
    //
    // NOTE: imported from Waves
    //
    IGUIResizeButtonControl *mGUISizeSmallButton;
    IGUIResizeButtonControl *mGUISizeMediumButton;
    IGUIResizeButtonControl *mGUISizeBigButton;
    
    IGUIResizeButtonControl *mGUISizePortraitButton;
    
    BL_FLOAT mRange;
    BL_FLOAT mContrast;
    int mColorMapNum;
    int mScrollSpeedNum;

    int mGUISizeIdx;
    
    GUIHelper12 *mGUIHelper;
    
    GraphAxis2 *mHAxis;
    GraphTimeAxis6 *mTimeAxis;
    bool mMustUpdateTimeAxis;
    
    GraphAxis2 *mVAxis;
    GraphFreqAxis2 *mFreqAxis;
    
    IBLSwitchControl *mMonitorControl;
    
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;
    
    // For GUI resize
    int mGUIOffsetX;
    int mGUIOffsetY;
    
    bool mMonitorEnabled;
    bool mWasPlaying;

    bool mIsPlaying;

    PlugBypassDetector *mBypassDetector;
    BLTransport *mTransport;

    SpectroMeter *mSpectroMeter;
    SpectroMeter::TimeMode mMeterTimeMode;
    SpectroMeter::FreqMode mMeterFreqMode;
    BL_FLOAT mPrevMouseX;
    BL_FLOAT mPrevMouseY;

    //bool mFirstTimeCreate;
    bool mMustSetScrollSpeed;
    int mPrevScrollSpeedNum;

    BLUtilsPlug mBLUtilsPlug;
    
 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf3;
        
    BL_PROFILE_DECLARE;
};

#endif
