#ifndef __BAT__
#define __BAT__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <SpectrogramDisplay2.h>

#include <BLUtils.h>

#include <ResizeGUIPluginInterface.h>

using namespace iplug;
using namespace igraphics;

class FftProcessObj16;
class BatFftObj5;
class GraphControl12;
class IGUIResizeButtonControl;
class GUIHelper12;
class BLSpectrogram4;
class BLImage;
class ImageDisplay2;

// New version: re-create the graph each time we close/open the windows, or resize gui
// and keep the data in other objects, not in the graph.
class Bat final : public Plugin,
    public ResizeGUIPluginInterface
{
 public:
    Bat(const InstanceInfo &info);
    virtual ~Bat();
  
    void OnReset() override;
    void OnParamChange(int paramIdx) override;

    void OnUIOpen() override;
    void OnUIClose() override;
    
    void ProcessBlock(iplug::sample **inputs,
                      iplug::sample **outputs, int nFrames) override;
  
    void PreResizeGUI(int guiSizeIdx,
                      int *outNewGUIWidth, int *outNewGUIHeight) override;
    
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

    void CreateSpectrogramDisplay(bool createFromInit);
    void CreateImageDisplay(bool createFromInit);    
    
    //
    GraphControl12 *mGraph;
    
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    FftProcessObj16 *mFftObj;
    BatFftObj5 *mBatObj;
    
    BLSpectrogram4 *mSpectrogram;
    SpectrogramDisplay2 *mSpectrogramDisplay;
    SpectrogramDisplay2::SpectrogramDisplayState *mSpectrogramDisplayState;
    
    BLImage *mImage;
    ImageDisplay2 *mImageDisplay;
    
    BL_FLOAT mRange;
    BL_FLOAT mContrast;
    int mColorMapNum;
    
    BL_FLOAT mSharpness;
    BL_FLOAT mSmooth;
    BL_FLOAT mIntermicCoeff;
    BL_FLOAT mSmoothData;
    
    //
    bool mNeedRecomputeData;
    bool mMustUpdateSpectrogram;
    
    //
    BL_FLOAT mPrevSampleRate;
    
    // Graph size (when GUI is small)
    int mGraphWidthSmall;
    int mGraphHeightSmall;
    
    // "Hack" for avoiding infinite loop + restoring correct size at initialization
    //
    // NOTE: imported from Waves
    //
    IGUIResizeButtonControl *mGUISizeSmallButton;
    IGUIResizeButtonControl *mGUISizeMediumButton;
    IGUIResizeButtonControl *mGUISizeBigButton;
    
    int mGUISizeIdx;
    
    GUIHelper12 *mGUIHelper;
    
    
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;
    
    // For GUI resize
    int mGUIOffsetX;
    int mGUIOffsetY;
    
    bool mPrevPlaying;

 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    
    BL_PROFILE_DECLARE;
};

#endif
