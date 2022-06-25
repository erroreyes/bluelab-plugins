#ifndef __SPECTRUM_VIEW__
#define __SPECTRUM_VIEW__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <ResizeGUIPluginInterface.h>

using namespace iplug;
using namespace igraphics;


class FftProcessObj16;
class SpectrumViewFftObj;
class GraphControl12;
class GraphFreqAxis2;
class GraphAmpAxis;
class GraphAxis2;
class GUIHelper12;
class SmoothCurveDB;
class IGUIResizeButtonControl;
class SpectrumView final : public Plugin,
    public ResizeGUIPluginInterface
{
 public:
    SpectrumView(const InstanceInfo &info);
    virtual ~SpectrumView();
  
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
    
    void CreateGraphAxes();
    void CreateGraphCurve();
    
    void UpdateCurve(const WDL_TypedBuf<BL_FLOAT> &fftSignal);

    void PrecisionChanged();

    void GUIResizeParamChange(int guiSizeIdx);
    void GetNewGUISize(int guiSizeIdx, int *width, int *height);
    
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    FftProcessObj16 *mFftObj;
    SpectrumViewFftObj *mSpectrumViewObj;

    bool mFreeze;
    IControl *mFreezeControl;
    
    // FIX: when freezed, if resize GUI, the curve becomes badly scaled
    WDL_TypedBuf<BL_FLOAT> mLastSignal;

    int mPrecision;
    int mBufferSize;
    bool mPrecisionChanged;
  
    GraphControl12 *mGraph;
    
    GraphAmpAxis *mAmpAxis;
    GraphAxis2 *mHAxis;
    
    GraphFreqAxis2 *mFreqAxis;
    GraphAxis2 *mVAxis;
    
    //
    GraphCurve5 *mSpectrumCurve;
    SmoothCurveDB *mSpectrumCurveSmooth;

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

    // For GUI resize
    int mGUIOffsetX;
    int mGUIOffsetY;
    
    //
    GUIHelper12 *mGUIHelper;
    
    //
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;

 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf3;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf4;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf5;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf6;
    
    BL_PROFILE_DECLARE;
};

#endif
