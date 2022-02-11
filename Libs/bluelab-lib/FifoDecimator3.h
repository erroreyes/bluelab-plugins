//
//  FifoDecimator3.h
//  BL-TransientShaper
//
//  Created by Pan on 11/04/18.
//
//

#ifndef __BL_TransientShaper__FifoDecimator3__
#define __BL_TransientShaper__FifoDecimator3__

#include <BLTypes.h>

#include "../../WDL/fastqueue.h"

#include "IPlug_include_in_plug_hdr.h"

// Keep many values, and decimate when GetValues()
class FifoDecimator3
{
public:
    // If we want to process samples (waveform, set isSamples to true)
    FifoDecimator3(long maxSize, BL_FLOAT decimFactor, bool isSamples);
    
    FifoDecimator3(bool isSamples);
    
    virtual ~FifoDecimator3();
    
    void Reset();
    
    void SetParams(long maxSize, BL_FLOAT decimFactor);
    void SetParams(long maxSize, BL_FLOAT decimFactor, bool isSamples);
    
    void AddValues(const WDL_TypedBuf<BL_FLOAT> &samples);
    
    void GetValues(WDL_TypedBuf<BL_FLOAT> *values);
    
protected:
    long mMaxSize;
    BL_FLOAT mDecimFactor;
    bool mIsSamples;
    
    //WDL_TypedBuf<BL_FLOAT> mValues;
    WDL_TypedFastQueue<BL_FLOAT> mValues;

private:
    // Tmp buffers
    WDL_TypedBuf<BL_FLOAT> mTmpBuf0;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf1;
};

#endif /* defined(__BL_TransientShaper__FifoDecimator3__) */
