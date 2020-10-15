//
//  RebalanceDumpFftObj2.cpp
//  BL-Rebalance-macOS
//
//  Created by applematuer on 10/14/20.
//
//

#include <BLUtils.h>

#include "RebalanceDumpFftObj2.h"

RebalanceDumpFftObj2::RebalanceDumpFftObj2(int bufferSize, int numInputCols)
: MultichannelProcess()
{
    mNumInputCols = numInputCols;
    
    // Fill with zeros at the beginning
    mCols.resize(mNumInputCols);
    for (int i = 0; i < mCols.size(); i++)
    {
        BLUtils::ResizeFillZeros(&mCols[i], bufferSize/2);
    }
}

RebalanceDumpFftObj2::~RebalanceDumpFftObj2() {}

void
RebalanceDumpFftObj2::ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
                                      const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer)
{
    // Stereo to mono
    vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > monoFftSamples;
    for (int i = 0; i < ioFftSamples->size(); i++)
    {
        monoFftSamples.push_back(*(*ioFftSamples)[i]);
    }
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> dataBuffer;
    BLUtils::StereoToMono(&dataBuffer, monoFftSamples);
    
    //
    BLUtils::TakeHalf(&dataBuffer);
    
    WDL_TypedBuf<BL_FLOAT> magnsMix;
    BLUtils::ComplexToMagn(&magnsMix, dataBuffer);
    
    mCols.push_back(magnsMix);
}

bool
RebalanceDumpFftObj2::HasEnoughData()
{
    bool hasEnoughData = (mCols.size() >= mNumInputCols);
    
    return hasEnoughData;
}

void
RebalanceDumpFftObj2::GetData(WDL_TypedBuf<BL_FLOAT> cols[REBALANCE_NUM_SPECTRO_COLS])
{
    for (int i = 0; i < mNumInputCols; i++)
    {
        cols[i] = mCols[i];
    }
    
    for (int i = 0; i < mNumInputCols; i++)
    {
        mCols.pop_front();
    }
}
