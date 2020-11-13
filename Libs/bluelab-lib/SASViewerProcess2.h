//
//  SASViewerProcess2.h
//  BL-Waves
//
//  Created by Pan on 20/04/18.
//
//

#ifndef __BL_SASViewer__SASViewerProcess2__
#define __BL_SASViewer__SASViewerProcess2__

#ifdef IGRAPHICS_NANOVG

#include <deque>
using namespace std;

#include <SmoothAvgHistogram.h>
#include <CMA2Smoother.h>
#include <LinesRender2.h>
#include <PartialTracker5.h>
#include <SASFrame3.h>
#include <BlaTimer.h>
#include <FftProcessObj16.h>

class PartialTracker5;
class SASFrame3;
class SASViewerRender2;
class SASViewerProcess2 : public ProcessObj
{
public:
    enum Mode
    {
        TRACKING,
        AMPLITUDE,
        FREQUENCY,
        COLOR,
        WARPING
    };
    
    SASViewerProcess2(int bufferSize,
                     BL_FLOAT overlapping, BL_FLOAT oversampling,
                     BL_FLOAT sampleRate);
    
    virtual ~SASViewerProcess2();
    
    void Reset();
    
    void Reset(int overlapping, int oversampling, BL_FLOAT sampleRate);
    
    void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                          const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer);
    
    void ProcessSamplesBuffer(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                              WDL_TypedBuf<BL_FLOAT> *scBuffer);
    
    // Use this to synthetize directly 1/4 of the samples from partials
    // (without overlap in internal)
    void ProcessSamplesBufferWin(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                 const WDL_TypedBuf<BL_FLOAT> *scBuffer);
    
    void SetSASViewerRender(SASViewerRender2 *sasViewerRender);
    
    void SetMode(Mode mode);
    
    void SetThreshold(BL_FLOAT threshold);
    
    void SetPitch(BL_FLOAT pitch);
    
    void SetNoiseMix(BL_FLOAT mix);
    
    void SetHarmonicSoundFlag(bool flag);
    
    void SetSynthMode(SASFrame3::SynthMode mode);
    
    // SideChain
    void SetUseSideChainFlag(bool flag);
    void SetScThreshold(BL_FLOAT threshold);
    void SetScHarmonicSoundFlag(bool flag);
    
    void SetMix(BL_FLOAT mix);

    void SetMixFreqFlag(bool flag);
    void SetMixNoiseFlag(bool flag);
    
    // Amp to dB
    static BL_FLOAT AmpToDBNorm(BL_FLOAT val);
    static BL_FLOAT DBToAmpNorm(BL_FLOAT val);
    
protected:
    void Display();
    
    void ScaleFreqs(WDL_TypedBuf<BL_FLOAT> *values);
    void AmpsToDb(WDL_TypedBuf<BL_FLOAT> *magns);
    
    // Apply freq scale to freq id
    int ScaleFreq(int idx);
    
    void DetectPartials(const WDL_TypedBuf<BL_FLOAT> &magns,
                        const WDL_TypedBuf<BL_FLOAT> &phases);
    
    void DetectScPartials(const WDL_TypedBuf<BL_FLOAT> &magns,
                          const WDL_TypedBuf<BL_FLOAT> &phases);
    
    void AmpsToDBNorm(WDL_TypedBuf<BL_FLOAT> *amps);
    
    void IdToColor(int idx, unsigned char color[3]);
    
    void PartialToColor(const PartialTracker5::Partial &partial,
                        unsigned char color[4]);

    // Utils
    int FindIndex(const vector<int> &ids, int idx);
 
    int FindIndex(const vector<LinesRender2::Point> &points, int idx);
    
    // Optimized version
    void CreateLines(const vector<LinesRender2::Point> &prevPoints);
    
    // Mix noise and harmonic sound
    void MixHarmoNoise(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                       const WDL_TypedBuf<BL_FLOAT> &harmoBuffer);

    void MixFrames(SASFrame3 *result,
                   const SASFrame3 &frame0,
                   const SASFrame3 &frame1,
                   BL_FLOAT t);

    
    // Display
    void DisplayTracking();
    void DisplayAmplitude();
    void DisplayFrequency();
    void DisplayColor();
    void DisplayWarping();
    
    int mBufferSize;
    BL_FLOAT mOverlapping;
    BL_FLOAT mOversampling;
    BL_FLOAT mSampleRate;
    
    WDL_TypedBuf<BL_FLOAT> mCurrentMagns;
    vector<PartialTracker5::Partial> mCurrentPartials;
    
    // Renderer
    SASViewerRender2 *mSASViewerRender;
    
    PartialTracker5 *mPartialTracker;
    PartialTracker5 *mScPartialTracker;
    
    SASFrame3 *mSASFrame;
    SASFrame3 *mScSASFrame;
    SASFrame3 *mMixSASFrame;
    
    Mode mMode;
    BL_FLOAT mThreshold;
    bool mHarmonicFlag;
    
    BL_FLOAT mNoiseMix;
    
    // SideChain
    bool mUseSideChain;
    BL_FLOAT mScThreshold;
    bool mScHarmonicFlag;
    bool mSideChainProvided;
    
    BL_FLOAT mMix;
    
    bool mMixFreqFlag;
    bool mMixNoiseFlag;
    
    // For tracking display
    deque<vector<LinesRender2::Point> > mPartialsPoints;
    long int mAddNum;
    
    // Keep an history, to avoid recomputing the whole lines each time
    // With this, we compute only the new extremity of the line
    vector<vector<LinesRender2::Point> > mPartialLines;
};

#endif // IGRAPHICS_NANOVG

#endif /* defined(__BL_SASViewer__SASViewerProcess2__) */
