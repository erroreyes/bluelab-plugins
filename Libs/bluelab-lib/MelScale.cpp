//
//  MelScale.cpp
//  BL-Rebalance-macOS
//
//  Created by applematuer on 10/15/20.
//
//

#include <cmath>

extern "C" {
#include <libmfcc.h>
}

#include <BLUtils.h>

#include "MelScale.h"

// See: http://practicalcryptography.com/miscellaneous/machine-learning/guide-mel-frequency-cepstral-coefficients-mfccs/

BL_FLOAT
MelScale::HzToMel(BL_FLOAT freq)
{
    BL_FLOAT mel = 2595.0*std::log10((BL_FLOAT)(1.0 + freq/700.0));
    
    return mel;
}

BL_FLOAT
MelScale::MelToHz(BL_FLOAT mel)
{
    BL_FLOAT hz = 700.0*(std::pow((BL_FLOAT)10.0, (BL_FLOAT)(mel/2595.0)) - 1.0);
    
    return hz;
}

void
MelScale::HzToMel(WDL_TypedBuf<BL_FLOAT> *resultMagns,
                  const WDL_TypedBuf<BL_FLOAT> &magns,
                  BL_FLOAT sampleRate)
{
    // For dB
    resultMagns->Resize(magns.GetSize());
    BLUtils::FillAllZero(resultMagns);
    
    BL_FLOAT maxFreq = sampleRate*0.5;
    if (maxFreq < BL_EPS)
        return;
    
    BL_FLOAT maxMel = HzToMel(maxFreq);
    
    BL_FLOAT melCoeff = maxMel/resultMagns->GetSize();
    BL_FLOAT idCoeff = (1.0/maxFreq)*resultMagns->GetSize();
    
    int resultMagnsSize = resultMagns->GetSize();
    BL_FLOAT *resultMagnsData = resultMagns->Get();
    
    int magnsSize = magns.GetSize();
    BL_FLOAT *magnsData = magns.Get();
    for (int i = 0; i < resultMagnsSize; i++)
    {
        BL_FLOAT mel = i*melCoeff;
        BL_FLOAT freq = MelToHz(mel);
        
        BL_FLOAT id0 = freq*idCoeff;
        
        int id0i = (int)id0;
        
        BL_FLOAT t = id0 - id0i;
        
        if (id0i >= magnsSize)
            continue;
        
        // NOTE: this optim doesn't compute exactly the same thing than the original version
        int id1 = id0i + 1;
        if (id1 >= magnsSize)
            continue;
        
        BL_FLOAT magn0 = magnsData[id0i];
        BL_FLOAT magn1 = magnsData[id1];
        
        BL_FLOAT magn = (1.0 - t)*magn0 + t*magn1;
        
        resultMagnsData[i] = magn;
    }
}


void
MelScale::MelToHz(WDL_TypedBuf<BL_FLOAT> *resultMagns,
                  const WDL_TypedBuf<BL_FLOAT> &magns,
                  BL_FLOAT sampleRate)
{
    resultMagns->Resize(magns.GetSize());
    BLUtils::FillAllZero(resultMagns);
    
    BL_FLOAT hzPerBin = sampleRate*0.5/magns.GetSize();
    
    BL_FLOAT maxFreq = sampleRate*0.5;
    BL_FLOAT maxMel = HzToMel(maxFreq);
    
    int resultMagnsSize = resultMagns->GetSize();
    BL_FLOAT *resultMagnsData = resultMagns->Get();
    int magnsSize = magns.GetSize();
    BL_FLOAT *magnsData = magns.Get();
    
    for (int i = 0; i < resultMagnsSize; i++)
    {
        BL_FLOAT freq = hzPerBin*i;
        BL_FLOAT mel = HzToMel(freq);
        
        BL_FLOAT id0 = (mel/maxMel) * resultMagnsSize;
        
        if ((int)id0 >= magnsSize)
            continue;
        
        BL_FLOAT t = id0 - (int)(id0);
        
        int id1 = id0 + 1;
        if (id1 >= magnsSize)
            continue;
        
        BL_FLOAT magn0 = magnsData[(int)id0];
        BL_FLOAT magn1 = magnsData[id1];
        
        BL_FLOAT magn = (1.0 - t)*magn0 + t*magn1;
        
        resultMagnsData[i] = magn;
    }
}

void
MelScale::HzToMelMfcc(WDL_TypedBuf<BL_FLOAT> *result,
                      const WDL_TypedBuf<BL_FLOAT> &magns,
                      BL_FLOAT sampleRate, int numMelBins)
{
    result->Resize(numMelBins);
    BLUtils::FillAllZero(result);
    
    int numFilters = 64; //30; //256; //60; //30; //numMelBins;
    int numCoeffs = numMelBins;
    
    BL_FLOAT *spectrum = magns.Get();
    int spectralDataArraySize = magns.GetSize();
    
    /*WDL_TypedBuf<BL_FLOAT> normMagns = magns;
    BL_FLOAT coeff = sqrt(normMagns.GetSize());
    BLUtils::MultValues(&normMagns, coeff);
    BL_FLOAT *spectrum = normMagns.Get();*/
    
    /*WDL_TypedBuf<BL_FLOAT> fftMagns = magns;
    fftMagns.Resize(fftMagns.GetSize()*2);
    BLUtils::FillSecondFftHalf(&fftMagns);
    BL_FLOAT *spectrum = fftMagns.Get();
    int spectralDataArraySize = fftMagns.GetSize();*/
    
    for (int coeff = 0; coeff < numCoeffs; coeff++)
    {
        BL_FLOAT res = GetCoefficient(spectrum, (int)sampleRate,
                                      numFilters, spectralDataArraySize, coeff);
        
        result->Get()[coeff] = res;
    }
}
