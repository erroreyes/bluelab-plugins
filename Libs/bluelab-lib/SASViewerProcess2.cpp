//
//  StereoVizProcess3.cpp
//  BL-PitchShift
//
//  Created by Pan on 20/04/18.
//
//

#ifdef IGRAPHICS_NANOVG

#include <BLUtils.h>
#include <DebugGraph.h>

#include <SASViewerRender2.h>

#include <PartialTracker3.h>
#include <SASFrame3.h>

#include "SASViewerProcess2.h"

// With -60, avoid taking background noise
// With -80, takes more partials (but some noise)
#define MIN_DB -60.0 //-80.0
#define MIN_AMP_DB -120.0

// Display magns or SAS param while debugging ?
#define DEBUG_DISPLAY_MAGNS 1  //0 //1

#define DEBUG_MUTE_NOISE 0 //1
#define DEBUG_MUTE_PARTIALS 0 //0 //1

#define SHOW_ONLY_ALIVE 0 //1
#define MIN_AGE_DISPLAY 0 //10 // 4


SASViewerProcess2::SASViewerProcess2(int bufferSize,
                                     BL_FLOAT overlapping, BL_FLOAT oversampling,
                                     BL_FLOAT sampleRate)
: ProcessObj(bufferSize)
{
    mBufferSize = bufferSize;
    mOverlapping = overlapping;
    mOversampling = oversampling;
    
    mSampleRate = sampleRate;
    
    mSASViewerRender = NULL;
    
    mPartialTracker = new PartialTracker5(bufferSize, sampleRate, overlapping);
    mScPartialTracker = new PartialTracker5(bufferSize, sampleRate, overlapping);
    
    mSASFrame = new SASFrame3(bufferSize, sampleRate, overlapping);
    mScSASFrame = new SASFrame3(bufferSize, sampleRate, overlapping);
    mMixSASFrame = new SASFrame3(bufferSize, sampleRate, overlapping);
    
    mThreshold = -60.0;
    mHarmonicFlag = false;
    
    mMode = TRACKING;
    
    mNoiseMix = 0.0;
    
    // SideChain
    mUseSideChain = false;
    mScThreshold = -60.0;
    mScHarmonicFlag = false;
    
    mSideChainProvided = false;
    
    mMix = 0.0;
    
    mMixFreqFlag = true;
    mMixNoiseFlag = true;
    
    // For additional lines
    mAddNum = 0;
    
    mXScale = Scale::MEL;
    //mYScale = Scale::LINEAR;
    mYScale = Scale::DB;
}

SASViewerProcess2::~SASViewerProcess2()
{
    delete mPartialTracker;
    delete mScPartialTracker;
    
    delete mSASFrame;
    delete mScSASFrame;
    delete mMixSASFrame;
}

void
SASViewerProcess2::Reset()
{
    Reset(mOverlapping, mOversampling, mSampleRate);
    
    mSASFrame->Reset(mSampleRate);
    mScSASFrame->Reset(mSampleRate);
    mMixSASFrame->Reset(mSampleRate);
}

void
SASViewerProcess2::Reset(int overlapping, int oversampling,
                         BL_FLOAT sampleRate)
{
    mOverlapping = overlapping;
    mOversampling = oversampling;
    
    mSampleRate = sampleRate;
    
    mSASFrame->Reset(sampleRate);
    mScSASFrame->Reset(sampleRate);
    mMixSASFrame->Reset(sampleRate);
}

void
SASViewerProcess2::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                    const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)

