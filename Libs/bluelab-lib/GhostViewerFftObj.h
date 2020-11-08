//
//  GhostViewerFftObj.h
//  BL-GhostViewer
//
//  Created by Pan on 02/06/18.
//
//

#ifndef __BL_GhostViewer__GhostViewerFftObj__
#define __BL_GhostViewer__GhostViewerFftObj__

#include "FftProcessObj16.h"

// From ChromaFftObj
//

// SpectrogramDisplayScroll => SpectrogramDisplayScroll3

class BLSpectrogram4;
class SpectrogramDisplayScroll3;
class GhostViewerFftObj : public ProcessObj
{
public:
    GhostViewerFftObj(int bufferSize, int oversampling, int freqRes,
                      BL_FLOAT sampleRate);
    
    virtual ~GhostViewerFftObj();
    
    void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                          const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer = NULL);
    
    void Reset(int bufferSize, int oversampling, int freqRes, BL_FLOAT sampleRate);
    
    BLSpectrogram4 *GetSpectrogram();
    
    void SetSpectrogramDisplay(SpectrogramDisplayScroll3 *spectroDisplay);
    
    void SetSpeedMod(int speedMod);
    
protected:
    void AddSpectrogramLine(const WDL_TypedBuf<BL_FLOAT> &magns,
                            const WDL_TypedBuf<BL_FLOAT> &phases);

    //
    BLSpectrogram4 *mSpectrogram;
    
    SpectrogramDisplayScroll3 *mSpectroDisplay;
    
    long mLineCount;
    
    deque<WDL_TypedBuf<BL_FLOAT> > mOverlapLines;
    
    int mSpeedMod;
};

#endif /* defined(__BL_GhostViewer__GhostViewerFftObj__) */
