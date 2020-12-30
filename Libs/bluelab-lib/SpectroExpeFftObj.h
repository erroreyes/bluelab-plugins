//
//  SpectroExpeFftObj.h
//  BL-SpectroExpe
//
//  Created by Pan on 02/06/18.
//
//

#ifndef __BL_SpectroExpe__SpectroExpeFftObj__
#define __BL_SpectroExpe__SpectroExpeFftObj__

#include <BLTypes.h>

#include <FftProcessObj16.h>

// From ChromaFftObj
//

// SpectrogramDisplayScroll => SpectrogramDisplayScroll3
// SpectroExpeFftObj: From GhostViewerFftObj
class BLSpectrogram4;
class SpectrogramDisplayScroll3;
class SpectroExpeFftObj : public MultichannelProcess
{
public:
    SpectroExpeFftObj(int bufferSize, int oversampling, int freqRes,
                      BL_FLOAT sampleRate);
    
    virtual ~SpectroExpeFftObj();

    void ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
                         const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer);
    
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

#endif /* defined(__BL_SpectroExpe__SpectroExpeFftObj__) */