{
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples = *ioBuffer;
    
    mSideChainProvided = false;
    WDL_TypedBuf<WDL_FFT_COMPLEX> scFftSamples;
    if (scBuffer != NULL)
    {
        scFftSamples = *scBuffer;
        
        mSideChainProvided = true;
    }
    
    // Take half of the complexes
    BLUtils::TakeHalf(&fftSamples);
    
    WDL_TypedBuf<BL_FLOAT> magns;
    WDL_TypedBuf<BL_FLOAT> phases;
    BLUtils::ComplexToMagnPhase(&magns, &phases, fftSamples);
    
    // Sc
    BLUtils::TakeHalf(&scFftSamples);
    
    WDL_TypedBuf<BL_FLOAT> scMagns;
    WDL_TypedBuf<BL_FLOAT> scPhases;
    BLUtils::ComplexToMagnPhase(&scMagns, &scPhases, scFftSamples);
    
    if (!mUseSideChain || !mSideChainProvided)
        mCurrentMagns = magns;
    else
        mCurrentMagns = scMagns;
    
    DetectPartials(magns, phases);
    
    if (mUseSideChain)
        DetectScPartials(scMagns, scPhases);
    
    if (mPartialTracker != NULL)
    {
        vector<PartialTracker5::Partial> partials;
        mPartialTracker->GetPartials(&partials);
        
        vector<PartialTracker5::Partial> scPartials;
        mScPartialTracker->GetPartials(&scPartials);
        
        if (!mUseSideChain || !mSideChainProvided)
            mCurrentPartials = partials;
        else
            mCurrentPartials = scPartials;
        
        // Avoid sending garbage partials to the SASFrame
        // (would slow down a lot when many garbage partial
        // are used to compute the SASFrame)
        //PartialTracker3::RemoveRealDeadPartials(&partials);
        
        mSASFrame->SetPartials(partials);
        mScSASFrame->SetPartials(scPartials);
        
        MixFrames(mMixSASFrame, *mSASFrame, *mScSASFrame, mMix);
        
        // Display the current data
        Display();
        
#if !DEBUG_MUTE_NOISE
        // Normal behavior
        // Noise envelope
        if (mMixNoiseFlag)
        {
            mPartialTracker->GetNoiseEnvelope(&magns);
            mScPartialTracker->GetNoiseEnvelope(&scMagns);
        
            WDL_TypedBuf<BL_FLOAT> mixMagns = magns;
            if (scMagns.GetSize() == magns.GetSize())
                BLUtils::Interp(&mixMagns, &magns, &scMagns, mMix);
            magns = mixMagns;
        }
        else
        {
            mPartialTracker->GetNoiseEnvelope(&magns);
        }
#endif
    }
    
    // For noise envelope
    BLUtils::MagnPhaseToComplex(ioBuffer, magns, phases);
    ioBuffer->Resize(ioBuffer->GetSize()*2);
    BLUtils::FillSecondFftHalf(ioBuffer);
}

void
SASViewerProcess2::ProcessSamplesBuffer(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                       WDL_TypedBuf<BL_FLOAT> *scBuffer)
{
    if (!mMixSASFrame->ComputeSamplesFlag())
        return;
    
#if DEBUG_MUTE_NOISE
    // For the moment, empty the io buffer
    BLUtils::FillAllZero(ioBuffer);
#endif
    
#if !DEBUG_MUTE_PARTIALS
    // Create a separate buffer for samples synthesis from partials
    // (because it doesn't use overlap)
    WDL_TypedBuf<BL_FLOAT> samplesBuffer;
    BLUtils::ResizeFillZeros(&samplesBuffer, ioBuffer->GetSize());
    
    // Compute the samples from partials
    mMixSASFrame->ComputeSamples(&samplesBuffer);

    // Mix
    // (the current io buffer may contain shaped noise)
    MixHarmoNoise(ioBuffer, samplesBuffer);
#endif
}

// Use this to synthetize directly 1/4 of the samples from partials
// (without overlap in internal)
void
SASViewerProcess2::ProcessSamplesBufferWin(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                          const WDL_TypedBuf<BL_FLOAT> *scBuffer)
{
    if (!mMixSASFrame->ComputeSamplesWinFlag())
        return;
    
#if DEBUG_MUTE_NOISE
    // For the moment, empty the io buffer
    BLUtils::FillAllZero(ioBuffer);
#endif
    
#if !DEBUG_MUTE_PARTIALS
    // Create a separate buffer for samples synthesis from partials
    // (because it doesn't use overlap)
    WDL_TypedBuf<BL_FLOAT> samplesBuffer;
    BLUtils::ResizeFillZeros(&samplesBuffer, ioBuffer->GetSize());
    
    // Compute the samples from partials
    mMixSASFrame->ComputeSamplesWin(&samplesBuffer);
    
    // Mix
    // (the current io buffer may contain shaped noise)
    MixHarmoNoise(ioBuffer, samplesBuffer);
#endif
}

