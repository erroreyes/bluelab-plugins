//
//  RebalanceDumpFftObj2.hpp
//  BL-Rebalance-macOS
//
//  Created by applematuer on 10/14/20.
//
//

#ifndef RebalanceDumpFftObj2_hpp
#define RebalanceDumpFftObj2_hpp

#include <deque>
using namespace std;

#include <FftProcessObj16.h>

#include <Rebalance_defs.h>

// RebalanceDumpFftObj2: from RebalanceDumpFftObj
// for ResampProcessObj
class MelScale;
class RebalanceDumpFftObj2 : public MultichannelProcess
{
public:
    RebalanceDumpFftObj2(int bufferSize, BL_FLOAT sampleRate, int numInputCols);
    
    virtual ~RebalanceDumpFftObj2();
    
    void ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
                         const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer) override;
    
    bool HasEnoughData();
    void GetData(WDL_TypedBuf<BL_FLOAT> cols[REBALANCE_NUM_SPECTRO_COLS]);
    
protected:
    int mNumInputCols;
    
    deque<WDL_TypedBuf<BL_FLOAT> > mCols;
    
    BL_FLOAT mSampleRate;
    
    MelScale *mMelScale;
};

#endif /* RebalanceDumpFftObj2_hpp */
