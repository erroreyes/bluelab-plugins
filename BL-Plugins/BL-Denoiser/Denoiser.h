#ifndef __DENOISER__
#define __DENOISER__

#include <vector>
using namespace std;

#include "IPlug_include_in_plug_hdr.h"
#include "IGraphics_include_in_plug_hdr.h"

#include <BLProfiler.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#include <BLUtilsPlug.h>

using namespace iplug;
using namespace igraphics;


class FftProcessObj16;
class DenoiserObj;
class PostTransientFftObj3;
class GraphControl12;
class GraphFreqAxis2;
class GraphAmpAxis;
class GraphAxis2;
class GUIHelper12;
class SmoothCurveDB;

class Denoiser final : public Plugin
{
public:
    Denoiser(const InstanceInfo &info);
    virtual ~Denoiser();

    void OnHostIdentified() override;
    
    void OnReset() override;
    void OnParamChange(int paramIdx) override;

    void OnUIOpen() override;
    void OnUIClose() override;
    
    void ProcessBlock(iplug::sample **inputs,
                      iplug::sample **outputs, int nFrames) override;
    
    bool SerializeState(IByteChunk& pChunk) const override;
    int UnserializeState(const IByteChunk& pChunk, int startPos) override;

    void OnIdle() override;
    
protected:
    IGraphics *MyMakeGraphics();
    void MyMakeLayout(IGraphics *pGraphics);

    void CreateControls(IGraphics *pGraphics);
    
    void InitNull();
    void InitParams();
    void ApplyParams();
    
    void Init(int oversampling, int freqRes);
    
    void CreateGraphAxes();
    void CreateGraphCurves();

    void UpdateGraph(WDL_TypedBuf<BL_FLOAT> *signal,
                     WDL_TypedBuf<BL_FLOAT> *noise);
    
    // Apply attenuation is a trick.
    // It consists in adding residual noise, to mask metallic frequencies
    // (psychoacoustic principle of masking)
    void ApplyAttenuation(BL_FLOAT *buf, BL_FLOAT *residue, int nFrames, BL_FLOAT attenuation);
    
    BL_FLOAT ComputeRMSAvg(const BL_FLOAT *output, int nFrames);

    void UpdateGraphNoisePattern(WDL_TypedBuf<BL_FLOAT> *noiseStats);
  
    // Compute mono and update
    void UpdateGraphNoisePattern();
    
    void QualityChanged();
    
    void CreateAvgHistograms();

    void UpdateLatency();
    
    // Secure starters
    SecureRestarter mSecureRestarter;
    
    PostTransientFftObj3 *mFftObj;
    vector<DenoiserObj *> mDenoiserObjs;
    
    GraphControl12 *mGraph;
    
    GraphAmpAxis *mAmpAxis;
    GraphAxis2 *mHAxis;
    
    GraphFreqAxis2 *mFreqAxis;
    GraphAxis2 *mVAxis;

    GraphCurve5 *mSignalCurve;
    SmoothCurveDB *mSignalCurveSmooth;
    
    GraphCurve5 *mNoiseCurve;
    SmoothCurveDB *mNoiseCurveSmooth;
    
    GraphCurve5 *mNoiseProfileCurve;
    SmoothCurveDB *mNoiseProfileCurveSmooth;
    
    //
    enum Quality
    {
        STANDARD = 0,
        HIGH,
        VERY_HIGH,
        OFFLINE
    };
    
    enum Quality mQuality;
    bool mQualityChanged;
    bool mLearnFlag;
    
    int mOverlapping;
    int mFreqRes;
    
    BL_FLOAT mThreshold;
    
    // Inverse of residue level
    BL_FLOAT mRatio;
  
    bool mOutputNoiseOnly;
  
    BL_FLOAT mTransBoost;

    BL_FLOAT mResNoiseThrs;
    
    // For sample rate change
    BL_FLOAT mPrevSampleRate;
    
    // For gray out
    IControl *mResNoiseKnob;
    
    bool mAutoResNoise;
  
    GUIHelper12 *mGUIHelper;
    
    DemoModeManager mDemoManager;
    
    bool mUIOpened;
    bool mControlsCreated;

    bool mIsInitialized;

    BLUtilsPlug mBLUtilsPlug;
    
private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf3;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf4;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf5[2];
    WDL_TypedBuf<BL_FLOAT> mTmpBuf6[2];
    WDL_TypedBuf<BL_FLOAT> mTmpBuf7;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf8;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf9;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf10;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf11;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf12;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf13;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf14;
    
    BL_PROFILE_DECLARE;
};

#endif
