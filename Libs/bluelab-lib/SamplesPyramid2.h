//
//  SamplesPyramid2.h
//  BL-Ghost
//
//  Created by applematuer on 9/30/18.
//
//

#ifndef __BL_Ghost__SamplesPyramid2__
#define __BL_Ghost__SamplesPyramid2__

#include <vector>
using namespace std;

#include <BLTypes.h>

#include "../../WDL/fastqueue.h"
#include "IPlug_include_in_plug_hdr.h"

// SamplesPyramid2: for UST
// Try to avoid glitches

// Test for UST
// => BETTER: gritches less
#define FIX_GLITCH 1

// Make "MipMaps" with samples
// (to optimize when displaying waveform of long sound)
class SamplesPyramid2
{
public:
    SamplesPyramid2();
    
    virtual ~SamplesPyramid2();
    
    void Reset();
    
    void SetValues(const WDL_TypedBuf<BL_FLOAT> &samples);
    
    void PushValues(const WDL_TypedBuf<BL_FLOAT> &samples);
    
    void PopValues(long numSamples);
    
    void ReplaceValues(long start, const WDL_TypedBuf<BL_FLOAT> &samples);
    
#if !FIX_GLITCH
    void GetValues(long start, long end, long numValues,
                   WDL_TypedBuf<BL_FLOAT> *samples);
#else
    void GetValues(BL_FLOAT start, BL_FLOAT end, long numValues,
                   WDL_TypedBuf<BL_FLOAT> *samples);
#endif
    
protected:
    void ResetTmpBuffers();
    
    //vector<WDL_TypedBuf<BL_FLOAT> > mSamplesPyramid;
    vector<WDL_TypedFastQueue<BL_FLOAT> > mSamplesPyramid;
    
    // Keep a push buffer, to be able to push
    // by bloocks of power of two (otherwise,
    // there are artefacts on the second half).
    //WDL_TypedBuf<BL_FLOAT> mPushBuf;
    WDL_TypedFastQueue<BL_FLOAT> mPushBuf;
    
    long mRemainToPop;

private:
    // Tmp buffers
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf1;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf2;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf3;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf4;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf5;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf6;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf7;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf8;
};

#endif /* defined(__BL_Ghost__SamplesPyramid2__) */
