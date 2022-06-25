#ifndef __DUET__
#define __DUET__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SpectrogramDisplay2.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtils.h>

#include <ResizeGUIPluginInterface.h>

using namespace iplug;
using namespace igraphics;

class FftProcessObj16;
class DUETFftObj2;
class GraphControl12;
class IGUIResizeButtonControl;
class GUIHelper12;
class DUETCustomControl;
class DUETCustomDrawer;
class BLSpectrogram4;
//class SpectrogramDisplay2;
class BLImage;
class ImageDisplay2;

// New version: re-create the graph each time we close/open the windows, or resize gui
// and keep the data in other objects, not in the graph.
class DUET final : public Plugin,
    public ResizeGUIPluginInterface,
    public DUETPlugInterface
{
 public:
    DUET(const InstanceInfo &info);
    virtual ~DUET();
  
    void OnReset() override;
    void OnParamChange(int paramIdx) override;

    void OnUIOpen() override;
    void OnUIClose() override;
    
    void ProcessBlock(iplug::sample **inputs,
                      iplug::sample **outputs, int nFrames) override;
  
    void PreResizeGUI(int guiSizeIdx,
                      int *outNewGUIWidth, int *outNewGUIHeight) override;

    // Callbacks for GUI
    void SetPickCursorActive(bool flag) override;
    void SetPickCursor(int x, int y) override;
    void SetInvertPickSelection(bool flag) override;
  
 protected:
    IGraphics *MyMakeGraphics();
    void MyMakeLayout(IGraphics *pGraphics);

    void CreateControls(IGraphics *pGraphics, int offset);
    
    void InitNull();
    void InitParams();
    void ApplyParams();
    
    void Init(int oversampling, int freqRes);
    void SetColorMap(int colorMapNum);
    
    void UpdateSpectrogramData();
    
    void GUIResizeParamChange(int guiSizeIdx);
    void GetNewGUISize(int guiSizeIdx, int *width, int *height);
    
    // Time axis
    void CreateSpectrogramDisplay(bool createFromInit);
    void CreateImageDisplay(bool createFromInit);
    
    //
    GraphControl12 *mGraph;
    
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    FftProcessObj16 *mFftObj;
    DUETFftObj2 *mDUETObj;
    
    BLSpectrogram4 *mSpectrogram;
    SpectrogramDisplay2 *mSpectrogramDisplay;
    SpectrogramDisplay2::SpectrogramDisplayState *mSpectrogramState;
    
    BLImage *mImage;
    ImageDisplay2 *mImageDisplay;
    
    DUETCustomControl *mGraphControl;
    DUETCustomDrawer *mGraphDrawer;
  
    //
    bool mNeedRecomputeData;
    
    bool mMustUpdateSpectrogram;
    
    // Graph size (when GUI is small)
    int mGraphWidthSmall;
    int mGraphHeightSmall;
    
    //
    BL_FLOAT mPrevSampleRate;
    
    bool mPrevPlaying;
    
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

    int mGUISizeIdx;

    BL_FLOAT mSmooth;
    BL_FLOAT mThresholdFloor;
    BL_FLOAT mThresholdPeaks;
    BL_FLOAT mThresholdPeaksWidth;
    bool mUseKernelSmooth;
    bool mDispThreshold;
    bool mDispMax;
    bool mDispMasks;
    bool mUseSoftMasks;
    bool mUseSoftMasksComp;
    int mSoftMasksSize;
    bool mUseGradientMasks;
    bool mThresholdAll;
    int mHistoSize;
    BL_FLOAT mAlphaZoom;
    BL_FLOAT mDeltaZoom;
    bool mUsePhaseAliasingCorrection;
    
    GUIHelper12 *mGUIHelper;
    
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;
    
    // For GUI resize
    int mGUIOffsetX;
    int mGUIOffsetY;

    bool mParamChanged;

 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    
    BL_PROFILE_DECLARE;
};

#endif
