//
//  BL_PanogramFftObj.h
//  BL-Ghost
//
//  Created by Pan on 02/06/18.
//
//

#ifndef __BL_Pano__PanogramFftObj__
#define __BL_Pano__PanogramFftObj__

#include "FftProcessObj16.h"

// From ChromaFftObj
//

class BLSpectrogram4;
class SpectrogramDisplayScroll3;
class HistoMaskLine2;
class PanogramPlayFftObj;

class PanogramFftObj : public MultichannelProcess
{
public:
    PanogramFftObj(int bufferSize, int oversampling, int freqRes,
                   BL_FLOAT sampleRate);
    
    virtual ~PanogramFftObj();
    
    void ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
                         const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer);
    
    void Reset(int bufferSize, int oversampling, int freqRes, BL_FLOAT sampleRate);
    
    BLSpectrogram4 *GetSpectrogram();
    
    void SetSpectrogramDisplay(SpectrogramDisplayScroll3 *spectroDisplay);
    
    void SetSharpness(BL_FLOAT sharpness);
    
    int GetNumColsAdd();
    
    void SetPlayFftObjs(PanogramPlayFftObj *objs[2]);
    
    void SetEnabled(bool flag);

    // Set public for SpectroExpe
    void MagnsToPanoLine(const WDL_TypedBuf<BL_FLOAT> magns[2],
                         WDL_TypedBuf<BL_FLOAT> *panoLine,
                         HistoMaskLine2 *maskLine = NULL);
    
protected:
    int GetNumCols();
    
    void AddSpectrogramLine(const WDL_TypedBuf<BL_FLOAT> &magns,
                            const WDL_TypedBuf<BL_FLOAT> &phases);
    
    //
    
    BLSpectrogram4 *mSpectrogram;
    
    SpectrogramDisplayScroll3 *mSpectroDisplay;
    
    long mLineCount;
    
    //deque<WDL_TypedBuf<BL_FLOAT> > mOverlapLines;
    
    WDL_TypedBuf<BL_FLOAT> mSmoothWin;
    
    BL_FLOAT mSharpness;
    
    //
    int mBufferSize;
    int mOverlapping;
    int mFreqRes;
    int mSampleRate;
    
    PanogramPlayFftObj *mPlayFftObjs[2];
    
    // For FIX_BAD_DISPLAY_HIGH_SAMPLERATES
    int mAddLineCount;
    
    bool mIsEnabled;
};

#endif /* defined(__BL_BL_Pano__BL_PanogramFftObj__) */
