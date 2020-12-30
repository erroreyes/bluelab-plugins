//
//  SpectroExpeFftObj.cpp
//  BL-SpectroExpe
//
//  Created by Pan on 02/06/18.
//
//

#ifdef IGRAPHICS_NANOVG

#include <BLSpectrogram4.h>
#include <BLUtils.h>
#include <BLDebug.h>

#include <SpectrogramDisplay.h>
#include <SpectrogramDisplayScroll3.h>

#include "SpectroExpeFftObj.h"

#define USE_AVG_LINES 0 //1

SpectroExpeFftObj::SpectroExpeFftObj(int bufferSize, int oversampling, int freqRes,
                                     BL_FLOAT sampleRate)
: MultichannelProcess()
{
    mSpectrogram = new BLSpectrogram4(sampleRate, bufferSize/4, -1);
    mSpectroDisplay = NULL;
    
    MultichannelProcess::Reset(bufferSize, oversampling, freqRes, sampleRate);
    
    mLineCount = 0;
    
    mSpeedMod = 1;
}

SpectroExpeFftObj::~SpectroExpeFftObj()
{
    delete mSpectrogram;
}

void
SpectroExpeFftObj::ProcessInputFft(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *ioFftSamples,
				   const vector<WDL_TypedBuf<WDL_FFT_COMPLEX> > *scBuffer)
{
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples[2];
    fftSamples[0] = *(*ioFftSamples)[0];
    fftSamples[1] = *(*ioFftSamples)[1];
    
    WDL_TypedBuf<BL_FLOAT> magns[2];
    WDL_TypedBuf<BL_FLOAT> phases[2];
    
    for (int i = 0; i < 2; i++)
    {
        BLUtils::TakeHalf(&fftSamples[i]);
        BLUtils::ComplexToMagnPhase(&magns[i], &phases[i], fftSamples[i]);
    }
    
    if (mLineCount % mSpeedMod == 0)
      AddSpectrogramLine(magns[0], phases[0]);
    
    mLineCount++;
}

void
SpectroExpeFftObj::Reset(int bufferSize, int oversampling,
                         int freqRes, BL_FLOAT sampleRate)
{
    MultichannelProcess::Reset(bufferSize, oversampling,
                               freqRes, sampleRate);
    
    // ORIGIN
    //int numCols = mBufferSize/8;
    
    // TODO: modify this in SpectroExpe too
    // NEW
    // Prefer this, so the scroll speed won't be modified when
    // the overlapping changes
    int numCols = bufferSize/(32/oversampling);
    
    // Adjust to the sample rate to avoid scrolling
    // 2 times faster when we go from 44100 to 88200
    //
    BL_FLOAT srCoeff = sampleRate/44100.0;
    srCoeff = bl_round(srCoeff);
    numCols *= srCoeff;
    
    mSpectrogram->Reset(sampleRate, bufferSize/4, numCols);
    
    mLineCount = 0;
    
    mOverlapLines.clear();
}

BLSpectrogram4 *
SpectroExpeFftObj::GetSpectrogram()
{
    return mSpectrogram;
}

void
SpectroExpeFftObj::SetSpectrogramDisplay(SpectrogramDisplayScroll3 *spectroDisplay)
{
    mSpectroDisplay = spectroDisplay;
}

void
SpectroExpeFftObj::SetSpeedMod(int speedMod)
{
    mSpeedMod = speedMod;
}

void
SpectroExpeFftObj::AddSpectrogramLine(const WDL_TypedBuf<BL_FLOAT> &magns,
                                      const WDL_TypedBuf<BL_FLOAT> &phases)
{
#if USE_AVG_LINES
    
#define USE_SIMPLE_AVG 1

    // Take the 4 last lines at the maximum
    // (otherwise it is too blurry)
#define MAX_LINES 2 //4
    
    int maxLines = mOverlapping;
    if (maxLines > MAX_LINES)
        maxLines = MAX_LINES;
        
    // Keep track of previous lines
    // For correctly display, with overlapping
    mOverlapLines.push_back(magns);
    if (mOverlapLines.size() > maxLines)
        mOverlapLines.pop_front();
    
    // Simply make the average of the previous lines
    WDL_TypedBuf<BL_FLOAT> line;
    BLUtils::ResizeFillZeros(&line, magns.GetSize());
    
    for (int i = 0; i < mOverlapLines.size(); i++)
    {
        WDL_TypedBuf<BL_FLOAT> currentLine = mOverlapLines[i];
        
#if !USE_SIMPLE_AVG
        // Multiply by a coeff to smooth
        BL_FLOAT coeff = (i + 1.0)/mOverlapLines.size();
        
        // Adjust, for MAX_LINES == 4
        coeff /= 2.5;
        
        BLUtils::MultValues(&currentLine, coeff);
#endif
        
        BLUtils::AddValues(&line, currentLine);
    }
    
#if USE_SIMPLE_AVG
    if (mOverlapping > 0)
        // Just in case
        BLUtils::MultValues(&line, 1.0/maxLines);
#endif
    
    mSpectrogram->AddLine(line, phases);

#else // Simple add
    if (mSpectroDisplay != NULL)
    {
        mSpectroDisplay->AddSpectrogramLine(magns, phases);
    }
    else
        mSpectrogram->AddLine(magns, phases);
#endif
}

#endif // IGRAPHICS_NANOVG
