//
//  ResampProcessObj.h
//  Saturate
//
//  Created by Apple m'a Tuer on 12/11/17.
//
//

#ifndef __Saturate__ResampProcessObj__
#define __Saturate__ResampProcessObj__

#include "IPlug_include_in_plug_hdr.h"

// WDL
#define OVERSAMPLER_CLASS Oversampler4

// r8brain
//#define OVERSAMPLER_CLASS Oversampler5

class Oversampler4;
class Oversampler5;

class FilterRBJNX;
class FilterIIRLow12dBNX;
class FilterButterworthLPFNX;
class FilterSincConvoLPF;
class FilterFftLowPass;

// Good, but less steep than sinc
#define USE_RBJ_FILTER 0

// Test: Use NIIRFilterLow12dB
// Good, but less steep than sinc
#define USE_IIR_FILTER 0

// NOTE: butterworth has a bug:
// at the early beginning, there is a lot of sound of very
// high amplitude
// NOTE: Butterworth is not so steep, even when chaining 100 filters
#define USE_BUTTERWORTH_FILTER 0

// GOOD: exactly like the result of Sir Clipper
#define USE_SINC_CONVO_FILTER 1

// Ultra-steep, but introduce plugin latency
#define USE_FFT_LOW_PASS_FILTER 0 //1

// From OversampProcessObj3
// Use target sample rate instead of int oversampling factor
class ResampProcessObj
{
public:
    ResampProcessObj(BL_FLOAT targetSampleRate, BL_FLOAT sampleRate,
                     bool filterNyquist);
    
    virtual ~ResampProcessObj();
    
    // Must be called at least once
    void Reset(BL_FLOAT sampleRate);
    
    void Reset(BL_FLOAT sampleRate, int blockSize);
    
    int GetLatency();
    
    void Process(WDL_TypedBuf<BL_FLOAT> *ioBuffer);
    
protected:
    // Return true if the result data is in ioResampBuffer
    // In the other case, ioBuffer has been directly modified
    // and we ignore ioResampBuffer after this call.
    virtual bool ProcessSamplesBuffer(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                      WDL_TypedBuf<BL_FLOAT> *ioResampBuffer) = 0;
    
    void InitFilter(BL_FLOAT sampleRate);
    
    //
    OVERSAMPLER_CLASS *mForwardResampler;
    OVERSAMPLER_CLASS *mBackwardResampler;
    
#if USE_RBJ_FILTER
    FilterRBJNX *mFilter;
#endif
    
#if USE_IIR_FILTER
    FilterIIRLow12dBNX *mFilter;
#endif
    
#if USE_BUTTERWORTH_FILTER
    FilterButterworthLPFNX *mFilter;
#endif
    
#if USE_SINC_CONVO_FILTER
    FilterSincConvoLPF *mFilter;
#endif
    
#if USE_FFT_LOW_PASS_FILTER
    FftLowPassFilter *mFilter;
#endif
    
    BL_FLOAT mSampleRate;
    BL_FLOAT mTargetSampleRate;
};

#endif /* defined(__Saturate__ResampProcessObj__) */
