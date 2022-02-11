//
//  FilterRBJ2X2.cpp
//  UST
//
//  Created by applematuer on 8/25/19.
//
//

#include <BLUtils.h>

#include "FilterRBJ2X2.h"


FilterRBJ2X2::FilterRBJ2X2(int type,
                         BL_FLOAT sampleRate,
                         BL_FLOAT cutoffFreq)
{
    mType = type;
    mSampleRate = sampleRate;
    mCutoffFreq = cutoffFreq;
    
    mQFactor = 0.707;
    
    mFilter = new CFxRbjFilter2X();
    
    CalcFilterCoeffs();
}

FilterRBJ2X2::FilterRBJ2X2(const FilterRBJ2X2 &other)
{
    mType = other.mType;
    mSampleRate = other.mSampleRate;
    mCutoffFreq = other.mCutoffFreq;
    
    mFilter = new CFxRbjFilter2X();
    
    CalcFilterCoeffs();
}

FilterRBJ2X2::~FilterRBJ2X2()
{
    delete mFilter;
}

void
FilterRBJ2X2::SetCutoffFreq(BL_FLOAT freq)
{
    mCutoffFreq = freq;
    
    CalcFilterCoeffs();
}

void
FilterRBJ2X2::SetSampleRate(BL_FLOAT sampleRate)
{
    mSampleRate = sampleRate;
    
    CalcFilterCoeffs();
}

void
FilterRBJ2X2::SetQFactor(BL_FLOAT q)
{
    mQFactor = q;
    
    CalcFilterCoeffs();
}

BL_FLOAT
FilterRBJ2X2::Process(BL_FLOAT sample)
{
    //FIX_FLT_DENORMAL(sample);
    
    sample = mFilter->filter(sample);
    
    //FIX_FLT_DENORMAL(sample)
    
    return sample;
}

void
FilterRBJ2X2::CalcFilterCoeffs()
{
    // For flat crossover
    //BL_FLOAT QFactor = 0.707;
    BL_FLOAT QFactor = mQFactor;
    
    BL_FLOAT dbGain = 0.0;
    bool qIsBandwidth = false;
    
    mFilter->calc_filter_coeffs(mType, mCutoffFreq, mSampleRate,
                                QFactor, dbGain, qIsBandwidth);
}

#if 0
void
FilterRBJ2X2::Process(WDL_TypedBuf<BL_FLOAT> *result,
                         const WDL_TypedBuf<BL_FLOAT> &samples)
{
    result->Resize(samples.GetSize());
    
    for (int i = 0; i < samples.GetSize(); i++)
    {
        BL_FLOAT sample = samples.Get()[i];
        
        BL_FLOAT sampleRes = Process(sample);
        
        result->Get()[i] = sampleRes;
    }
}
#endif

#if 0 //1
void
FilterRBJ2X2::Process(WDL_TypedBuf<BL_FLOAT> *ioSamples)
{
    for (int i = 0; i < ioSamples->GetSize(); i++)
    {
        BL_FLOAT sample = ioSamples->Get()[i];
        
        BL_FLOAT sampleRes = Process(sample);
        
        ioSamples->Get()[i] = sampleRes;
    }
}
#endif

#if 1
void
FilterRBJ2X2::Process(WDL_TypedBuf<BL_FLOAT> *ioSamples)
{
    //FIX_FLT_DENORMAL(sample);
    
    mFilter->filter(ioSamples->Get(), ioSamples->GetSize());
    
    //FIX_FLT_DENORMAL(sample)
}
#endif
