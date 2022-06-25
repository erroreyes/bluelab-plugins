//
//  MorphoFrameAnalyzerFftObj.h
//  BL-Waves
//
//  Created by Pan on 20/04/18.
//
//

#ifndef __MorphoFrameAnalyzerFftObj__
#define __MorphoFrameAnalyzerFftObj__

#include <FftProcessObj16.h>

#include <Partial2.h>

#include <MorphoFrame7.h>

#include <BlaTimer.h>

class PartialTracker8;
class MorphoFrameAna2;
class MorphoFrameAnalyzerFftObj : public ProcessObj
{
public:
    MorphoFrameAnalyzerFftObj(int bufferSize,
                           BL_FLOAT overlapping, BL_FLOAT oversampling,
                           BL_FLOAT sampleRate,
                           bool storeDetectDataInFrames);
    
    virtual ~MorphoFrameAnalyzerFftObj();
    
    void Reset(BL_FLOAT sampleRate);
    
    void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                          const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer) override;

    void GetMorphoFrames(vector<MorphoFrame7> *frames);
    
    // Analysis parameters
    void SetThreshold(BL_FLOAT threshold);
    void SetThreshold2(BL_FLOAT threshold2);
    
    void SetTimeSmoothCoeff(BL_FLOAT coeff);
    void SetTimeSmoothNoiseCoeff(BL_FLOAT coeff);
    void SetNeriDelta(BL_FLOAT delta);
        
protected:
    void DenormPartials(vector<Partial2> *partials);

    //
    bool mStoreDetectDataInFrames;
    
    WDL_TypedBuf<BL_FLOAT> mCurrentMagns;
    // Not filtered
    vector<Partial2> mCurrentRawPartials;
    // Filtered
    vector<Partial2> mCurrentNormPartials;
    
    PartialTracker8 *mPartialTracker;
    
    MorphoFrameAna2 *mMorphoFrameAna;
    
    // Current Morpho frame
    vector<MorphoFrame7> mCurrentMorphoFrames;

private:
    WDL_TypedBuf<WDL_FFT_COMPLEX> mTmpBuf0;
    WDL_TypedBuf<WDL_FFT_COMPLEX> mTmpBuf1;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf2;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf3;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf4;
    vector<Partial2> mTmpBuf5;
    MorphoFrame7 mTmpBuf6;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf7;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf8;
};

#endif
