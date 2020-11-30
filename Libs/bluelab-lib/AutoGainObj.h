//
//  AutoGainObj.hpp
//  BL-AutoGain-macOS
//
//  Created by applematuer on 11/26/20.
//
//

#ifndef AutoGainObj_hpp
#define AutoGainObj_hpp

#include <vector>
using namespace std;

#include <BLTypes.h>
#include <FftProcessObj16.h>

#include "IPlug_include_in_plug_hdr.h"

// NOT USED: Test for Protools
#define SKIP_FIRST_FRAME 0

// FIX: fixes the jump to +12dB when starting playback
// (detected on Protools)
#define FIX_START_JUMP 1

class SmoothAvgHistogram;
class ParamSmoother;
class AutoGainObj : public MultichannelProcess
{
public:
    enum Mode
    {
        BYPASS_WRITE = 0,
        READ = 1
    };
    
    AutoGainObj(int bufferSize, int oversampling,
                int freqRes, BL_FLOAT sampleRate,
                BL_FLOAT minGain, BL_FLOAT maxGain);
    
    virtual ~AutoGainObj();
    
    void Reset();
    
    void Reset(int bufferSize, int overlapping,
               int oversampling, BL_FLOAT sampleRate);
    
    void SetMode(Mode mode);
    
    void SetThreshold(BL_FLOAT threshold);
    void SetPrecision(BL_FLOAT precision);
    void SetDryWet(BL_FLOAT dryWet);
    
    BL_FLOAT GetGain();
    void SetGain(BL_FLOAT gain);
    
    void SetScGain(BL_FLOAT gain);
    
    void SetGainSmooth(BL_FLOAT gainSmooth);
    
    //
    void ProcessInputSamplesWin(vector<WDL_TypedBuf<BL_FLOAT> * > *ioSamples,
                                const vector<WDL_TypedBuf<BL_FLOAT> > *scBuffer);
    
    void ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
                         const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer);
    
    // For graph curves
    void GetCurveSignal0(WDL_TypedBuf<BL_FLOAT> *signal0);
    void GetCurveSignal1(WDL_TypedBuf<BL_FLOAT> *signal1);
    void GetCurveResult(WDL_TypedBuf<BL_FLOAT> *result);
    
protected:
    static void ApplyGain(const vector<WDL_TypedBuf<BL_FLOAT> > &inSamples,
                          vector<WDL_TypedBuf<BL_FLOAT> > *outSamples,
                          BL_FLOAT gainDB);
    
    static void ApplyGainConstantSc(BL_FLOAT *scConstantValue,
                                    BL_FLOAT gainDB);
    
    static void ApplyDryWet(const vector<WDL_TypedBuf<BL_FLOAT> > &inSamples,
                            vector<WDL_TypedBuf<BL_FLOAT> > *outSamples,
                            BL_FLOAT dryWet);
    
    BL_FLOAT ComputeOutGainSpect(const vector<WDL_TypedBuf<BL_FLOAT> > &inSamples,
                                 const vector<WDL_TypedBuf<BL_FLOAT> > &scIn);
    
    BL_FLOAT ComputeOutGainSpectConstantSc(const vector<WDL_TypedBuf<BL_FLOAT> > &inSamples,
                                           BL_FLOAT constantScValue);
    
    BL_FLOAT ComputeOutGainSpectAux(const WDL_TypedBuf<BL_FLOAT> &dbIn,
                                    const WDL_TypedBuf<BL_FLOAT> &dbSc,
                                    BL_FLOAT inGain);
    
    BL_FLOAT ComputeInGain(const WDL_TypedBuf<BL_FLOAT> &monoIn);
    
    //
    BL_FLOAT ComputeOutGainRMS(const vector<WDL_TypedBuf<BL_FLOAT> > &inSamples,
                               const vector<WDL_TypedBuf<BL_FLOAT> > &scIn);
    
    BL_FLOAT ComputeFftGain(const WDL_TypedBuf<BL_FLOAT> &avgIn,
                            const WDL_TypedBuf<BL_FLOAT> &avgSc);
    
    BL_FLOAT ComputeFftGain2(const WDL_TypedBuf<BL_FLOAT> &avgIn,
                             const WDL_TypedBuf<BL_FLOAT> &avgSc);
    
    static void BoundScCurve(WDL_TypedBuf<BL_FLOAT> *curve,
                             BL_FLOAT mindB, BL_FLOAT maxdB,
                             BL_FLOAT scGain);
    
    void UpdateGraphReadMode(const vector<WDL_TypedBuf<BL_FLOAT> > &inData,
                             const vector<WDL_TypedBuf<BL_FLOAT> > &outData);
    
    
    //
    BL_FLOAT mThreshold;
    BL_FLOAT mPrecision;
    BL_FLOAT mDryWet;
    
    BL_FLOAT mGain;
    BL_FLOAT mScGain;
    
    Mode mMode;
    
    //
    bool mConstantSc;
    BL_FLOAT mScConstantValue;
    
    //
    SmoothAvgHistogram *mAvgHistoIn;
    SmoothAvgHistogram *mAvgHistoScIn;
    
    // For read mode
    SmoothAvgHistogram *mAvgHistoOut;
    
    //
    WDL_TypedBuf<BL_FLOAT> mAWeights;
    
    //
    ParamSmoother *mScSamplesSmoother;
    ParamSmoother *mInSamplesSmoother;
    ParamSmoother *mGainSmoother;
    
    //
    int mBufferSize;
    BL_FLOAT mSampleRate;
    
#if SKIP_FIRST_FRAME
    // Used to ignore the processing until we reach "latency" num samples
    // FIX: fixed Protools when start playing, gain jumps to +12
    unsigned long long mNumSamples;
#endif
    
#if FIX_START_JUMP
    bool mHasJustReset;
#endif
    
    // Data for curves
    WDL_TypedBuf<BL_FLOAT> mCurveSignal0;
    WDL_TypedBuf<BL_FLOAT> mCurveSignal1;
    WDL_TypedBuf<BL_FLOAT> mCurveResult;
    
    BL_FLOAT mMinGain;
    BL_FLOAT mMaxGain;
};

#endif /* AutoGainObj_hpp */
