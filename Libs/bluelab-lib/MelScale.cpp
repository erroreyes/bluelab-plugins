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

extern "C" {
#include <fast-dct-lee.h>
}

#include <BLUtils.h>
#include <BLDebug.h>

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

#if 0
void
MelScale::HzToMelMfcc(WDL_TypedBuf<BL_FLOAT> *result,
                      const WDL_TypedBuf<BL_FLOAT> &magns,
                      BL_FLOAT sampleRate, int numMelBins)
{
    result->Resize(numMelBins);
    BLUtils::FillAllZero(result);
    
    int numFilters = numMelBins; //64; //30; //256; //60; //30; //numMelBins;
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
    
    WDL_TypedBuf<double> tmpDebugData;
    tmpDebugData.Resize(magns.GetSize());
    
    for (int coeff = 0; coeff < numCoeffs; coeff++)
    {
        BL_FLOAT res = BLGetCoefficient(spectrum, (int)sampleRate,
                                        numFilters, spectralDataArraySize, coeff,
                                        tmpDebugData.Get());
        
        result->Get()[coeff] = res;
        
        if (coeff == 0)
            BLDebug::DumpData("filters.txt", tmpDebugData);
        else
        {
            BLDebug::AppendNewline("filters.txt");
            BLDebug::AppendData("filters.txt", tmpDebugData);
        }
    }
    
#if 0
    // Now, to have mel from mfcc, we need to do idct(log(mfcc))
    // (or dct(log(mfcc), not sure...)
    //
    // See: https://stackoverflow.com/questions/53925401/difference-between-mel-spectrogram-and-an-mfcc/54326385#54326385
    for (int i = 0; i < result->GetSize(); i++)
    {
        BL_FLOAT val = result->Get()[i];
        val = log(val + 1.0);
        result->Get()[i] = val;
    }
    
    FastDctLee_inverseTransform(result->Get(), result->GetSize());
#endif
}
#endif

// See: https://haythamfayek.com/2016/04/21/speech-processing-for-machine-learning.html
void
MelScale::HzToMelFilter(WDL_TypedBuf<BL_FLOAT> *result,
                        const WDL_TypedBuf<BL_FLOAT> &magns,
                        BL_FLOAT sampleRate, int numFilters)
{
    // Create filters
    //
    BL_FLOAT lowFreqMel = 0.0;
    BL_FLOAT highFreqMel = HzToMel(sampleRate*0.5);
    
    // Compute equally spaced mel values
    WDL_TypedBuf<BL_FLOAT> melPoints;
    melPoints.Resize(numFilters + 2);
    for (int i = 0; i < melPoints.GetSize(); i++)
    {
        // Compute mel value
        BL_FLOAT t = ((BL_FLOAT)i)/(melPoints.GetSize() - 1);
        BL_FLOAT val = lowFreqMel + t*(highFreqMel - lowFreqMel);
        
        melPoints.Get()[i] = val;
    }
    
    BLDebug::DumpData("mel-points.txt", melPoints);
    
    // Compute mel points
    WDL_TypedBuf<BL_FLOAT> hzPoints;
    hzPoints.Resize(melPoints.GetSize());
    for (int i = 0; i < hzPoints.GetSize(); i++)
    {
        // Compute hz value
        BL_FLOAT val = melPoints.Get()[i];
        
        val = MelToHz(val);
        
        hzPoints.Get()[i] = val;
    }
    
    BLDebug::DumpData("hz-points.txt", hzPoints);
    
    // Compute bin points
    WDL_TypedBuf<BL_FLOAT> bin;
    bin.Resize(hzPoints.GetSize());
    
    //BL_FLOAT hzPerBinInv = (magns.GetSize()*2.0 - 1)/sampleRate;
    BL_FLOAT hzPerBinInv = (magns.GetSize() + 1)/(sampleRate*0.5);
    
    for (int i = 0; i < bin.GetSize(); i++)
    {
        // Compute hz value
        BL_FLOAT val = hzPoints.Get()[i];
        
        val = val*hzPerBinInv;
        val = std::floor(val);
        
        bin.Get()[i] = val;
    }
    
    BLDebug::DumpData("bin-points.txt", bin);
    
    // Apply filters
    //
    result->Resize(numFilters);
    BLUtils::FillAllZero(result);
    
    for (int m = 1; m < numFilters + 1; m++)
    {
        WDL_TypedBuf<double> dbgFilterBank;
        dbgFilterBank.Resize(numFilters);
        BLUtils::FillAllZero(&dbgFilterBank);
        
        int f_m_minus = int(bin.Get()[m - 1]); // left
        int f_m = int(bin.Get()[m]);           // center
        int f_m_plus = int(bin.Get()[m + 1]);  // right
        
        for (int k = f_m_minus; k < f_m; k++)
        {
            BL_FLOAT t = (k - bin.Get()[m - 1])/(bin.Get()[m] - bin.Get()[m - 1]);
            result->Get()[m - 1] += t*magns.Get()[k];
            
            dbgFilterBank.Get()[k] = t;
        }
        
        for (int k = f_m; k </*=*/ f_m_plus; k++)
        {
            BL_FLOAT t = (bin.Get()[m + 1] - k)/(bin.Get()[m + 1] - bin.Get()[m]);
            result->Get()[m - 1] += t*magns.Get()[k];
            
            dbgFilterBank.Get()[k] = t;
        }
        
        if (m == 1)
        {
            BLDebug::DumpData("filters.txt", dbgFilterBank);
        }
        else
        {
            BLDebug::AppendNewline("filters.txt");
            BLDebug::AppendData("filters.txt", dbgFilterBank);
        }
    }
    
    // TODO: manage attenuation (triangles area must always have the same area
}