void
SASViewerProcess2::SetSASViewerRender(SASViewerRender2 *sasViewerRender)
{
    mSASViewerRender = sasViewerRender;
}

void
SASViewerProcess2::SetMode(Mode mode)
{
    if (mode != mMode)
    {
        mMode = mode;
    
        // Clear the previous data
        if (mSASViewerRender != NULL)
            mSASViewerRender->Clear();
    
        mPartialsPoints.clear();
        
        mPartialLines.clear();
        
        // Try to avoid remaining blue lines when quitting
        // tracking mode and returning to it
        mPartialTracker->ClearResult();
        
        if (mSASViewerRender != NULL)
            mSASViewerRender->ClearAdditionalLines();
        
        mCurrentMagns.Resize(0);
        
        Display();
    }
}

void
SASViewerProcess2::SetThreshold(BL_FLOAT threshold)
{
    mThreshold = threshold;
    mPartialTracker->SetThreshold(threshold);
}

void
SASViewerProcess2::SetPitch(BL_FLOAT pitch)
{
    mMixSASFrame->SetPitch(pitch);
}

void
SASViewerProcess2::SetNoiseMix(BL_FLOAT mix)
{
    mNoiseMix = mix;
}

void
SASViewerProcess2::SetHarmonicSoundFlag(bool flag)
{
    mHarmonicFlag = flag;
    mSASFrame->SetHarmonicSoundFlag(flag);
}

void
SASViewerProcess2::SetSynthMode(SASFrame3::SynthMode mode)
{
    mMixSASFrame->SetSynthMode(mode);
}

// SideChain
//

void
SASViewerProcess2::SetUseSideChainFlag(bool flag)
{
    mUseSideChain = flag;
    
    if (!mUseSideChain)
    {
        mPartialTracker->SetThreshold(mThreshold);
    }
    else
    {
        mScPartialTracker->SetThreshold(mScThreshold);
    }
}

void
SASViewerProcess2::SetScThreshold(BL_FLOAT threshold)
{
    mScThreshold = threshold;
    
    mScPartialTracker->SetThreshold(mScThreshold);
}

void
SASViewerProcess2::SetScHarmonicSoundFlag(bool flag)
{
    mScHarmonicFlag = flag;
    
    mScSASFrame->SetHarmonicSoundFlag(flag);
}


void
SASViewerProcess2::SetMix(BL_FLOAT mix)
{
    mMix = mix;
}

void
SASViewerProcess2::SetMixFreqFlag(bool flag)
{
    mMixFreqFlag = flag;
}

void
SASViewerProcess2::SetMixNoiseFlag(bool flag)
{
    mMixNoiseFlag = flag;
}

void
SASViewerProcess2::Display()
{
#if !DEBUG_DISPLAY_SCEPSTRUM
    if (mMode == TRACKING)
    {
        DisplayTracking();
    }
    
    if (mMode == AMPLITUDE)
    {
        DisplayAmplitude();
    }
    
    if (mMode == FREQUENCY)
    {
        DisplayFrequency();
    }
    
    if (mMode == COLOR)
    {
        DisplayColor();
    }
    
    if (mMode == WARPING)
    {
        DisplayWarping();
    }
#endif
}

void
SASViewerProcess2::ScaleMagns(WDL_TypedBuf<BL_FLOAT> *magns)
{
    // X
    Scale::ApplyScale(mXScale, magns, (BL_FLOAT)0.0, (BL_FLOAT)(mSampleRate*0.5));
 
    // Y
    for (int i = 0; i < magns->GetSize(); i++)
    {
        BL_FLOAT magn = magns->Get()[i];
        magn = Scale::ApplyScale(mYScale, magn, (BL_FLOAT)MIN_AMP_DB, (BL_FLOAT)0.0);
        magns->Get()[i] = magn;
    }
}

