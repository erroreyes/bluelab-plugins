//
//  ResampProcessObj.cpp
//  Saturate
//
//  Created by Apple m'a Tuer on 12/11/17.
//
//

#include <Oversampler4.h>
#include <Oversampler5.h>

#include <FilterRBJNX.h>
#include <FilterIIRLow12dBNX.h>
#include <FilterButterworthLPFNX.h>
#include <FilterSincConvoLPF.h>
#include <FilterFFTLowPass.h>

#include <BLUtils.h>

#include "ResampProcessObj.h"


#define NYQUIST_COEFF 0.5 // 0.45

#define NYQUIST_FILTER_ORDER 1

//#define SINC_FILTER_SIZE 64 // ORIGIN, less steep
#define SINC_FILTER_SIZE 256 // GOOD, steepness like Sir Clipper


ResampProcessObj::ResampProcessObj(BL_FLOAT targetSampleRate,
                                   BL_FLOAT sampleRate,
                                   bool filterNyquist)
{
    mFilter = NULL;
    
    mSampleRate = sampleRate;
    mTargetSampleRate = targetSampleRate;
    
    if (filterNyquist)
    {
        InitFilter(sampleRate);
    }
    
    int dummyBlockSize = 1024;
    
    BL_FLOAT oversampling = mTargetSampleRate/sampleRate;
    if (oversampling < 1.0)
        oversampling = 1.0/oversampling;
    
    bool upsampleForward = (mTargetSampleRate > sampleRate);
    
    mForwardResampler = new OVERSAMPLER_CLASS(oversampling, upsampleForward);
    mForwardResampler->Reset(sampleRate, dummyBlockSize);
    
    mBackwardResampler = new OVERSAMPLER_CLASS(oversampling, !upsampleForward);
    mBackwardResampler->Reset(targetSampleRate, dummyBlockSize);
}

ResampProcessObj::~ResampProcessObj()
{
    delete mForwardResampler;
    delete mBackwardResampler;
    
    if (mFilter != NULL)
        delete mFilter;
}

void
ResampProcessObj::Reset(BL_FLOAT sampleRate, int blockSize)
{
    mSampleRate = sampleRate;
    
    mForwardResampler->Reset(sampleRate, blockSize);
    mBackwardResampler->Reset(mTargetSampleRate, blockSize);
    
    if (mFilter != NULL)
    {
        InitFilter(sampleRate);
    }
    
#if USE_SINC_CONVO_FILTER
    if (mFilter != NULL)
    {
        mFilter->Reset(sampleRate, blockSize);
    }
#endif
}

int
ResampProcessObj::GetLatency()
{
    int latency = 0;
    
    latency += mForwardResampler->GetLatency();
    latency += mBackwardResampler->GetLatency();
    
#if USE_SINC_CONVO_FILTER
    latency += mFilter->GetLatency();
#endif

#if USE_FFT_LOW_PASS_FILTER
    latency += mFilter->GetLatency();
#endif

    return latency;
}

void
ResampProcessObj::Process(WDL_TypedBuf<BL_FLOAT> *ioBuffer)
{
    if (std::fabs(mSampleRate - mTargetSampleRate) < BL_EPS)
    // Nothing to do
    {
        
        ProcessSamplesBuffer(ioBuffer, NULL);
        
        return;
    }
    
    WDL_TypedBuf<BL_FLOAT> resampBuffer = *ioBuffer;
    
    // Upsample
    mForwardResampler->Resample(&resampBuffer);
    
    bool resultIsResampBuffer = ProcessSamplesBuffer(ioBuffer, &resampBuffer);
    
    if (resultIsResampBuffer)
    {
        // Filter Nyquist ?
        if (mFilter != NULL)
        {
            WDL_TypedBuf<BL_FLOAT> filtBuffer;
            mFilter->Process(&filtBuffer, resampBuffer);
        
            resampBuffer = filtBuffer;
        }
    
        mBackwardResampler->Resample(&resampBuffer);
    
        *ioBuffer = resampBuffer;
    }
}

void
ResampProcessObj::InitFilter(BL_FLOAT sampleRate)
{
    // Default: upsample first
    // We will use the filters when downsampling
    BL_FLOAT filterSampleRate = mTargetSampleRate;
    BL_FLOAT nyquistFreq = sampleRate*NYQUIST_COEFF;
    
    if (mTargetSampleRate < sampleRate)
        // Downsample first
    {
        filterSampleRate = sampleRate;
        nyquistFreq = mTargetSampleRate*NYQUIST_COEFF;
    }
    
#if USE_RBJ_FILTER
    mFilter = new FilterRBJNX(NYQUIST_FILTER_ORDER,
                              FILTER_TYPE_LOWPASS,
                              filterSampleRate, nyquistFreq);
#endif
    
#if USE_IIR_FILTER
    mFilter = new FilterIIRLow12dBNX(NYQUIST_FILTER_ORDER);
    mFilter->Init(nyquistFreq, filterSampleRate);
#endif
    
#if USE_BUTTERWORTH_FILTER
    mFilter = new FilterButterworthLPFNX(NYQUIST_FILTER_ORDER);
    mFilter->Init(nyquistFreq, filterSampleRate);
#endif
    
#if USE_SINC_CONVO_FILTER
    mFilter = new FilterSincConvoLPF();
    mFilter->Init(nyquistFreq, filterSampleRate, SINC_FILTER_SIZE);
#endif
    
#if USE_FFT_LOW_PASS_FILTER
    mFilter = new FilterFftLowPass();
    mFilter->Init(nyquistFreq, filterSampleRate);
#endif
}
