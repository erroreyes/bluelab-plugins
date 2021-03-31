//
//  RebalanceProcessFftObjComp4.h
//  BL-Rebalance
//
//  Created by applematuer on 5/17/20.
//
//

#ifndef __BL_Rebalance__RebalanceProcessFftObjComp4__
#define __BL_Rebalance__RebalanceProcessFftObjComp4__

#include "IPlug_include_in_plug_hdr.h"

#include "../../WDL/fft.h"

#include <FftProcessObj16.h>

// RebalanceProcessFftObj
//
// Apply the masks to the current channels
//
// RebalanceProcessFftObjComp: from RebalanceProcessFftObj
// - use RebalanceMaskPredictorComp
//
// RebalanceProcessFftObjComp2:
// for RebalanceMaskPredictorComp3
//
// RebalanceProcessFftObjComp3: for ResampProcessObj
// (define target sample rate, more corret for any source sample rate,
// more correct than just downsacaling the mask)

//class RebalanceMaskPredictorComp7;
//class SoftMaskingNComp;
class RebalanceMaskPredictor8;
class RebalanceMaskProcessor;
class Scale;
class SoftMaskingComp4;
class BLSpectrogram4;
class SpectrogramDisplayScroll3;
class RebalanceProcessFftObjComp4 : public ProcessObj
{
public:
    RebalanceProcessFftObjComp4(int bufferSize, int oversampling,
                                BL_FLOAT sampleRate,
                                RebalanceMaskPredictor8 *maskPred,
                                int numInputCols,
                                int softMaskHistoSize);
    
    virtual ~RebalanceProcessFftObjComp4();
    
    void Reset(int bufferSize, int oversampling, int freqRes, BL_FLOAT sampleRate);
    void Reset();

    BLSpectrogram4 *GetSpectrogram();
    void SetSpectrogramDisplay(SpectrogramDisplayScroll3 *spectroDisplay);
    
    virtual void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                  const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
    
    void SetVocal(BL_FLOAT vocal);
    void SetBass(BL_FLOAT bass);
    void SetDrums(BL_FLOAT drums);
    void SetOther(BL_FLOAT other);

    void SetVocalSensitivity(BL_FLOAT vocal);
    void SetBassSensitivity(BL_FLOAT bass);
    void SetDrumsSensitivity(BL_FLOAT drums);
    void SetOtherSensitivity(BL_FLOAT other);
    
    // Global precision (previous soft/hard)
    void SetContrast(BL_FLOAT contrast);
    
protected:
    void AddSpectrogramLine(const WDL_TypedBuf<BL_FLOAT> &magns,
                            const WDL_TypedBuf<BL_FLOAT> &phases);
    
    void ResetSamplesHistory();
    void ResetMixColsComp();

    void ApplyMask(const WDL_TypedBuf<WDL_FFT_COMPLEX> &inData,
                   WDL_TypedBuf<WDL_FFT_COMPLEX> *outData,
                   const WDL_TypedBuf<BL_FLOAT> &masks);

    void ApplySoftMasking(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioData,
                          const WDL_TypedBuf<BL_FLOAT> &mask);

    void ComputeInverseDB(WDL_TypedBuf<BL_FLOAT> *magns);

    void ComputeResult(const WDL_TypedBuf<WDL_FFT_COMPLEX> &mixBuffer,
                       const WDL_TypedBuf<BL_FLOAT> masks[NUM_STEM_SOURCES],
                       WDL_TypedBuf<WDL_FFT_COMPLEX> *result,
                       WDL_TypedBuf<BL_FLOAT> *resMagns,
                       WDL_TypedBuf<BL_FLOAT> *resPhases);

    void RecomputeSpectrogram();
        
    //
    BL_FLOAT mSampleRate;
    
    BLSpectrogram4 *mSpectrogram;
    SpectrogramDisplayScroll3 *mSpectroDisplay;
    
    //
    RebalanceMaskPredictor8 *mMaskPred;
    RebalanceMaskProcessor *mMaskProcessor;
    
    int mNumInputCols;
    
    // Keep the history of input data
    // So we can get exactly the same correponding the the correct location of the mask
    deque<WDL_TypedBuf<WDL_FFT_COMPLEX> > mSamplesHistory;
    
    SoftMaskingComp4 *mSoftMasking;
    
    deque<WDL_TypedBuf<WDL_FFT_COMPLEX> > mMixColsComp;

    Scale *mScale;

    // Keep masks history, so when chaging parameters, all the spectrogram changes
    deque<WDL_TypedBuf<BL_FLOAT> > mMasksHistory[NUM_STEM_SOURCES];
    deque<WDL_TypedBuf<WDL_FFT_COMPLEX> > mSignalHistory;
};

#endif /* defined(__BL_Rebalance__RebalanceProcessFftObjComp4__) */