void
SASViewerProcess2::ScalePhases(WDL_TypedBuf<BL_FLOAT> *phases)
{
    // X
    Scale::ApplyScale(mXScale, phases,
                      (BL_FLOAT)0.0, (BL_FLOAT)(mSampleRate*0.5));
}

int
SASViewerProcess2::ScaleFreq(int idx)
{
    BL_FLOAT hzPerBin = mSampleRate/mBufferSize;
    BL_FLOAT freq = idx*hzPerBin;
    
    // Normalize
    freq /= (mSampleRate*0.5);
    
    freq = Scale::ApplyScale(mXScale, freq,
                             (BL_FLOAT)0.0, (BL_FLOAT)(mSampleRate*0.5));
    
    int idx2 = freq*mBufferSize*0.5;

    return idx2;
}

void
SASViewerProcess2::DetectPartials(const WDL_TypedBuf<BL_FLOAT> &magns,
                                 const WDL_TypedBuf<BL_FLOAT> &phases)
{
    mPartialTracker->SetData(magns, phases);
    mPartialTracker->DetectPartials();
    
#if !DEBUG_MUTE_NOISE
    mPartialTracker->ExtractNoiseEnvelope();
#endif
    
    mPartialTracker->FilterPartials();
}

void
SASViewerProcess2::DetectScPartials(const WDL_TypedBuf<BL_FLOAT> &magns,
                                 const WDL_TypedBuf<BL_FLOAT> &phases)
{
    mScPartialTracker->SetData(magns, phases);
    mScPartialTracker->DetectPartials();
    
#if !DEBUG_MUTE_NOISE
    mScPartialTracker->ExtractNoiseEnvelope();
#endif
    
    mScPartialTracker->FilterPartials();
}

