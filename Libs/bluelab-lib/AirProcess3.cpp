//
//  AirProcess3.cpp
//  BL-Air
//
//  Created by Pan on 20/04/18.
//
//

#include <BLUtils.h>
#include <BLDebug.h>

#include <PartialTracker5.h>
#include <SoftMaskingComp4.h>

#include "AirProcess3.h"

#define USE_SOFT_MASKING 0 //1 // 0
// 8 gives more gating, but less musical noise remaining
#define SOFT_MASKING_HISTO_SIZE 8


AirProcess3::AirProcess3(int bufferSize,
                         BL_FLOAT overlapping, BL_FLOAT oversampling,
                         BL_FLOAT sampleRate)
: ProcessObj(bufferSize)
{
    mBufferSize = bufferSize;
    mOverlapping = overlapping;
    mOversampling = oversampling;
    
    mSampleRate = sampleRate;
    
    mPartialTracker = new PartialTracker5(bufferSize, sampleRate, overlapping);
    
    mMix = 0.5;
    
    mSoftMaskingComp = NULL;
#if USE_SOFT_MASKING
    mSoftMaskingComp = new SoftMaskingComp4(bufferSize, overlapping,
                                            SOFT_MASKING_HISTO_SIZE);
#endif
}

AirProcess3::~AirProcess3()
{
    delete mPartialTracker;
    
    if (mSoftMaskingComp != NULL)
        delete mSoftMaskingComp;
}

void
AirProcess3::Reset()
{
    Reset(mBufferSize, mOverlapping, mOversampling, mSampleRate);
}

void
AirProcess3::Reset(int bufferSize, int overlapping,
                   int oversampling, BL_FLOAT sampleRate)
{
    mBufferSize = bufferSize;
    
    mOverlapping = overlapping;
    mOversampling = oversampling;
    
    mSampleRate = sampleRate;
    
    mPartialTracker->Reset(bufferSize, sampleRate);
    
#if USE_SOFT_MASKING
    if (mSoftMaskingComp != NULL)
        mSoftMaskingComp->Reset(bufferSize, overlapping);
#endif
}

void
AirProcess3::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer0,
                              const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)

{
    WDL_TypedBuf<WDL_FFT_COMPLEX> &fftSamples0 = mTmpBuf0;
    fftSamples0 = *ioBuffer0;
    
    // Take half of the complexes
    WDL_TypedBuf<WDL_FFT_COMPLEX> &fftSamples = mTmpBuf15;
    BLUtils::TakeHalf(fftSamples0, &fftSamples);
        
    WDL_TypedBuf<BL_FLOAT> &magns = mTmpBuf1;
    WDL_TypedBuf<BL_FLOAT> &phases = mTmpBuf2;
    BLUtils::ComplexToMagnPhase(&magns, &phases, fftSamples);
    
    DetectPartials(magns, phases);
        
    if (mPartialTracker != NULL)
    {
        // "Envelopes"
        //
        
        // Noise "envelope"
        mPartialTracker->GetNoiseEnvelope(&mNoise);
        mPartialTracker->DenormData(&mNoise);
                        
        // Harmonic "envelope"
        mPartialTracker->GetHarmonicEnvelope(&mHarmo);
        mPartialTracker->DenormData(&mHarmo);
                
        BL_FLOAT noiseCoeff;
        BL_FLOAT harmoCoeff;
        BLUtils::MixParamToCoeffs(mMix, &noiseCoeff, &harmoCoeff);
     
#if !USE_SOFT_MASKING
        // Result
        WDL_TypedBuf<BL_FLOAT> &newMagns = mTmpBuf7;
        newMagns.Resize(magns.GetSize());
        for (int i = 0; i < newMagns.GetSize(); i++)
        {
            BL_FLOAT n = mNoise.Get()[i];
            BL_FLOAT h = mHarmo.Get()[i];
            
            BL_FLOAT val = n*noiseCoeff + h*harmoCoeff;
            newMagns.Get()[i] = val;
        }
        magns = newMagns;

        mSum = magns;

        // Result
        BLUtils::MagnPhaseToComplex(&fftSamples, magns, phases);
        
        BLUtils::FillSecondFftHalf(fftSamples, ioBuffer0);
        
#else // Use oft masking

        // Compute noise mask
        WDL_TypedBuf<BL_FLOAT> &noiseMask = mTmpBuf17;
        ComputeNoiseMask(mNoise, mHarmo, &noiseMask);

        WDL_TypedBuf<WDL_FFT_COMPLEX> &softMaskedResult0 = mTmpBuf18;
        WDL_TypedBuf<WDL_FFT_COMPLEX> &softMaskedResult1 = mTmpBuf19;
        mSoftMaskingComp->ProcessCentered(&fftSamples,
                                          noiseMask,
                                          &softMaskedResult0,
                                          &softMaskedResult1);
    
        if (mSoftMaskingComp->IsProcessingEnabled())
        {
            // Apply "mix"
            BLUtils::MultValues(&softMaskedResult0, noiseCoeff);
            BLUtils::MultValues(&softMaskedResult1, harmoCoeff);

            // Sum
            fftSamples = softMaskedResult0;
            BLUtils::AddValues(&fftSamples, softMaskedResult1);
        }

        // Keep the sum
        BLUtils::ComplexToMagn(&mSum, fftSamples);

        // Result
        BLUtils::FillSecondFftHalf(fftSamples, ioBuffer0);
#endif
    }
}

void
AirProcess3::SetThreshold(BL_FLOAT threshold)
{
    mPartialTracker->SetThreshold(threshold);
}

void
AirProcess3::SetMix(BL_FLOAT mix)
{
    mMix = mix;
}

int
AirProcess3::GetLatency()
{
#if USE_SOFT_MASKING
    int latency = mSoftMaskingComp->GetLatency();
   
    return latency;
#endif
    
    return 0;
}

void
AirProcess3::GetNoise(WDL_TypedBuf<BL_FLOAT> *magns)
{
    *magns = mNoise;
}

void
AirProcess3::GetHarmo(WDL_TypedBuf<BL_FLOAT> *magns)
{
    *magns = mHarmo;
}

void
AirProcess3::GetSum(WDL_TypedBuf<BL_FLOAT> *magns)
{
    *magns = mSum;
}

void
AirProcess3::DetectPartials(const WDL_TypedBuf<BL_FLOAT> &magns,
                            const WDL_TypedBuf<BL_FLOAT> &phases)
{
    mPartialTracker->SetData(magns, phases);
    
    mPartialTracker->DetectPartials();
    
    // Filter or not ?
    //
    // If we filter and we get zombie partials
    // this would make musical noise
    mPartialTracker->FilterPartials();
    
    mPartialTracker->ExtractNoiseEnvelope();
}

void
AirProcess3::ComputeNoiseMask(const WDL_TypedBuf<BL_FLOAT> &noise,
                              const WDL_TypedBuf<BL_FLOAT> &harmo,
                              WDL_TypedBuf<BL_FLOAT> *noiseMask)
{
    noiseMask->Resize(noise.GetSize());
    BLUtils::FillAllZero(noiseMask);
    
    for (int i = 0; i < noise.GetSize(); i++)
    {
        BL_FLOAT n = noise.Get()[i];
        BL_FLOAT h = harmo.Get()[i];

        BL_FLOAT s = n + h;
        if (s > BL_EPS)
        {
            BL_FLOAT m = n/s;
            noiseMask->Get()[i] = m;
        }
    }
}
