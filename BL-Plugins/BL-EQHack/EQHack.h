#ifndef __EQHACK__
#define __EQHACK__

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <EQHackPluginInterface.h>

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtilsPlug.h>

using namespace iplug;
using namespace igraphics;


class FftProcessObj16;
class EQHackFftObj2;
class FftProcessBufObj;
class GraphControl12;
class GraphFreqAxis2;
class GraphAxis2;
class GUIHelper12;
class GraphCurve5;
class SmoothCurveDB;
class GraphAmpAxis;
class CMA2Smoother;
class IBLSwitchControl;
class EQHack final : public Plugin
{
 public:
    EQHack(const InstanceInfo &info);
    virtual ~EQHack();

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
    void ApplyParams();
    
    void Init(int oversampling, int freqRes);

    bool SerializeState(IByteChunk &pChunk) const override;
    int UnserializeState(const IByteChunk &pChunk, int startPos) override;
    
    void CreateGraphAxes();
    void CreateGraphCurves();
    
    void UpdateCurves(const WDL_TypedBuf<BL_GUI_FLOAT> *currentEQ,
                      bool updateLearnCurve);

    // Set to true if called from unserialization
    void ModeChanged(bool fromUnserial = false);
    void SetCurveSpecsModeLearn();

    // Unused
    void SetCurveSpecsModeApply();
    void SetCurveSpecsModeApplyInv();

    void SetCurveSpecsModeGuess();

    void CMASmooth(int smootherIndex, WDL_TypedBuf<BL_GUI_FLOAT> *buf);

    void ResetLearnCurve();

    void UpdateCurvesSmoothFactor();
        
    //
    EQHackPluginInterface::Mode mMode;
    
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    FftProcessObj16 *mFftObj;
    EQHackFftObj2 *mEQHackObj;

    // Keep trace of the smoothed mAvgHistoAvg
    WDL_TypedBuf<BL_GUI_FLOAT> mLearnCurve;

    BL_FLOAT mPrevSampleRate;
    
    GraphControl12 *mGraph;
    
    GraphFreqAxis2 *mFreqAxis;
    GraphAxis2 *mHAxis;

    // Amp axis without labels, just to have the axis grid
    GraphAmpAxis *mAmpAxis;
    GraphAxis2 *mVAxis;
    
    GraphCurve5 *mEQLearnCurve;
    SmoothCurveDB *mEQLearnCurveSmooth;
    
    GraphCurve5 *mEQInstantCurve;
    SmoothCurveDB *mEQInstantCurveSmooth;
    
    GraphCurve5 *mEQGuessCurve;
    SmoothCurveDB *mEQGuessCurveSmooth;
    
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;

    GUIHelper12 *mGUIHelper;

    CMA2Smoother *mSmoothers[2];

    IBLSwitchControl *mMonitorControl;
    bool mMonitorEnabled;
    bool mIsPlaying;

    // For LockFree
    bool mMustUpdateCurvesLF;
    bool mUpdateLearnCurveLF;
    WDL_TypedBuf<BL_FLOAT> mUpdateCurveBufLF;

    BLUtilsPlug mBLUtilsPlug;
    
 private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf3;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf4;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf5;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf6;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf7;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf8;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf9;
    vector< WDL_TypedBuf<BL_FLOAT> > mTmpBuf10;
    vector< WDL_TypedBuf<BL_FLOAT> > mTmpBuf11;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf12;
    
    BL_PROFILE_DECLARE;
};

#endif