void
SASViewerProcess2::IdToColor(int idx, unsigned char color[3])
{
    if (idx == -1)
    {
        color[0] = 255;
        color[1] = 0;
        color[2] = 0;
        
        return;
    }
    
    int r = (678678 + idx*12345)%255;
    int g = (3434345 + idx*123345435345)%255;
    int b = (997867 + idx*12345114222)%255;
    
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void
SASViewerProcess2::PartialToColor(const PartialTracker5::Partial &partial,
                                  unsigned char color[4])
{
    if (partial.mId == -1)
    {
        // Green
        color[0] = 0;
        color[1] = 255;
        color[2] = 0;
        color[3] = 255;
        
        return;
    }
    
    int deadAlpha = 255; //128;
    
#if SHOW_ONLY_ALIVE
    deadAlpha = 0;
#endif
    
    if (partial.mState == PartialTracker5::Partial::ZOMBIE)
    {
        // Green
        color[0] = 255;
        color[1] = 0;
        color[2] = 255;
        color[3] = deadAlpha;
        
        return;
    }
    
    if (partial.mState == PartialTracker5::Partial::DEAD)
    {
        // Green
        color[0] = 255;
        color[1] = 0;
        color[2] = 0;
        color[3] = deadAlpha;
        
        return;
    }
    
    int alpha = 255;
    if (partial.mAge < MIN_AGE_DISPLAY)
    {
        alpha = 0;
    }
    
    IdToColor((int)partial.mId, color);
    color[3] = alpha; //255;
}

void
SASViewerProcess2::DisplayTracking()
{
    if (mSASViewerRender != NULL)
    {
        // Add the magnitudes
        //
        WDL_TypedBuf<BL_FLOAT> magnsScale = mCurrentMagns;
        ScaleMagns(&magnsScale);
        
        mSASViewerRender->AddMagns(magnsScale);
        
        mSASViewerRender->SetLineMode(LinesRender2::LINES_FREQ);
        
        // Add lines corresponding to the well tracked partials
        //
        BL_FLOAT hzPerBin = mSampleRate/mBufferSize;
        
        vector<PartialTracker5::Partial> partials = mCurrentPartials;
        
        // Create blue lines from trackers
        vector<LinesRender2::Point> line;
        for (int i = 0; i < partials.size(); i++)
        {
            const PartialTracker5::Partial &partial = partials[i];
            
            LinesRender2::Point p;
            
            BL_FLOAT partialX = partial.mFreq/hzPerBin;
            partialX = bl_round(partialX);
            
            partialX = ScaleFreq((int)partialX);
            if (mCurrentMagns.GetSize() <= 1)
                p.mX = partialX/mCurrentMagns.GetSize() - 0.5;
            else
                p.mX = partialX/(mCurrentMagns.GetSize() - 1) - 0.5;
            
            // dB for display
            p.mY = BLUtils::DBToAmp(partial.mAmpDB);
            //p.mY = AmpToDBNorm(p.mY);
            p.mY = Scale::ApplyScale(mYScale, p.mY, (BL_FLOAT)MIN_AMP_DB, (BL_FLOAT)0.0);
            
            p.mZ = 0.0;
            
            p.mId = (int)partial.mId;
            
            line.push_back(p);
        }
        
        int numSlices = mSASViewerRender->GetNumSlices();
        int speed = mSASViewerRender->GetSpeed();
        
        bool skipAdd = (mAddNum++ % speed != 0);
        if (!skipAdd)
        {
            // Keep track of the points we pop
            vector<LinesRender2::Point> prevPoints;
            
            mPartialsPoints.push_back(line);
            while(mPartialsPoints.size() > numSlices)
            {
                prevPoints = mPartialsPoints[0];
                
                mPartialsPoints.pop_front();
            }
            
            CreateLines(prevPoints);
            
            // It is cool like that: lite blue with alpha
            //unsigned char color[4] = { 64, 64, 255, 255 };
            
            // Set color
            for (int j = 0; j < mPartialLines.size(); j++)
            {
                LinesRender2::Line &line = mPartialLines[j];
                
                if (!line.mPoints.empty())
                {
                    // Default
                    //line.mColor[0] = color[0];
                    //line.mColor[1] = color[1];
                    //line.mColor[2] = color[2];
                    
                    // Debug
                    IdToColor(line.mPoints[0].mId, line.mColor);
                    
                    line.mColor[3] = 255; // alpha
                }
            }
            
            //BL_FLOAT lineWidth = 4.0;
            BL_FLOAT lineWidth = 1.5;
            mSASViewerRender->SetAdditionalLines(mPartialLines, lineWidth);
        }
        
        mSASViewerRender->ShowAdditionalLines(true);
    }
}

void
SASViewerProcess2::DisplayAmplitude()
{
    BL_FLOAT ampDB;
    if (!mUseSideChain)
        ampDB = mMixSASFrame->GetAmplitudeDB();
    else
        ampDB = mScSASFrame->GetAmplitudeDB();
    
#define Y_COEFF 20.0
#define Y_OFFSET 0.0
    
    BL_FLOAT amp = DBToAmp(ampDB);
    
    WDL_TypedBuf<BL_FLOAT> amps;
    amps.Resize(mBufferSize/2);
    BLUtils::FillAllValue(&amps, amp);
    
    BLUtils::MultValues(&amps, (BL_FLOAT)Y_COEFF);
    BLUtils::AddValues(&amps, (BL_FLOAT)Y_OFFSET);
    
    if (mSASViewerRender != NULL)
    {
        mSASViewerRender->AddMagns(amps);
        
        // WARNING: Won't benefit from straight lines optim
        mSASViewerRender->SetLineMode(LinesRender2::LINES_TIME);
        
        mSASViewerRender->ShowAdditionalLines(false);
    }
}

void
SASViewerProcess2::DisplayFrequency()
{
    BL_FLOAT freq;
    if (!mUseSideChain)
        freq = mMixSASFrame->GetFrequency();
    else
        freq = mScSASFrame->GetFrequency();
    
    BL_FLOAT factor = 3.0;
    freq = BLUtils::LogScaleNormInv(freq, (BL_FLOAT)44100.0, factor);
  
#define Y_COEFF 0.002
#define Y_OFFSET -4.0

    WDL_TypedBuf<BL_FLOAT> freqs;
    freqs.Resize(mBufferSize/2);
    BLUtils::FillAllValue(&freqs, freq);
    
    BLUtils::MultValues(&freqs, (BL_FLOAT)Y_COEFF);
    BLUtils::AddValues(&freqs, (BL_FLOAT)Y_OFFSET);
    
    if (mSASViewerRender != NULL)
    {
        mSASViewerRender->AddMagns(freqs);
        
        // WARNING: Won't benefit from straight lines optim
        mSASViewerRender->SetLineMode(LinesRender2::LINES_TIME);
        
        mSASViewerRender->ShowAdditionalLines(false);
    }
}

void
SASViewerProcess2::DisplayColor()
{
    WDL_TypedBuf<BL_FLOAT> color;
    if (!mUseSideChain)
        mMixSASFrame->GetColor(&color);
    else
        mScSASFrame->GetColor(&color);
    
    ScaleMagns(&color);
    
    BL_FLOAT amplitudeDB;
    if (!mUseSideChain)
        amplitudeDB = mMixSASFrame->GetAmplitudeDB();
    else
        amplitudeDB = mScSASFrame->GetAmplitudeDB();
    
    BL_FLOAT amplitude = DBToAmp(amplitudeDB);
    
    BLUtils::MultValues(&color, amplitude);
    
    // Scale to dB for display
    Scale::ApplyScale(mYScale, &color, (BL_FLOAT)MIN_AMP_DB, (BL_FLOAT)0.0);
    
    if (mSASViewerRender != NULL)
    {
        mSASViewerRender->AddMagns(color);
        mSASViewerRender->SetLineMode(LinesRender2::LINES_FREQ);
        mSASViewerRender->ShowAdditionalLines(false);
    }
}

void
SASViewerProcess2::DisplayWarping()
{
    WDL_TypedBuf<BL_FLOAT> warping;
    
    if (!mUseSideChain)
        mMixSASFrame->GetNormWarping(&warping);
    else
        mScSASFrame->GetNormWarping(&warping);
    
    ScaleMagns(&warping);
    
    BLUtils::AddValues(&warping, (BL_FLOAT)-1.0);
    
#define COEFF 4.0
    BLUtils::MultValues(&warping, (BL_FLOAT)COEFF);
    
    if (mSASViewerRender != NULL)
    {
        mSASViewerRender->AddMagns(warping);
        mSASViewerRender->SetLineMode(LinesRender2::LINES_FREQ);
        mSASViewerRender->ShowAdditionalLines(false);
    }
}

int
SASViewerProcess2::FindIndex(const vector<int> &ids, int idx)
{
    if (idx == -1)
        return -1;
    
    for (int i = 0; i < ids.size(); i++)
    {
        if (ids[i] == idx)
            return i;
    }
    
    return -1;
}

int
SASViewerProcess2::FindIndex(const vector<LinesRender2::Point> &points, int idx)
{
    if (idx == -1)
        return -1;
    
    for (int i = 0; i < points.size(); i++)
    {
        if (points[i].mId == idx)
            return i;
    }
    
    return -1;
}

// Optimized version
void
SASViewerProcess2::CreateLines(const vector<LinesRender2::Point> &prevPoints)
{
    if (mSASViewerRender == NULL)
        return;
    
    if (mPartialsPoints.empty())
        return;
    
    // Update z for the current line points
    //
    int divisor = mSASViewerRender->GetNumSlices() - 1;
    if (divisor <= 0)
        divisor = 1;
    BL_FLOAT incrZ = 1.0/divisor;
    for (int i = 0; i < mPartialLines.size(); i++)
    {
        LinesRender2::Line &line = mPartialLines[i];
        for (int j = 0; j < line.mPoints.size(); j++)
        {
            LinesRender2::Point &p = line.mPoints[j];
            
            p.mZ -= incrZ;
        }
    }
    
    // Shorten the lines if they are too long
    vector<LinesRender2::Line> newLines;
    for (int i = 0; i < mPartialLines.size(); i++)
    {
        const LinesRender2::Line &line = mPartialLines[i];
        
        LinesRender2::Line newLine;
        for (int j = 0; j < line.mPoints.size(); j++)
        {
            const LinesRender2::Point &p = line.mPoints[j];
            if (p.mZ > 0.0)
                newLine.mPoints.push_back(p);
        }
        
        if (!newLine.mPoints.empty())
            newLines.push_back(newLine);
    }
    
    // Update the current partial lines
    mPartialLines = newLines;
    newLines.clear();
    
    // Create the new lines
    const vector<LinesRender2::Point> &newPoints =
                    mPartialsPoints[mPartialsPoints.size() - 1];
    for (int i = 0; i < newPoints.size(); i++)
    {
        LinesRender2::Point newPoint = newPoints[i];
        newPoint.mZ = 1.0;
        
        bool pointAdded = false;
        
        // Search for previous lines to be continued
        for (int j = 0; j < mPartialLines.size(); j++)
        {
            LinesRender2::Line &prevLine = mPartialLines[j];
        
            if (!prevLine.mPoints.empty())
            {
                int lineIdx = prevLine.mPoints[0].mId;
                
                if (lineIdx == newPoint.mId)
                {
                    // Add the point to prev line
                    prevLine.mPoints.push_back(newPoint);
                    
                    // We are done
                    pointAdded = true;
                    
                    break;
                }
            }
        }
        
        // Create a new line ?
        if (!pointAdded)
        {
            LinesRender2::Line newLine;
            newLine.mPoints.push_back(newPoint);
            
            mPartialLines.push_back(newLine);
        }
    }
}

// ioBuffer contains noise in input
// and the result as output
void
SASViewerProcess2::MixHarmoNoise(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                const WDL_TypedBuf<BL_FLOAT> &harmoBuffer)
{
    BL_FLOAT noiseCoeff;
    BL_FLOAT harmoCoeff;
    BLUtils::MixParamToCoeffs(mNoiseMix, &noiseCoeff, &harmoCoeff);
    
    WDL_TypedBuf<BL_FLOAT> newBuf;
    newBuf.Resize(ioBuffer->GetSize());
    for (int i = 0; i < newBuf.GetSize(); i++)
    {
        BL_FLOAT n = ioBuffer->Get()[i];
        BL_FLOAT h = harmoBuffer.Get()[i];
        
        BL_FLOAT val = n*noiseCoeff + h*harmoCoeff;
        newBuf.Get()[i] = val;
    }
    
    *ioBuffer = newBuf;
}

void
SASViewerProcess2::MixFrames(SASFrame3 *result,
                            const SASFrame3 &frame0,
                            const SASFrame3 &frame1,
                            BL_FLOAT t)
{
    // Amp
    BL_FLOAT amp0 = frame0.GetAmplitudeDB();
    BL_FLOAT amp1 = frame1.GetAmplitudeDB();
    BL_FLOAT resultAmp = (1.0 - t)*amp0 + t*amp1;
    result->SetAmplitudeDB(resultAmp);
    
    // Freq
    if (mMixFreqFlag)
    {
        BL_FLOAT freq0 = frame0.GetFrequency();
        BL_FLOAT freq1 = frame1.GetFrequency();
        BL_FLOAT resultFreq = (1.0 - t)*freq0 + t*freq1;

        result->SetFrequency(resultFreq);
    }
    else
    {
        BL_FLOAT freq0 = frame0.GetFrequency();
        result->SetFrequency(freq0);
    }
    
    // Color
    WDL_TypedBuf<BL_FLOAT> color0;
    frame0.GetColor(&color0);
    
    WDL_TypedBuf<BL_FLOAT> color1;
    frame1.GetColor(&color1);
    
    WDL_TypedBuf<BL_FLOAT> resultColor;
    BLUtils::Interp(&resultColor, &color0, &color1, t);
    
    result->SetColor(resultColor);
    
    // Warping
    WDL_TypedBuf<BL_FLOAT> warp0;
    frame0.GetNormWarping(&warp0);
    
    WDL_TypedBuf<BL_FLOAT> warp1;
    frame1.GetNormWarping(&warp1);
    
    WDL_TypedBuf<BL_FLOAT> resultWarping;
    BLUtils::Interp(&resultWarping, &warp0, &warp1, t);
    
    result->SetNormWarping(resultWarping);
}

#endif // IGRAPHICS_NANOVG
