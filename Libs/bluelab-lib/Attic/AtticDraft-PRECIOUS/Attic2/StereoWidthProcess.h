//
//  StereoWidthProcess.h
//  BL-PitchShift
//
//  Created by Pan on 20/04/18.
//
//

#ifndef __BL_PitchShift__StereoWidthProcess__
#define __BL_PitchShift__StereoWidthProcess__

#include "FftProcessObj14.h"

class StereoWidthProcess : public MultichannelProcess
{
public:
    StereoWidthProcess(int bufferSize,
                       double overlapping, double oversampling,
                       double sampleRate);
    
    virtual ~StereoWidthProcess();
    
    void Reset();
    
    void Reset(int overlapping, int oversampling, double sampleRate);
    
    void SetWidthChange(double widthChange);
    
    void SetFakeStereo(bool flag);
    
    void ProcessInputSamples(vector<WDL_TypedBuf<double> * > *ioSamples);
    
    void ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples);
    
    // Get the result
    void GetWidthValues(WDL_TypedBuf<double> *xValues,
                        WDL_TypedBuf<double> *yValues);
    
protected:
    // We have to change the magns too
    // ... because if we decrease at the maximum, the two magns channels
    // must be similar
    // (otherwise it vibrates !)
    void ApplyWidthChange(WDL_TypedBuf<double> *ioPhasesL,
                          WDL_TypedBuf<double> *ioPhasesR,
                          WDL_TypedBuf<double> *ioMagnsL,
                          WDL_TypedBuf<double> *ioMagnsR);
    
    void Stereoize(WDL_TypedBuf<double> *ioPhasesL,
                   WDL_TypedBuf<double> *ioPhasesR);
    
    void GenerateRandomCoeffs(int size);

    void MagnPhasesToDistances(double *distL, double *distR,
                               double magnsL, double magnsR,
                               double phasesL, double phasesR,
                               double freq);
    
    void DistancesToMagnPhases(double distL, double distR,
                               double *newMagnL, double *newMagnR,
                               double *newPhaseL, double *newPhaseR,
                               double freq);

    int mBufferSize;
    double mOverlapping;
    double mOversampling;
    double mSampleRate;
    
    double mWidthChange;
    
    bool mFakeStereo;
    
    
    WDL_TypedBuf<double> mWidthValuesX;
    WDL_TypedBuf<double> mWidthValuesY;
    
    WDL_TypedBuf<double> mCurrentSamples;
    
    WDL_TypedBuf<double> mRandomCoeffs;
};

#endif /* defined(__BL_PitchShift__StereoPhasesProcess__) */
