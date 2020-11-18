//
//  PartialTracker5.cpp
//  BL-SASViewer
//
//  Created by applematuer on 2/2/19.
//
//

#include <algorithm>
using namespace std;

#include <SASViewerProcess.h>
#include <FreqAdjustObj3.h>

#include <Window.h>

#include <BLUtils.h>
#include <BLDebug.h>

#include "PartialTracker5.h"

#define MIN_AMP_DB -120.0
//#define MIN_NORM_AMP 0.001
#define MIN_NORM_AMP 1e-15

#define PARTIALS_HISTORY_SIZE 2

// Makes wobbling frequencies
// NOTE: not adapted at all when input magns are already scaled to mel
#define USE_FREQ_OBJ 0


// Detect partials
//
#define DETECT_PARTIALS_START_INDEX 2

#define DETECT_PARTIALS_SMOOTH 0 // 1
// Take odd size ! (otherwise there will be a shift on the right)
//
// 5: good, but very high freqs are not well detected
// 3: detect well high frequencies, but dos not smooth enough low freqs
//
#define DETECT_PARTIALS_SMOOTH_SIZE 5


// Compute simple, avg, or parabola ?
// (Avg seems a little more smooth than parabola)
#define COMPUTE_PEAKS_AVG      1 // Good even for flat partial top
#define COMPUTE_PEAKS_PARABOLA 0 // Good, but process flat partial tops badly
#define COMPUTE_PEAKS_SIMPLE   0 //1 // NEW

// With 1, that made more defined partials
// With 0, avoids partial leaking in noise ("oohoo")
#define NARROW_PARTIAL_FOOT 0 //1 //0
#define NARROW_PARTIAL_FOOT_COEFF 20.0 //40.0

#define DISCARD_FLAT_PARTIAL 1
#define DISCARD_FLAT_PARTIAL_COEFF 25000.0 //30000.0

#define DISCARD_INVALID_PEAKS 1

// Threshold partials
//
#define THRESHOLD_MIN               0 // good (but makes "noise blur")
#define THRESHOLD_PEAK_PROMINENCE   0 // better (but suppresses partial in case of "double head")
#define THRESHOLD_PEAK_HEIGHT_DB    0 // EXPE: not working
#define THRESHOLD_PEAK_HEIGHT_PINK  0 // EXPE: not working

// ORIGIN
#define THRESHOLD_PEAK_HEIGHT       1 //0 //1 // better: fixes "double-heads" problem
// NEW
#define THRESHOLD_FILTERED_PARTIALS 0 //1 //0

// Experimental
#define THRESHOLD_SMOOTH          0
#define THRESHOLD_AUTO            0 // Not working

#define THRESHOLD_PARTIALS_SMOOTH_SIZE   21
#define THRESHOLD_PARTIALS_AUTO_WIN_SIZE 21

//
#define GLUE_BARBS              1 //0
#define GLUE_BARBS_AMP_RATIO    10.0 //4.0


#define GLUE_TWIN_PARTIALS 0


// Filter
//

// Do we filter ?
#define FILTER_PARTIALS 1

// Do we smooth when filter ?
#define FILTER_SMOOTH       0 //1
#define FILTER_SMOOTH_COEFF 0.9 //0.8

#define MAX_ZOMBIE_AGE 2

// Seems better with 200Hz (tested on "oohoo")
#define MAX_FREQ_DIFF_ASSOC 0.01 // For normalized freqs. Around 100Hz
// 0.01 => gives good result (avoids "tss" detected as partial)
// 0.005 => avoids missing partials in heigh frequencies
//#define MIN_SPACING_NORM_MEL 0.01
//#define MIN_SPACING_NORM_MEL 0.005
#define MIN_SPACING_NORM_MEL 0.001 // in test...

// Minimum spacing between partials in bins
#define APPLY_MIN_SPACING 0
#define MIN_SPACING_BINS  0 //16

// Kalman
// "How much do we expect to our measurement vary"
//
// 200Hz (previously tested with 50Hz)
#define KF_E_MEA 0.01 // 200.0Hz
#define KF_E_EST KF_E_MEA

// "usually a small number between 0.001 and 1"
//
// If too low: predicted values move too slowly
// If too high: predicted values go straight
//
// 0.01: "oohoo" => fails when "EEAAooaa"
#define KF_Q 5.0 //2.0 // Was 1.0

// Seems better without, not sure...
#define USE_KALMAN_FOR_ASSOC 0 //1 //0
// GOOD: avoid many small zig-zags on the result ines
#define USE_KALMAN_FOR_RESULT 1

// Extract noise envelope
//
#define EXTRACT_NOISE_ENVELOPE_MAX    0
#define EXTRACT_NOISE_ENVELOPE_TRACK  0 // Prev
#define EXTRACT_NOISE_ENVELOPE_SIMPLE 1

#define DEBUG_DONT_FILL_NOISE_ENV     0
//#define DEBUG_DONT_FILL_NOISE_ENV   1 // TEST for Air

// Expe
// 1 gives good results for "Ooohoo" (method "Min")
// 2 gives good results for "Ti Tsu Koi" (method "Min")
#define NUM_ITER_EXTRACT_NOISE 4 //1

#define PROCESS_MUS_NOISE      1 // Working (but dos not remove all mus noise)
// 4 seems better than 2
#define HISTORY_SIZE_MUS_NOISE 4 //2 //4

// Take isles in the noise envelope, and suppress them if they are too big
// Interesting, but will need some sort of adaptive threshold
// Example: "oohoo"
#define THRESHOLD_NOISE_ISLES 0

// NOTE: See: https://www.dsprelated.com/freebooks/sasp/Spectral_Modeling_Synthesis.html
//
// and: https://www.dsprelated.com/freebooks/sasp/PARSHL_Program.html#app:parshlapp
//
// and: https://ccrma.stanford.edu/~jos/parshl/

// Since PartialTracker4, for Infra
#define FIX_COMPUTE_PARTIAL_AMP 1


unsigned long PartialTracker5::Partial::mCurrentId = 0;


PartialTracker5::Partial::Partial()
: mKf(KF_E_MEA, KF_E_EST, KF_Q)
{
    mPeakIndex = 0;
    mLeftIndex = 0;
    mRightIndex = 0;
    
    mFreq = 0.0;
    mAmp = 0.0;
    
    mPhase = 0.0;
    
    mState = ALIVE;
    
    mId = -1;
    
    mWasAlive = false;
    mZombieAge = 0;
    
    mAge = 0;
    
    mCookie = 0.0;
    
    // Kalman
    mPredictedFreq = 0.0;
}

    
PartialTracker5::Partial::Partial(const Partial &other)
: mKf(other.mKf)
{
    mPeakIndex = other.mPeakIndex;
    mLeftIndex = other.mLeftIndex;
    mRightIndex = other.mRightIndex;
    
    mFreq = other.mFreq;
    mAmp = other.mAmp;
    
    mPhase = other.mPhase;
        
    mState = other.mState;;
        
    mId = other.mId;
    
    mWasAlive = other.mWasAlive;
    mZombieAge = other.mZombieAge;
    
    mAge = other.mAge;
    
    mCookie = other.mCookie;
    
    // Kalman
    mPredictedFreq = other.mPredictedFreq;
}

PartialTracker5::Partial::~Partial() {}

void
PartialTracker5::Partial::GenNewId()
{
    mId = mCurrentId++;
}
    
bool
PartialTracker5::Partial::FreqLess(const Partial &p1, const Partial &p2)
{
    return (p1.mFreq < p2.mFreq);
}

bool
PartialTracker5::Partial::AmpLess(const Partial &p1, const Partial &p2)
{
    return (p1.mAmp < p2.mAmp);
}

bool
PartialTracker5::Partial::IdLess(const Partial &p1, const Partial &p2)
{
    return (p1.mId < p2.mId);
}

bool
PartialTracker5::Partial::CookieLess(const Partial &p1, const Partial &p2)
{
    return (p1.mCookie < p2.mCookie);
}

PartialTracker5::PartialTracker5(int bufferSize, BL_FLOAT sampleRate,
                                 BL_FLOAT overlapping)
{
    mBufferSize = bufferSize;
    mSampleRate = sampleRate;
    mOverlapping = overlapping;
    
    mThreshold = -60.0;
    
    mFreqObj = NULL;
    
#if USE_FREQ_OBJ
    int freqRes = 1;
    mFreqObj = new FreqAdjustObj3(bufferSize, overlapping,
                                  freqRes, sampleRate);
#endif
    
    mMaxDetectFreq = -1.0;
    
    mDbgParam = 1.0;
}

PartialTracker5::~PartialTracker5()
{
    if (mFreqObj != NULL)
        delete mFreqObj;
}

void
PartialTracker5::Reset()
{
    if (mFreqObj != NULL)
    {
        int freqRes = 1;
        mFreqObj->Reset(mBufferSize, mOverlapping,
                        freqRes, mSampleRate);
    }
    
    mPartials.clear();
    mResult.clear();
    
    mNoiseEnvelope.Resize(0);
    mHarmonicEnvelope.Resize(0);;
    
    mRealFreqs.Resize(0);
    mPrevMagns.Resize(0);

    mCurrentMagns.Resize(0);
    mCurrentPhases.Resize(0);
    
    mSmoothWinDetect.Resize(0);
    mCurrentSmoothMagns.Resize(0);
    
    mSmoothWinThreshold.Resize(0);
    mSmoothWinNoise.Resize(0);
    

    mPrevNoiseEnvelope.Resize(0);
    // For ComputeMusicalNoise()
    mPrevNoiseMasks.clear();
}

#if FIX_INFRA_SAMPLERATE
void
PartialTracker5::Reset(int bufferSize, BL_FLOAT sampleRate)
{
    mBufferSize = bufferSize;
    mSampleRate = sampleRate;
    
    Reset();
}
#endif

void
PartialTracker5::SetThreshold(BL_FLOAT threshold)
{
    mThreshold = threshold;
}

void
PartialTracker5::SetData(const WDL_TypedBuf<BL_FLOAT> &magns,
                         const WDL_TypedBuf<BL_FLOAT> &phases)
{
    mCurrentMagns = magns;
    mCurrentPhases = phases;
}

void
PartialTracker5::DetectPartials()
{
    WDL_TypedBuf<BL_FLOAT> magns0 = mCurrentMagns;
    
    if (mFreqObj != NULL)
    {
        // See: http://blogs.zynaptiq.com/bernsee/pitch-shifting-using-the-ft/
        mFreqObj->ComputeRealFrequencies(mCurrentPhases, &mRealFreqs);
    }
    
    vector<Partial> partials;
    DetectPartials(magns0, mCurrentPhases, &partials);
    
    SuppressZeroFreqPartials(&partials);
    
    // Some operations
#if GLUE_BARBS
    vector<Partial> prev = partials;
    
    GluePartialBarbs(magns0, &partials);
#endif
    
#if NARROW_PARTIAL_FOOT
    NarrowPartialFoot(magns0, &partials);
#endif
    
#if DISCARD_FLAT_PARTIAL
    DiscardFlatPartials(magns0, &partials);
#endif
    
    // NEW
    ComputePeaksHeights(mCurrentMagns, &partials);
    
    // Threshold
    //
    
    // Remove small partials before gluing the twins
    // => this avoids gluing many small partials together
#if THRESHOLD_MIN
    ThresholdPartialsAmp(&partials);
#endif

#if THRESHOLD_PEAK_PROMINENCE
    ThresholdPartialsPeakProminence(magns0, &partials);
#endif

#if THRESHOLD_PEAK_HEIGHT
    ThresholdPartialsPeakHeight(/*magns0,*/ &partials);
#endif

#if THRESHOLD_PEAK_HEIGHT_DB
    ThresholdPartialsPeakHeightDb(magns0, &partials);
#endif

#if THRESHOLD_PEAK_HEIGHT_PINK
    ThresholdPartialsPeakHeightPink(magns0, &partials);
#endif

#if THRESHOLD_SMOOTH
    ThresholdPartialsAmpSmooth(magns0, &partials);
#endif

#if THRESHOLD_AUTO
    ThresholdPartialsAmpAuto(magns0, &partials);
#endif
    
    // "tibetan bell": avoids keeping a partial at 2KHz in the noise envelope
    //
    // With "bell" => we loose the first partial
    // (glued with the second)
#if 0
    GlueTwinPartials(magns0, &partials);
#endif
    
    mPartials.push_front(partials);
    
    while(mPartials.size() > PARTIALS_HISTORY_SIZE)
        mPartials.pop_back();
    
    mResult = partials;
}

void
PartialTracker5::ExtractNoiseEnvelope()
{
#if EXTRACT_NOISE_ENVELOPE_MAX
    ExtractNoiseEnvelopeMax();
#endif

#if EXTRACT_NOISE_ENVELOPE_TRACK
    ExtractNoiseEnvelopeTrack();
#endif
    
#if EXTRACT_NOISE_ENVELOPE_SIMPLE
    ExtractNoiseEnvelopeSimple();
#endif
}

void
PartialTracker5::ExtractNoiseEnvelopeMax()
{
    // If not partial detected, noise envelope is simply the input magns
    mNoiseEnvelope = mCurrentMagns;
    
    if (mPartials.empty())
        return;
    
    // NOTE: may be better in the loop
    // (to avoid special case for first iteration)
    CutPartials(mPartials[0], &mNoiseEnvelope);
    
    // Iterate
    WDL_TypedBuf<BL_FLOAT> dummyPhases;
    BLUtils::ResizeFillZeros(&dummyPhases, mNoiseEnvelope.GetSize());
    for (int i = 0; i < NUM_ITER_EXTRACT_NOISE - 1; i++)
    {
        vector<Partial> partials;
        DetectPartials(mNoiseEnvelope, dummyPhases, &partials);
        
        CutPartials(partials, &mNoiseEnvelope);
    }
}

void
PartialTracker5::ExtractNoiseEnvelopeTrack()
{
    // If not partial detected, noise envelope is simply the input magns
    mNoiseEnvelope = mCurrentMagns;
    
    vector<Partial> partials;
    
    // Get all partials, or only alive ?
    //partials = mResult; // Not so good, makes noise peaks
    
    // NOTE: Must get the alive partials only,
    // otherwise we would get additional "garbage" partials,
    // that would corrupt the partial rectangle
    // and then compute incorrect noise peaks
    // (the state must be ALIVE, and not mWasAlive !)
    GetAlivePartials(&partials);
    
    // "tibetan bell": avoids keeping a partial at 2KHz in the noise envelope
    // => makes a better noise envelope !
#if GLUE_TWIN_PARTIALS
    GlueTwinPartials(mCurrentMagns, &partials);
#endif
    
    CutPartials(partials, &mNoiseEnvelope);
    // CutPartialsMinEnv(&mNoiseEnvelope);
    
    // Compute harmonic envelope
    // (origin signal less noise)
    mHarmonicEnvelope = mCurrentMagns;
    BLUtils::SubstractValues(&mHarmonicEnvelope, mNoiseEnvelope);
    
    BLUtils::ClipMin(&mHarmonicEnvelope, (BL_FLOAT)0.0);
}

void
PartialTracker5::ExtractNoiseEnvelopeSimple()
{
    vector<Partial> partials;
    
    // Get all partials, or only alive ?
    //partials = mResult; // Not so good, makes noise peaks
    
    // NOTE: Must get the alive partials only,
    // otherwise we would get additional "garbage" partials,
    // that would corrupt the partial rectangle
    // and then compute incorrect noise peaks
    // (the state must be ALIVE, and not mWasAlive !)
    GetAlivePartials(&partials);
    
    mHarmonicEnvelope = mCurrentMagns;
    
    // Just in case
    for (int i = 0; i < DETECT_PARTIALS_START_INDEX; i++)
        mHarmonicEnvelope.Get()[i] = MIN_NORM_AMP;
    
    KeepOnlyPartials(partials, &mHarmonicEnvelope);
    
    // Compute harmonic envelope
    // (origin signal less noise)
    mNoiseEnvelope = mCurrentMagns;
    
    BLUtils::SubstractValues(&mNoiseEnvelope, mHarmonicEnvelope);
    
    // Because it is in dB
    BLUtils::AddValues(&mNoiseEnvelope, (BL_FLOAT)0.0);
    
    BLUtils::ClipMin(&mNoiseEnvelope, (BL_FLOAT)0.0);
    
    // BUG: some small spots remain ("alphabet, A")
    // TODO: re-inject the remaining in the harmonic envelope
    //ZeroToNextNoiseMinimum(&mNoiseEnvelope);
    
    // Avoids interpolation from 0 to the first valid index
    // (could have made an artificial increasing slope in the low freqs)
    for (int i = 0; i < mNoiseEnvelope.GetSize(); i++)
    {
        BL_FLOAT val = mNoiseEnvelope.Get()[i];
        if (val > BL_EPS)
            // First value
        {
            int prevIdx = i - 1;
            if (prevIdx > 0)
            {
                mNoiseEnvelope.Get()[prevIdx] = BL_EPS;
            }
            
            break;
        }
    }
    
#if THRESHOLD_NOISE_ISLES 
    // Threshold peak isles
    ThresholdNoiseIsles(&mNoiseEnvelope);
#endif
    
#if PROCESS_MUS_NOISE
    // Works well, but do not remove all musical noise
    ProcessMusicalNoise(&mNoiseEnvelope);
#endif
    
#if !DEBUG_DONT_FILL_NOISE_ENV
    // Create an envelope
    // NOTE: good for "oohoo", not good for "alphabet A"
    BLUtils::FillMissingValues2(&mNoiseEnvelope, false, (BL_FLOAT)0.0);
#endif
    
    //SmoothNoiseEnvelope(&mNoiseEnvelope);
    //SmoothNoiseEnvelopeTime(&mNoiseEnvelope);
}

// Supress musical noise in the raw noise (not filled)
void
PartialTracker5::ProcessMusicalNoise(WDL_TypedBuf<BL_FLOAT> *noise)
{
// Must choose bigger value than 1e-15
// (otherwise the threshold won't work)
#define MUS_NOISE_EPS 1e-8
    
    // Better with an history
    // => suppress only spots that are in zone where partials are erased
    
    // If history is small => remove more spots (but unwanted ones)
    // If history too big => keep some spots that should have been erased
    if (mPrevNoiseMasks.size() < HISTORY_SIZE_MUS_NOISE)
    {
        mPrevNoiseMasks.push_back(*noise);
        if (mPrevNoiseMasks.size() > HISTORY_SIZE_MUS_NOISE)
            mPrevNoiseMasks.pop_front();
        
        return;
    }
    
    const WDL_TypedBuf<BL_FLOAT> noiseCopy = *noise;
    
    // Search for begin of first isle: values with zero borders
    //
    int startIdx = 0;
    while (startIdx < noise->GetSize())
    {
        BL_FLOAT val = noise->Get()[startIdx];
        if (val < MUS_NOISE_EPS)
            // Zero
        {
            // Adjust the index to the last zero value
            //if (startIdx >= 0)
            //    startIdx--;
            
            break;
        }
        
        startIdx++;
    }
    
    // Loop to search for isles
    //
    while(startIdx < noise->GetSize())
    {
        // Find "isles" in the current noise
        int startIdxIsle = startIdx;
        while (startIdxIsle < noise->GetSize())
        {
            BL_FLOAT val = noise->Get()[startIdxIsle];
            if (val > MUS_NOISE_EPS)
            // One
            {
                // Start of isle found
                break;
            }
            
            startIdxIsle++;
        }
        
        // Search for isles: values with zero borders
        int endIdxIsle = startIdxIsle;
        while (endIdxIsle < noise->GetSize())
        {
            BL_FLOAT val = noise->Get()[endIdxIsle];
            if (val < MUS_NOISE_EPS)
            // Zero
            {
                // End of isle found
                
                // Adjust the index to the last zero value
                if (endIdxIsle > startIdxIsle)
                    endIdxIsle--;
                
                break;
            }
            
            endIdxIsle++;
        }
        
        // Check that the prev mask is all zero
        // at in front of the isle
        bool prevMaskZero = true;
        for (int i = 0; i < mPrevNoiseMasks.size(); i++)
        {
            const WDL_TypedBuf<BL_FLOAT> &mask = mPrevNoiseMasks[i];
            
            for (int j = startIdxIsle; j <= endIdxIsle; j++)
            {
                BL_FLOAT prevVal = mask.Get()[j];
                if (prevVal > MUS_NOISE_EPS)
                {
                    prevMaskZero = false;
                
                    break;
                }
            }
            
            if (!prevMaskZero)
                break;
        }
        
        if (prevMaskZero)
        // We have a real isle
        {
            // TODO: check isle size ?
            
            // Earse the isle
            for (int i = startIdxIsle; i <= endIdxIsle; i++)
            {
                noise->Get()[i] = 0.0;
            }
        }
        
        startIdx = endIdxIsle + 1;
    }
    
    // Fill the history at the end
    // (to avoid having processing the current noise as history
    mPrevNoiseMasks.push_back(noiseCopy);
    if (mPrevNoiseMasks.size() > HISTORY_SIZE_MUS_NOISE)
        mPrevNoiseMasks.pop_front();
}

void
PartialTracker5::ThresholdNoiseIsles(WDL_TypedBuf<BL_FLOAT> *noise)
{
    // Must choose bigger value than 1e-15
    // (otherwise the threshold won't work)
#define ISLE_THRS_EPS 1e-8
    
#define ISLE_THRESHOLD -60.0
    
    // Search for begin of first isle: values with zero borders
    //
    int startIdx = 0;
    while (startIdx < noise->GetSize())
    {
        BL_FLOAT val = noise->Get()[startIdx];
        if (val < ISLE_THRS_EPS)
            // Zero
        {
            break;
        }
        
        startIdx++;
    }
    
    // Loop to search for isles
    //
    while(startIdx < noise->GetSize())
    {
        // Find "isles" in the current noise
        int startIdxIsle = startIdx;
        while (startIdxIsle < noise->GetSize())
        {
            BL_FLOAT val = noise->Get()[startIdxIsle];
            if (val > ISLE_THRS_EPS)
                // One
            {
                // Start of isle found
                break;
            }
            
            startIdxIsle++;
        }
        
        // Search for isles: values with zero borders
        int endIdxIsle = startIdxIsle;
        while (endIdxIsle < noise->GetSize())
        {
            BL_FLOAT val = noise->Get()[endIdxIsle];
            if (val < ISLE_THRS_EPS)
                // Zero
            {
                // End of isle found
                
                // Adjust the index to the last zero value
                if (endIdxIsle > startIdxIsle)
                    endIdxIsle--;
                
                break;
            }
            
            endIdxIsle++;
        }
        
        // Threshold
        BL_FLOAT maxVal = -BL_INF;
        for (int i = startIdxIsle; i <= endIdxIsle; i++)
        {
            BL_FLOAT val = noise->Get()[i];
            if (val > maxVal)
                maxVal = val;
        }
        
        if (maxVal > ISLE_THRESHOLD)
        // Above the threshold
        {
            // Suppress the isle
            for (int i = startIdxIsle; i <= endIdxIsle; i++)
            {
                noise->Get()[i] = 0.0;
            }
        }
        
        startIdx = endIdxIsle + 1;
    }
}

// Set to 0 until the next minimum
// Avoids "half partials" at the beginning that would
// not be detected, and finish in the noise envelope
void
PartialTracker5::ZeroToNextNoiseMinimum(WDL_TypedBuf<BL_FLOAT> *noise)
{
    if (noise->GetSize() == 0)
        return;
    
    BL_FLOAT prevVal = noise->Get()[0];
    for (int i = 0; i < noise->GetSize(); i++)
    {
        BL_FLOAT val = noise->Get()[i];
        
        if (i < DETECT_PARTIALS_START_INDEX)
        {
            noise->Get()[i] = 0.0;
        }
        else
        {
            if (val > prevVal)
            {
                // We are just after the minimum. Stop.
                break;
            }
            
            noise->Get()[i] = 0.0;
        }
        
        prevVal = val;
    }
}

void
PartialTracker5::SmoothNoiseEnvelope(WDL_TypedBuf<BL_FLOAT> *noise)
{
#define NOISE_SMOOTH_WIN_SIZE 27 //9
    
    // Get a smoothed version of the magns
    if (mSmoothWinNoise.GetSize() != NOISE_SMOOTH_WIN_SIZE)
    {
        //Window::MakeHanning(NOISE_SMOOTH_WIN_SIZE, &mSmoothWinNoise);
        
        // Works well too
        //
        // See: https://en.wikipedia.org/wiki/Window_function
        //
        BL_FLOAT sigma = 0.1;
        Window::MakeGaussian2(sigma, NOISE_SMOOTH_WIN_SIZE, &mSmoothWinNoise);
    }
    
    WDL_TypedBuf<BL_FLOAT> smoothNoise;
    BLUtils::SmoothDataWin(&smoothNoise, *noise, mSmoothWinNoise);
    
    *noise = smoothNoise;
}

void
PartialTracker5::SmoothNoiseEnvelopeTime(WDL_TypedBuf<BL_FLOAT> *noise)
{
#define NOISE_SMOOTH_TIME_COEFF 0.5
    
    if (mPrevNoiseEnvelope.GetSize() != noise->GetSize())
    {
        mPrevNoiseEnvelope = *noise;
        
        return;
    }
    
    for (int i = 0; i < noise->GetSize(); i++)
    {
        BL_FLOAT val = noise->Get()[i];
        BL_FLOAT prevVal = mPrevNoiseEnvelope.Get()[i];
        
        BL_FLOAT newVal = (1.0 - NOISE_SMOOTH_TIME_COEFF)*val + NOISE_SMOOTH_TIME_COEFF*prevVal;
        
        noise->Get()[i] = newVal;
    }
    
    mPrevNoiseEnvelope = *noise;
}

void
PartialTracker5::CutPartials(const vector<Partial> &partials,
                            WDL_TypedBuf<BL_FLOAT> *magns)
{
    // Take each partial, and "cut" it from the full data
    for (int i = 0; i < partials.size(); i++)
    {
        const Partial &partial = partials[i];
        
        int minIdx = partial.mLeftIndex;
        if (minIdx >= magns->GetSize())
            continue;
        
        int maxIdx = partial.mRightIndex;
        if (maxIdx >= magns->GetSize())
            continue;
        
        BL_FLOAT minVal = magns->Get()[minIdx];
        BL_FLOAT maxVal = magns->Get()[maxIdx];
        
        int numIdx = maxIdx - minIdx + 1;
        for (int j = 0; j < numIdx; j++)
        {
            BL_FLOAT t = 0.0;
            if (numIdx > 1)
                t = ((BL_FLOAT)j)/(numIdx - 1);
            
            BL_FLOAT val = (1.0 - t)*minVal + t*maxVal;
            if (minIdx + j >= magns->GetSize())
                continue;
            
            magns->Get()[minIdx + j] = val;
        }
    }
}

void
PartialTracker5::KeepOnlyPartials(const vector<Partial> &partials,
                                  WDL_TypedBuf<BL_FLOAT> *magns)
{
    WDL_TypedBuf<BL_FLOAT> result;
    BLUtils::ResizeFillValue(&result, magns->GetSize(), (BL_FLOAT)0.0);
                   
    for (int i = 0; i < partials.size(); i++)
    {
        const Partial &partial = partials[i];
        
        int minIdx = partial.mLeftIndex;
        if (minIdx >= magns->GetSize())
            continue;
        
        int maxIdx = partial.mRightIndex;
        if (maxIdx >= magns->GetSize())
            continue;
        
        for (int j = minIdx; j <= maxIdx; j++)
        {
            BL_FLOAT val = magns->Get()[j];
            result.Get()[j] = val;
        }
    }
                           
    *magns = result;
}

// Compute the envelope of the minima
void
PartialTracker5::CutPartialsMinEnv(WDL_TypedBuf<BL_FLOAT> *magns)
{
    // Detect the minima
    vector<int> minIndices;
    for (int i = 1; i < magns->GetSize() - 1; i++)
    {
        BL_FLOAT val0 = magns->Get()[i - 1];
        BL_FLOAT val1 = magns->Get()[i];
        BL_FLOAT val2 = magns->Get()[i + 1];
        
        if ((val1 < val0) && (val1 < val2))
        // Minimum detected
        {
            minIndices.push_back(i);
        }
    }

    // Compute envelope
    WDL_TypedBuf<BL_FLOAT> minEnv;
    BLUtils::ResizeFillZeros(&minEnv, magns->GetSize());
    
    for (int i = 0; i < minIndices.size(); i++)
    {
        int idx = minIndices[i];
        BL_FLOAT val = magns->Get()[idx];
        
        minEnv.Get()[idx] = val;
    }
    
    BLUtils::FillMissingValues(&minEnv, false, (BL_FLOAT)0.0);
    
    *magns = minEnv;
}

void
PartialTracker5::FilterPartials()
{
#if !FILTER_PARTIALS
    mResult = mPartials[0];
#else // Filtered
    FilterPartials(&mResult);
#endif
}

// For noise envelope extraction, the
// state must be ALIVE, and not mWasAlive
bool
PartialTracker5::GetAlivePartials(vector<Partial> *partials)
{
    if (mPartials.empty())
        return false;
    
    partials->clear();
    
    for (int i = 0; i < mPartials[0].size(); i++)
    {
        const Partial &p = mPartials[0][i];
        if (p.mState == Partial::ALIVE)
        {
            partials->push_back(p);
        }
    }
    
    return true;
}

void
PartialTracker5::RemoveRealDeadPartials(vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &p = (*partials)[i];
        if (p.mWasAlive)
            result.push_back(p);
    }
    
    *partials = result;
}

void
PartialTracker5::GetPartials(vector<Partial> *partials)
{
    *partials = mResult;
    
#if USE_KALMAN_FOR_RESULT
    for (int i = 0; i < partials->size(); i++)
    {
        (*partials)[i].mFreq = (*partials)[i].mPredictedFreq;
    }
#endif
    
    // For sending good result to SASFrame
    RemoveRealDeadPartials(partials);
}

void
PartialTracker5::ClearResult()
{
    mResult.clear();
}

void
PartialTracker5::GetNoiseEnvelope(WDL_TypedBuf<BL_FLOAT> *noiseEnv)
{
    *noiseEnv = mNoiseEnvelope;
}

void
PartialTracker5::GetHarmonicEnvelope(WDL_TypedBuf<BL_FLOAT> *harmoEnv)
{
    *harmoEnv = mHarmonicEnvelope;
}

void
PartialTracker5::SetMaxDetectFreq(BL_FLOAT maxFreq)
{
    mMaxDetectFreq = maxFreq;
}

// Optimized version (original version removed)
void
PartialTracker5::DetectPartials(const WDL_TypedBuf<BL_FLOAT> &magns,
                                const WDL_TypedBuf<BL_FLOAT> &phases,
                                vector<Partial> *outPartials)
{
    outPartials->clear();
    
    //
    WDL_TypedBuf<BL_FLOAT> smoothMagns = magns;
    
#if DETECT_PARTIALS_SMOOTH
    // Get a smoothed version of the magns
    if (mSmoothWinDetect.GetSize() != DETECT_PARTIALS_SMOOTH_SIZE)
    {
        Window::MakeHanning(DETECT_PARTIALS_SMOOTH_SIZE, &mSmoothWinDetect);
        
        // Works well too
        //
        // See: https://en.wikipedia.org/wiki/Window_function
        //
        //BL_FLOAT sigma = 0.1;
        //Window::MakeGaussian2(sigma, DETECT_PARTIALS_SMOOTH_SIZE, &mSmoothWinDetect);
    }
    
    BLUtils::SmoothDataWin(&smoothMagns, magns, mSmoothWinDetect);
    
#endif
    
    // prevIndex, currentIndex, nextIndex
    
    // Skip the first ones
    // (to avoid artifacts of very low freq partial)
    //int currentIndex = 0;
    int currentIndex = DETECT_PARTIALS_START_INDEX;
    
    BL_FLOAT prevVal = 0.0;
    BL_FLOAT nextVal = 0.0;
    BL_FLOAT currentVal = 0.0;
    
    int maxDetectIndex = smoothMagns.GetSize();
    
    if (mMaxDetectFreq > 0.0)
    {
        maxDetectIndex = mMaxDetectFreq*mBufferSize*0.5;
    }
    
    if (maxDetectIndex > smoothMagns.GetSize())
        maxDetectIndex = smoothMagns.GetSize();
    
    while(currentIndex < maxDetectIndex)
    {
        if ((currentVal > prevVal) && (currentVal >= nextVal))
            // Maximum found
        {
            if (currentIndex - 1 >= 0)
            {
                // Take the left and right "feets" of the partial,
                // then the middle.
                // (in order to be more precise)
                
                // Left
                int leftIndex = currentIndex;
                if (leftIndex > 0)
                {
                    BL_FLOAT prevLeftVal = smoothMagns.Get()[leftIndex];
                    while(leftIndex > 0)
                    {
                        leftIndex--;
                        
                        BL_FLOAT leftVal = smoothMagns.Get()[leftIndex];
                        
                        // Stop if we reach 0 or if it goes up again
                        if ((leftVal < MIN_NORM_AMP) || (leftVal > prevLeftVal))
                        {
                            if (leftVal >= prevLeftVal)
                                leftIndex++;
                            
                            // Check bounds
                            if (leftIndex < 0)
                                leftIndex = 0;
                            
                            if (leftIndex >= maxDetectIndex)
                                leftIndex = maxDetectIndex - 1;
                            
                            break;
                        }
                        
                        prevLeftVal = leftVal;
                    }
                }
                
                // Right
                int rightIndex = currentIndex;
                
                if (rightIndex < maxDetectIndex)
                {
                    BL_FLOAT prevRightVal = smoothMagns.Get()[rightIndex];
                    
                    while(rightIndex < maxDetectIndex - 1)
                    {
                        rightIndex++;
                                
                        BL_FLOAT rightVal = smoothMagns.Get()[rightIndex];
                                
                        // Stop if we reach 0 or if it goes up again
                        if ((rightVal < MIN_NORM_AMP) || (rightVal > prevRightVal))
                            {
                                if (rightVal >= prevRightVal)
                                    rightIndex--;
                                    
                                // Check bounds
                                if (rightIndex < 0)
                                    rightIndex = 0;
                                    
                                if (rightIndex >= maxDetectIndex)
                                    rightIndex = maxDetectIndex - 1;
                                    
                                break;
                            }
                                
                        prevRightVal = rightVal;
                    }
                }
                
                // Take the max (better than taking the middle)
                int peakIndex = currentIndex;
                
                if ((peakIndex < 0) || (peakIndex >= maxDetectIndex))
                    // Out of bounds
                    continue;
                
#if NARROW_PARTIAL_FOOT
                //NarrowPartialFoot(magns, peakIndex, &leftIndex, &rightIndex);
#endif
                
                bool discard = false;
                
#if DISCARD_FLAT_PARTIAL
                //discard = DiscardFlatPartial(magns, peakIndex, leftIndex, rightIndex);
#endif
                
#if DISCARD_INVALID_PEAKS
                if (!discard)
                {
                    discard = DiscardInvalidPeaks(magns, peakIndex, leftIndex, rightIndex);
                }
#endif
                
                if (!discard)
                {
                    // Create new partial
                    //
                    Partial p;
                    p.mLeftIndex = leftIndex;
                    p.mRightIndex = rightIndex;
                    
#if COMPUTE_PEAKS_AVG
                    // Make smooth partials change
                    // But makes wobble in the sound volume
                    BL_FLOAT peakIndexF = ComputePeakIndexAvg(magns, leftIndex, rightIndex);
#endif
#if COMPUTE_PEAKS_PARABOLA
                    // Won't work well with peaks with almost flat top ?
                    //
                    // Make smooth partials change
                    // But makes wobble in the sound volume
                    BL_FLOAT peakIndexF = ComputePeakIndexParabola(magns, peakIndex);
#endif
                    
#if COMPUTE_PEAKS_SIMPLE
                    //BL_FLOAT peakIndexF = peakIndex;
                    BL_FLOAT peakIndexF = (leftIndex + rightIndex)*0.5;
#endif
                    
                    p.mPeakIndex = bl_round(peakIndexF);
                    if (p.mPeakIndex < 0)
                        p.mPeakIndex = 0;
                    
                    if (p.mPeakIndex > maxDetectIndex - 1)
                        p.mPeakIndex = maxDetectIndex - 1;

                    BL_FLOAT peakFreq = peakIndexF/(mBufferSize*0.5);
                    p.mFreq = peakFreq;
                    
                    // Kalman
                    //
                    // Update the estimate with the first value
                    //p.mKf.updateEstimate(p.mFreq);
                    p.mKf.initEstimate(p.mFreq);
                    
                    // For predicted freq to be freq for the first value
                    p.mPredictedFreq = p.mFreq;
                    
                    // Default value. will be overwritten
                    BL_FLOAT peakAmp = magns.Get()[(int)peakIndexF];
                    
                    if (mFreqObj == NULL)
                        peakAmp = ComputePeakAmpInterp(magns, peakFreq);
                    else
                        // Buggy (mel scale ?)
                        peakAmp = ComputePeakAmpInterpFreqObj(magns, peakFreq);
                    
                    p.mAmp = peakAmp;
                    // Phase
                    p.mPhase = phases.Get()[(int)peakIndexF];
                    
                    outPartials->push_back(p);
                }
                
                // Go just after the right foot of the partial
                currentIndex = rightIndex;
            }
        }
        else
            // No maximum found, continue 1 step
            currentIndex++;
        
        // Update the values
        currentVal = smoothMagns.Get()[currentIndex];
        
        if (currentIndex - 1 >= 0)
            prevVal = magns.Get()[currentIndex - 1];
        
        if (currentIndex + 1 < maxDetectIndex)
            nextVal = magns.Get()[currentIndex + 1];
    }
}

// From GlueTwinPartials()
bool
PartialTracker5::GluePartialBarbs(const WDL_TypedBuf<BL_FLOAT> &magns,
                                  vector<Partial> *partials)
{
    vector<Partial> result;
    bool glued = false;
    
    sort(partials->begin(), partials->end(), Partial::FreqLess);
    
    int idx = 0;
    while(idx < partials->size())
    {
        Partial currentPartial = (*partials)[idx];
        
        vector<Partial> twinPartials;
        twinPartials.push_back(currentPartial);
        
        for (int j = idx + 1; j < partials->size(); j++)
        {
            const Partial &otherPartial = (*partials)[j];
            
            if (otherPartial.mLeftIndex == currentPartial.mRightIndex)
            // This is a twin partial...
            {
                BL_FLOAT promCur = ComputePeakProminence(magns,
                                                       currentPartial.mPeakIndex,
                                                       currentPartial.mLeftIndex,
                                                       currentPartial.mRightIndex);
                
                BL_FLOAT promOther = ComputePeakProminence(magns,
                                                         otherPartial.mPeakIndex,
                                                         otherPartial.mLeftIndex,
                                                         otherPartial.mRightIndex);
                
                // Default ratio value
                // If it keeps this value, this is ok, this will be glued
                BL_FLOAT ratio = 0.0;
                if (promOther > BL_EPS)
                {
                    ratio = promCur/promOther;
                    if ((ratio > GLUE_BARBS_AMP_RATIO) || (ratio < 1.0/GLUE_BARBS_AMP_RATIO))
                    // ... with a big amp ratio
                    {
                        // Check that the barb is "in the middle" of a side of the main partial
                        // (in height)
                        bool inTheMiddle = false;
                        bool onTheSide = false;
                        if (promCur < promOther)
                        {
                            BL_FLOAT hf = ComputePeakHigherFoot(magns,
                                                              currentPartial.mLeftIndex,
                                                              currentPartial.mRightIndex);

                            
                            BL_FLOAT lf = ComputePeakLowerFoot(magns,
                                                             otherPartial.mLeftIndex,
                                                             otherPartial.mRightIndex);
                            
                            if ((hf > lf) && (hf < otherPartial.mAmp))
                                inTheMiddle = true;
                            
                            // Check that the barb is on the right side
                            BL_FLOAT otherLeftFoot = magns.Get()[otherPartial.mLeftIndex];
                            BL_FLOAT otherRightFoot = magns.Get()[otherPartial.mRightIndex];
                            if (otherLeftFoot > otherRightFoot)
                                onTheSide = true;
                            
                        }
                        else
                        {
                            BL_FLOAT hf = ComputePeakHigherFoot(magns,
                                                              otherPartial.mLeftIndex,
                                                              otherPartial.mRightIndex);
                            
                            
                            BL_FLOAT lf = ComputePeakLowerFoot(magns,
                                                             currentPartial.mLeftIndex,
                                                             currentPartial.mRightIndex);
                            
                            if ((hf > lf) && (hf < currentPartial.mAmp))
                                inTheMiddle = true;
                            
                            // Check that the barb is on the right side
                            BL_FLOAT curLeftFoot = magns.Get()[currentPartial.mLeftIndex];
                            BL_FLOAT curRightFoot = magns.Get()[currentPartial.mRightIndex];
                            if (curLeftFoot < curRightFoot)
                                onTheSide = true;
                        }
                            
                        if (inTheMiddle && onTheSide)
                            // This tween partial is a barb ! 
                            twinPartials.push_back(otherPartial);
                    }
                }
            }
        }
        
        //
        // Glue ?
        //
        
        if (twinPartials.size() > 1)
        {
            glued = true;
            
            // Compute glued partial
            int leftIndex = twinPartials[0].mLeftIndex;
            int rightIndex = twinPartials[twinPartials.size() - 1].mRightIndex;
            
            BL_FLOAT peakIndex = ComputePeakIndexAvg(magns, leftIndex, rightIndex);
            
            // For peak amp, take max amp
            BL_FLOAT maxAmp = -BL_INF;
            for (int k = 0; k < twinPartials.size(); k++)
            {
                BL_FLOAT amp = twinPartials[k].mAmp;
                if (amp > maxAmp)
                    maxAmp = amp;
            }
            
            Partial res;
            res.mLeftIndex = leftIndex;
            res.mRightIndex = rightIndex;
            
            // Artificial peak
            res.mPeakIndex = peakIndex;
            
            BL_FLOAT peakFreq = peakIndex/(mBufferSize*0.5);
            res.mFreq = peakFreq;
            res.mAmp = maxAmp;
            
            // Kalman
            res.mKf.initEstimate(res.mFreq);
            res.mPredictedFreq = res.mFreq;
            
            // Do not set mPhase for now
            
            result.push_back(res);
        }
        else
            // Not twin, simply add the partial
        {
            result.push_back(twinPartials[0]);
        }
        
        // 1 or more
        idx += twinPartials.size();
    }
    
    *partials = result;
    
    return glued;
}

// Avoid the partial foot to leak on the left and right
// with very small amplitudes
void
PartialTracker5::NarrowPartialFoot(const WDL_TypedBuf<BL_FLOAT> &magns,
                                   int peakIndex,
                                   int *leftIndex, int *rightIndex)
{
    BL_FLOAT peakAmp = magns.Get()[peakIndex];
    
    // We will stop if we are under minAmp
    BL_FLOAT minAmp = peakAmp/NARROW_PARTIAL_FOOT_COEFF;
    
    // Left
    int newLeftIndex = peakIndex;
    while(newLeftIndex > *leftIndex)
    {
        BL_FLOAT ampLeft = magns.Get()[newLeftIndex];
        if (ampLeft <= minAmp)
            break;
        
        newLeftIndex--;
    }
    *leftIndex = newLeftIndex;
    
    // Right
    int newRightIndex = peakIndex;
    while(newRightIndex < *rightIndex)
    {
        BL_FLOAT ampRight = magns.Get()[newRightIndex];
        if (ampRight <= minAmp)
            break;
        
        newRightIndex++;
    }
    *rightIndex = newRightIndex;
}

void
PartialTracker5::NarrowPartialFoot(const WDL_TypedBuf<BL_FLOAT> &magns,
                                   vector<Partial> *partials)
{
    for (int i = 0; i < partials->size(); i++)
    {
        Partial &partial = (*partials)[i];
        
        NarrowPartialFoot(magns, partial.mPeakIndex,
                          &partial.mLeftIndex, &partial.mRightIndex);
    }
}

bool
PartialTracker5::DiscardFlatPartial(const WDL_TypedBuf<BL_FLOAT> &magns,
                                    int peakIndex, int leftIndex, int rightIndex)
{
    BL_FLOAT amp = magns.Get()[peakIndex];
    
    BL_FLOAT binDiff = rightIndex - leftIndex;
    
    BL_FLOAT coeff = binDiff/amp;
    
    bool result = (coeff > DISCARD_FLAT_PARTIAL_COEFF);
    
    return result;
}

void
PartialTracker5::DiscardFlatPartials(const WDL_TypedBuf<BL_FLOAT> &magns,
                                     vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
            
        bool discard = DiscardFlatPartial(magns,
                                          partial.mPeakIndex,
                                          partial.mLeftIndex,
                                          partial.mRightIndex);
        
        if (!discard)
            result.push_back(partial);
    }
    
    *partials = result;
}

bool
PartialTracker5::DiscardInvalidPeaks(const WDL_TypedBuf<BL_FLOAT> &magns,
                                     int peakIndex, int leftIndex, int rightIndex)
{
    BL_FLOAT peakAmp = magns.Get()[peakIndex];
    BL_FLOAT leftAmp = magns.Get()[leftIndex];
    BL_FLOAT rightAmp = magns.Get()[rightIndex];
    
    if ((peakAmp > leftAmp) && (peakAmp > rightAmp))
        // Correct, do not discard
        return false;
    
    return true;
}

// Works well to suppress the "tsss"
// while keeping the low frequencies partials
// Mel works better than linear scale
void
PartialTracker5::ApplyMinSpacingMel(vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial0 = (*partials)[i];
        BL_FLOAT peakFreq0 = partial0.mFreq;
        
        bool discard = false;
        for (int j = 0; j < partials->size(); j++)
        {
            if (j == i)
                continue;
            
            const Partial &partial1 = (*partials)[j];
            BL_FLOAT peakFreq1 = partial1.mFreq;
            
            if (std::fabs(peakFreq0 - peakFreq1) < MIN_SPACING_NORM_MEL)
            {
                discard = true;
                
                break;
            }
        }
        
        if (!discard)
            result.push_back(partial0);
    }
    
    *partials = result;
}

// For Air + keep only noise + Tibetan bowl: avoids a partial remaining at 2KHz
void
PartialTracker5::GlueTwinPartials(const WDL_TypedBuf<BL_FLOAT> &magns,
                                  vector<Partial> *partials)
{
    vector<Partial> result;
    
    sort(partials->begin(), partials->end(), Partial::FreqLess);
    
    int idx = 0;
    while(idx < partials->size())
    {
        Partial currentPartial = (*partials)[idx];
        
        vector<Partial> twinPartials;
        twinPartials.push_back(currentPartial);
        
        for (int j = idx + 1; j < partials->size(); j++)
        {
            const Partial &other = (*partials)[j];
            
            if (other.mLeftIndex == currentPartial.mRightIndex)
                // This is a twin partial
            {
                twinPartials.push_back(other);
                
                // Iterate
                // (may glue more than 2 partials)
                currentPartial = other;
            }
            else
            {
                // Not twin, continue over next partials
                break;
            }
        }
        
        if (twinPartials.size() > 1)
        {
            // Compute glued partial
            int leftIndex = twinPartials[0].mLeftIndex;
            int rightIndex = twinPartials[twinPartials.size() - 1].mRightIndex;
            
            BL_FLOAT peakIndex = ComputePeakIndexAvg(magns, leftIndex, rightIndex);
            
            // For peak amp, take max amp
            BL_FLOAT maxAmp = -BL_INF;
            for (int k = 0; k < twinPartials.size(); k++)
            {
                BL_FLOAT amp = twinPartials[k].mAmp;
                if (amp > maxAmp)
                    maxAmp = amp;
            }
            
            Partial res;
            res.mLeftIndex = leftIndex;
            res.mRightIndex = rightIndex;
            
            // Artificial peak
            res.mPeakIndex = peakIndex;
            
            BL_FLOAT peakFreq = peakIndex/(mBufferSize*0.5);
            res.mFreq = peakFreq;
            res.mAmp = maxAmp;
            
            // Kalman
            //
            res.mKf.initEstimate(res.mFreq);
            res.mPredictedFreq = res.mFreq;

            // Do not set mPhase for now
            result.push_back(res);
        }
        else
            // Not twin, simply add the partial
        {
            result.push_back(twinPartials[0]);
        }
        
        // 1 or more
        idx += twinPartials.size();
    }
    
    *partials = result;
}

// Other version (works less)
// => center of partials are not correct
// (because not ponderated by magnitudes)
void
PartialTracker5::GlueTwinPartials(vector<Partial> *partials)
{
    vector<Partial> result;
    
    sort(partials->begin(), partials->end(), Partial::FreqLess);
    
    int idx = 0;
    while(idx < partials->size())
    {
        Partial currentPartial = (*partials)[idx];
        
        vector<Partial> twinPartials;
        twinPartials.push_back(currentPartial);
        
        for (int j = idx + 1; j < partials->size(); j++)
        {
            const Partial &other = (*partials)[j];
            
            if (other.mLeftIndex == currentPartial.mRightIndex)
                // This is a twin partial
            {
                twinPartials.push_back(other);
                
                // Iterate
                // (may glue more than 2 partials)
                currentPartial = other;
            }
            else
            {
                // Not twin, continue over next partials
                break;
            }
        }
        
        if (twinPartials.size() > 1)
        {
            // Compute glued partial
            int leftIndex = twinPartials[0].mLeftIndex;
            int rightIndex = twinPartials[twinPartials.size() - 1].mRightIndex;
            
            int peakIndex = (leftIndex + rightIndex)/2;
            
            BL_FLOAT peakFreq = ((BL_FLOAT)peakIndex)/(mBufferSize*0.5);
            
            // For peak amp, take max amp
            BL_FLOAT maxAmp = -BL_INF;
            for (int k = 0; k < twinPartials.size(); k++)
            {
                BL_FLOAT amp = twinPartials[k].mAmp;
                if (amp > maxAmp)
                    maxAmp = amp;
            }
            
            Partial res;
            res.mLeftIndex = leftIndex;
            res.mRightIndex = rightIndex;
            
            // Artificial peak
            res.mPeakIndex = peakIndex;
            
            res.mFreq = peakFreq;
            res.mAmp = maxAmp;
            
            // Kalman
            res.mKf.initEstimate(res.mFreq);
            res.mPredictedFreq = res.mFreq;
            
            // Do not set mPhase for now
            
            result.push_back(res);
        }
        else
            // Not twin, simply add the partial
        {
            result.push_back(currentPartial);
        }
        
        // 1 or more
        idx += twinPartials.size();
    }
    
    *partials = result;
}

void
PartialTracker5::SuppressZeroFreqPartials(vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
        
        BL_FLOAT peakFreq = partial.mFreq;
        
        // Zero frequency (because of very small magn) ?
        bool discard = false;
        if (peakFreq < BL_EPS)
            discard = true;
        
        if (!discard)
            result.push_back(partial);
    }
    
    *partials = result;
}

void
PartialTracker5::ThresholdPartialsAmp(vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
    
        BL_FLOAT peakAmp = partial.mAmp;
        
        // Too small amp ?
        if (peakAmp > mThreshold)
            result.push_back(partial);
    }
    
    *partials = result;
}

void
PartialTracker5::ThresholdPartialsPeakProminence(const WDL_TypedBuf<BL_FLOAT> &magns,
                                                 vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
        
        BL_FLOAT prominence = ComputePeakProminence(magns,
                                                  partial.mPeakIndex,
                                                  partial.mLeftIndex,
                                                  partial.mRightIndex);
        
        // Just in case
        if (prominence < 0.0)
            prominence = 0.0;
    
        // Threshold
        if (prominence >= -(MIN_AMP_DB - mThreshold))
            result.push_back(partial);
    }
    
    *partials = result;
}

void
PartialTracker5::ThresholdPartialsPeakHeight(/*const WDL_TypedBuf<BL_FLOAT> &magns,*/
                                            vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
        
        /*BL_FLOAT height = ComputePeakHeight(magns,
                                            partial.mPeakIndex,
                                            partial.mLeftIndex,
                                            partial.mRightIndex); */
        
        BL_FLOAT height = partial.mPeakHeight;
        
        // Just in case
        if (height < 0.0)
            height = 0.0;
        
        // Threshold
        // TODO: Re-check this well
        //BL_FLOAT thrsNorm = -(MIN_AMP_DB - mThreshold)/(-MIN_AMP_DB);
        
        int binNum = partial.mFreq*mBufferSize*0.5;
        BL_FLOAT thrsNorm = GetThreshold(binNum);
        
        if (height >= thrsNorm)
             result.push_back(partial);
    }
    
    *partials = result;
}

// NOT USED ANYMORE
#if 0
void
PartialTracker5::ThresholdFilteredPartials(vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
        
        //BL_FLOAT amp = partial.mAmp;
        BL_FLOAT amp = partial.mPeakHeight;
        
        // Just in case
        if (amp < 0.0)
            amp = 0.0;
        
        //int binNum = partial.mFreq*mBufferSize*0.5;
        //BL_FLOAT thrsNorm = GetThreshold(binNum);
        // Threshold
        // TODO: Re-check this well
        BL_FLOAT thrsNorm = -(MIN_AMP_DB - mThreshold)/(-MIN_AMP_DB);
        
        if (amp >= thrsNorm)
            result.push_back(partial);
    }
    
    *partials = result;
}
#endif

void
PartialTracker5::ThresholdPartialsPeakHeightDb(const WDL_TypedBuf<BL_FLOAT> &magns,
                                               vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
        
        BL_FLOAT heightDb = ComputePeakHeightDb(magns,
                                              partial.mPeakIndex,
                                              partial.mLeftIndex,
                                              partial.mRightIndex,
                                              partial);
        
        // Threshold
        if (heightDb >= mThreshold)
            result.push_back(partial);
    }
    
    *partials = result;
}

// Try to threshold with a sloped threshold
// (to threshold musical noise in low frequencies,
//  while keeping high frequencies not thresholded)
// => Interesting but dos not solve the problem
void
PartialTracker5::ThresholdPartialsPeakHeightPink(const WDL_TypedBuf<BL_FLOAT> &magns,
                                                 vector<Partial> *partials)
{
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
        
        // Height
        BL_FLOAT height = ComputePeakHeight(magns,
                                            partial.mPeakIndex,
                                            partial.mLeftIndex,
                                            partial.mRightIndex);
        
        // Pink scale
        // Pink scale => threshold more the low frequencies
        // (continuous slope in log scale)
        BL_FLOAT threshold = mThreshold;
        
        // NOTE: should be fixed
        BL_FLOAT gain = 3.0*log(partial.mFreq)/log(2.0);
        threshold = threshold + gain - 40.0;
        
        // Threshold
        if (height + MIN_AMP_DB >= threshold)
            result.push_back(partial);
    }
    
    *partials = result;
}

// Fixed version
BL_FLOAT
PartialTracker5::ComputePeakProminence(const WDL_TypedBuf<BL_FLOAT> &magns,
                                       int peakIndex, int leftIndex, int rightIndex)
{
    // Compute prominence
    //
    // See: https://www.mathworks.com/help/signal/ref/findpeaks.html
    //
    BL_FLOAT maxFootAmp = magns.Get()[leftIndex];
    if (magns.Get()[rightIndex] > maxFootAmp)
        maxFootAmp = magns.Get()[rightIndex];
    
    BL_FLOAT peakAmp = magns.Get()[peakIndex];
    
    BL_FLOAT prominence = peakAmp - maxFootAmp;
    
    return prominence;
}

// Inverse of prominence
BL_FLOAT
PartialTracker5::ComputePeakHeight(const WDL_TypedBuf<BL_FLOAT> &magns,
                                   int peakIndex, int leftIndex, int rightIndex)
{
    // Compute height
    //
    // See: https://www.mathworks.com/help/signal/ref/findpeaks.html
    //
    BL_FLOAT minFootAmp = magns.Get()[leftIndex];
    if (magns.Get()[rightIndex] < minFootAmp)
        minFootAmp = magns.Get()[rightIndex];
    
    BL_FLOAT peakAmp = magns.Get()[peakIndex];
    
    BL_FLOAT height = peakAmp - minFootAmp;
    
    return height;
}

// Compute difference in amp, then convert back to Db
BL_FLOAT
PartialTracker5::ComputePeakHeightDb(const WDL_TypedBuf<BL_FLOAT> &magns,
                                     int peakIndex, int leftIndex, int rightIndex,
                                     const Partial &partial)
{
    // Compute height
    //
    // See: https://www.mathworks.com/help/signal/ref/findpeaks.html
    //
    
    // Get in Db
    BL_FLOAT minFoot = magns.Get()[leftIndex];
    if (magns.Get()[rightIndex] < minFoot)
        minFoot = magns.Get()[rightIndex];
    
    BL_FLOAT peak = partial.mAmp;
    
    // Compute height
    BL_FLOAT height = peak - minFoot;
    
    return height;
}

BL_FLOAT
PartialTracker5::ComputePeakHigherFoot(const WDL_TypedBuf<BL_FLOAT> &magns,
                                       int leftIndex, int rightIndex)
{
    BL_FLOAT leftVal = magns.Get()[leftIndex];
    BL_FLOAT rightVal = magns.Get()[rightIndex];
    
    if (leftVal > rightVal)
        return leftVal;
    else
        return rightVal;
}

BL_FLOAT
PartialTracker5::ComputePeakLowerFoot(const WDL_TypedBuf<BL_FLOAT> &magns,
                                      int leftIndex, int rightIndex)
{
    BL_FLOAT leftVal = magns.Get()[leftIndex];
    BL_FLOAT rightVal = magns.Get()[rightIndex];
    
    if (leftVal < rightVal)
        return leftVal;
    else
        return rightVal;
}

void
PartialTracker5::ComputePeaksHeights(const WDL_TypedBuf<BL_FLOAT> &magns,
                                    vector<Partial> *partials)
{
    for (int i = 0; i < partials->size(); i++)
    {
        Partial &partial = (*partials)[i];
        
        BL_FLOAT height = ComputePeakHeight(magns,
                                            partial.mPeakIndex,
                                            partial.mLeftIndex,
                                            partial.mRightIndex);
        
        partial.mPeakHeight = height;
    }
}

void
PartialTracker5::ThresholdPartialsAmpSmooth(const WDL_TypedBuf<BL_FLOAT> &magns,
                                            vector<Partial> *partials)
{
    if (mSmoothWinThreshold.GetSize() != THRESHOLD_PARTIALS_SMOOTH_SIZE)
    {
        Window::MakeHanning(THRESHOLD_PARTIALS_SMOOTH_SIZE, &mSmoothWinThreshold);
    }
    
    WDL_TypedBuf<BL_FLOAT> smoothMagns;
    BLUtils::SmoothDataWin(&smoothMagns, mCurrentMagns, mSmoothWinThreshold);
    
    WDL_TypedBuf<BL_FLOAT> magnsDiff = magns;
    BLUtils::SubstractValues(&magnsDiff, smoothMagns);
    
    BL_FLOAT hzPerBin = mSampleRate/mBufferSize;
    
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
        
        BL_FLOAT binNum = partial.mFreq/hzPerBin;
        
        // Take the nearest bin
        binNum = bl_round(binNum);
        
        BL_FLOAT magn = magnsDiff.Get()[(int)binNum];
        if (magn >= 0.0)
            // Amp is big enough, keep the partial
            result.push_back(partial);
    }
    
    *partials = result;
}


// See: https://imagej.net/Auto_Local_Threshold (Phansalkar)
//
// NOTE: did not success to make it work
//
void
PartialTracker5::ThresholdPartialsAmpAuto(const WDL_TypedBuf<BL_FLOAT> &magns,
                                          vector<Partial> *partials)
{
    int winSize = THRESHOLD_PARTIALS_AUTO_WIN_SIZE;
    
    BL_FLOAT hzPerBin = mSampleRate/mBufferSize;
    
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
        
        BL_FLOAT binNum = partial.mFreq/hzPerBin;
        
        // Take the nearest bin
        binNum = bl_round(binNum);
        
        // Compute the mean
        BL_FLOAT mean = 0.0;
        int numValues = 0;
        for (int j = 0; j < winSize; j++)
        {
            int idx = i + j - winSize/2;
            if ((idx < 0) || (idx > magns.GetSize() - 1))
                continue;
            
            BL_FLOAT val = magns.Get()[idx];
            
            mean += val;
            numValues++;
        }
        if (numValues > 0)
        {
            mean /= numValues;
        }
        
        // Compute the standard deviation
        BL_FLOAT stdev = 0.0;
        int numValues2 = 0;
        for (int j = 0; j < winSize; j++)
        {
            int idx = i + j - winSize/2;
            if ((idx < 0) || (idx > magns.GetSize() - 1))
                continue;
            
            BL_FLOAT val = magns.Get()[idx];
            
            stdev += (val - mean)*(val - mean);
            numValues2++;
        }
        if (numValues2 > 0)
        {
            stdev /= numValues2;
            
            stdev = std::sqrt(stdev);
        }
        
#if 0
        // See: https://imagej.net/Auto_Local_Threshold (Phansalkar)
        
        // Phansalkar parameters
        BL_FLOAT k = 0.25; // modifiable
        BL_FLOAT r = 0.5; // modifiable
        
        BL_FLOAT p = 2.0;
        BL_FLOAT q = 10.0;
        
        BL_FLOAT t = mean * (1.0 + p * std::exp(-q * mean) + k * ((stdev / r) - 1.0));
#endif
        
#if 0
        // Sauvola
        BL_FLOAT k = 0.5;
        BL_FLOAT r = 128.0;
        
        BL_FLOAT t = mean*(1.0 + k*(stdev/r - 1.0));
#endif
        
#if 0
        // Mean
        BL_FLOAT t = 1.0;
        if (partial.mAmpDB > mean - mThreshold)
            t = 0.0;
#endif
        
#if 1 // Original
        BL_FLOAT t = mThreshold;
#endif
        
        if (partial.mAmp/*DB*/ >= t)
            result.push_back(partial);
    }
    
     *partials = result;
}

void
PartialTracker5::SuppressBarbs(vector<Partial> *partials)
{
#define HEIGHT_COEFF 2.0
#define WIDTH_COEFF 1.0
    
    vector<Partial> result;
    for (int i = 0; i < partials->size(); i++)
    {
        const Partial &partial = (*partials)[i];
    
        // Check if the partial is a barb
        bool isBarb = false;
        for (int j = 0; j < partials->size(); j++)
        {
            const Partial &other = (*partials)[j];
            
            if (other.mAmp < partial.mAmp*HEIGHT_COEFF)
                // Amplitudes of tested partial is not small enough
                // compared to the current partial
                // => Not a candidate for barb
                continue;
            
            int center = other.mPeakIndex;
            int size = other.mRightIndex - other.mLeftIndex;
            
            if ((partial.mPeakIndex > center - size*WIDTH_COEFF) &&
                (partial.mPeakIndex < center + size*WIDTH_COEFF))
            {
                // Tested partial peak is "inside" a margin around
                // the current partial.
                // And its amplitude is small compared to the current partial
                // => this is a barb !
                isBarb = true;
                
                break;
            }
        }
        
        // It is not a barb
        if (!isBarb)
            result.push_back(partial);
    }
    
    *partials = result;
}

// Filter
void
PartialTracker5::FilterPartials(vector<Partial> *result)
{
    result->clear();
    
    if (mPartials.empty())
        return;
    
    //SuppressNoisyPartials(&mPartials[0]);
    
#if 0 // Deactivated for the moment, because it made loose tracking
      // on the lowest partial ("bell", second bell strike)
    
    // Suppress the barbs on the last detected series
    // (just added by DetectPartials)
    SuppressBarbs(&mPartials[0]);
#endif
    
#if 0 // NOTE: not so good
    //ApplyMinSpacing(&mPartials[0]);
#endif
    
#if APPLY_MIN_SPACING
    // NOTE: better
    ApplyMinSpacingMel(&mPartials[0]);
#endif
    
    // TEST
#if 1
    if (mPartials.size() == 1)
    {
        for (int j = 0; j < mPartials[0].size(); j++)
        {
            Partial &currentPartial = mPartials[0][j];
            currentPartial.GenNewId();
        }
        return;
    }
#endif
    
    if (mPartials.size() < 2)
        return;
    
    const vector<Partial> &prevPartials = mPartials[1];
    vector<Partial> currentPartials = mPartials[0];
    
    // Partials that was not associated at the end
    vector<Partial> remainingPartials;
    
    // ORIGIN
    // NOTE: new version: better (sort by amp, then assoc with threshold)
    //AssociatePartials(prevPartials, &currentPartials, &remainingPartials);
    
    AssociatePartialsPARSHL(prevPartials, &currentPartials, &remainingPartials);
    
    // Was just a test
    //AssociatePartialsFreqAmp(prevPartials, &currentPartials, &remainingPartials);
    
#if THRESHOLD_FILTERED_PARTIALS
    //ThresholdFilteredPartials(&remainingPartials);
    ThresholdPartialsPeakHeight(&currentPartials);
#endif
    
#if FILTER_SMOOTH
    SmoothPartials(prevPartials, &currentPartials);
#endif
    
    // Add the new zombie and dead partials
    for (int i = 0; i < prevPartials.size(); i++)
    {
        const Partial &prevPartial = prevPartials[i];

        bool found = false;
        for (int j = 0; j < currentPartials.size(); j++)
        {
            const Partial &currentPartial = currentPartials[j];
            
            if (currentPartial.mId == prevPartial.mId)
            {
                found = true;
                
                break;
            }
        }

        if (!found)
        {
            if (prevPartial.mState == Partial::ALIVE)
            {
                // We set zombie for 1 frame only
                Partial newPartial = prevPartial;
                newPartial.mState = Partial::ZOMBIE;
                newPartial.mZombieAge = 0;
                
                // Kalman:
                // GOOD: extrapolate the zombies
                newPartial.mPredictedFreq =
                    newPartial.mKf.updateEstimate(newPartial.mFreq);
                
                currentPartials.push_back(newPartial);
            }
            else if (prevPartial.mState == Partial::ZOMBIE)
            {
                Partial newPartial = prevPartial;
                
                newPartial.mZombieAge++;
                if (newPartial.mZombieAge >= MAX_ZOMBIE_AGE)
                    newPartial.mState = Partial::DEAD;
  
                // Kalman
                // GOOD: extrapolate the zombies
                newPartial.mPredictedFreq =
                    newPartial.mKf.updateEstimate(newPartial.mFreq);

                currentPartials.push_back(newPartial);
            }
            
            // If DEAD, do not add, forget it
        }
    }
    
    // Get the result here
    // So we get the partials that are well tracked over time
    *result = currentPartials;
    
    // At the end, there remains the partial that have not been matched
    //
    // Add them at to the history for next time
    //
    for (int i = 0; i < remainingPartials.size(); i++)
    {
        Partial p = remainingPartials[i];
        
        p.GenNewId();
        
        currentPartials.push_back(p);
    }
    
    // Then sort the new partials by frequency
    sort(currentPartials.begin(), currentPartials.end(), Partial::FreqLess);
    
    //
    // Update: add the partials to the history
    // (except the dead ones)
    mPartials[0].clear();
    for (int i = 0; i < currentPartials.size(); i++)
    {
        const Partial &currentPartial = currentPartials[i];
        
        // TEST: do not skip the dead partials:
        // they will be used for fade out !
        //if (currentPartial.mState != Partial::DEAD)
        mPartials[0].push_back(currentPartial);
    }
}

// Better than "Simple" => do not make jumps between bins
BL_FLOAT
PartialTracker5::ComputePeakIndexAvg(const WDL_TypedBuf<BL_FLOAT> &magns,
                                     int leftIndex, int rightIndex)
{
    // Pow coeff, to select preferably the high amp values
    // With 2.0, makes smoother freq change
    // With 3.0, make just a little smoother than 2.0
#define COEFF 3.0
    
    BL_FLOAT sumIndex = 0.0;
    BL_FLOAT sumMagns = 0.0;
    
    for (int i = leftIndex; i <= rightIndex; i++)
    {
        BL_FLOAT magn = magns.Get()[i];
        
        magn = std::pow(magn, COEFF);
        
        sumIndex += i*magn;
        sumMagns += magn;
    }
    
    if (sumMagns < BL_EPS)
        return 0.0;
    
    BL_FLOAT result = sumIndex/sumMagns;
    
    return result;
}

// Parabola peak center detection
// Works well (but I prefer my method) 
//
// See: http://eprints.maynoothuniversity.ie/4523/1/thesis.pdf (p32)
//
// and: https://ccrma.stanford.edu/~jos/parshl/Peak_Detection_Steps_3.html#sec:peakdet
//
BL_FLOAT
PartialTracker5::ComputePeakIndexParabola(const WDL_TypedBuf<BL_FLOAT> &magns,
                                          int peakIndex)
{
    if ((peakIndex - 1 < 0) || (peakIndex >= magns.GetSize()))
    {
        return peakIndex;
    }
    
    // magns are in DB
    // => no need to convert !
    BL_FLOAT alpha = magns.Get()[peakIndex - 1];
    BL_FLOAT beta = magns.Get()[peakIndex];
    BL_FLOAT gamma = magns.Get()[peakIndex + 1];
    
    // Center
    BL_FLOAT c = 0.5*((alpha - gamma)/(alpha - 2.0*beta + gamma));
    
    BL_FLOAT result = peakIndex + c;
    
    return result;
}

// Simple method
BL_FLOAT
PartialTracker5::ComputePeakAmpInterp(const WDL_TypedBuf<BL_FLOAT> &magns,
                                      BL_FLOAT peakFreq)
{
    BL_FLOAT freqBin = peakFreq*mBufferSize*0.5;
    
    // GOOD !
    // Bin idx must be BL_FLOAT, not int, to compute t parameter later
    // This fixes amplitude too low values sometimes!
#if !FIX_COMPUTE_PARTIAL_AMP
    int bin = freqBin;
#else
    BL_FLOAT bin = freqBin;
#endif
    
    int prevBin = (int)bin;
    int nextBin = (int)bin + 1;
    
    if (nextBin >= magns.GetSize())
    {
        BL_FLOAT peakAmp = magns.Get()[prevBin];
        
        return peakAmp;
    }
    
    BL_FLOAT prevAmp = magns.Get()[prevBin];
    BL_FLOAT nextAmp = magns.Get()[nextBin];
    
    BL_FLOAT t = bin - prevBin;
    
    BL_FLOAT peakAmp = (1.0 - t)*prevAmp + t*nextAmp;
    
    return peakAmp;
}

// NOTE: not well tested
BL_FLOAT
PartialTracker5::ComputePeakPhaseInterp(const WDL_TypedBuf<BL_FLOAT> &phases,
                                        BL_FLOAT peakFreq)
{
    int bin = peakFreq*(mBufferSize*0.5);
    
    int prevBin = (int)bin;
    int nextBin = (int)bin + 1;
    
    if (nextBin >= phases.GetSize())
    {
        BL_FLOAT peakPhase = phases.Get()[prevBin];
        
        return peakPhase;
    }
    
    BL_FLOAT prevPhase = phases.Get()[prevBin];
    BL_FLOAT nextPhase = phases.Get()[nextBin];
    
    BL_FLOAT t = bin - prevBin;
    
    BL_FLOAT peakPhase = (1.0 - t)*prevPhase + t*nextPhase;
    
    return peakPhase;
}

BL_FLOAT
PartialTracker5::ComputePeakAmpInterpFreqObj(const WDL_TypedBuf<BL_FLOAT> &magns,
                                             BL_FLOAT peakFreq)
{
    int idx0 = 0;
    for (int i = 0; i < mRealFreqs.GetSize(); i++)
    {
        BL_FLOAT realFreq = mRealFreqs.Get()[i];
        
        if (realFreq > peakFreq)
        {
            idx0 = i - 1;
            if (idx0 < 0)
                idx0 = 0;
            
            break;
        }
    }
    
    BL_FLOAT freq0 = mRealFreqs.Get()[idx0];
    
    int idx1 = idx0 + 1;
    if (idx1 >= mRealFreqs.GetSize())
    {
        BL_FLOAT result = magns.Get()[idx0];
        
        return result;
    }
    
    BL_FLOAT freq1 = mRealFreqs.Get()[idx1];

    BL_FLOAT t = (peakFreq - freq0)/(freq1 - freq0);
    
    BL_FLOAT prevAmp = magns.Get()[idx0];
    BL_FLOAT nextAmp = magns.Get()[idx1];
    
    BL_FLOAT peakAmp = (1.0 - t)*prevAmp + t*nextAmp;
    
    return peakAmp;
}

// TODO: check / Mel !!
BL_FLOAT
PartialTracker5::GetFrequency(int binIndex)
{
    if ((mFreqObj == NULL) || (binIndex >= mRealFreqs.GetSize()))
    {
        BL_FLOAT hzPerBin = mSampleRate/mBufferSize;
        BL_FLOAT result = binIndex*hzPerBin;
        
        return result;
    }
    else
    {
        BL_FLOAT result = mRealFreqs.Get()[binIndex];
        
        return result;
    }
}

int
PartialTracker5::FindPartialById(const vector<PartialTracker5::Partial> &partials,
                                 int idx)
{
    for (int i = 0; i < partials.size(); i++)
    {
        const Partial &partial = partials[i];
        
        if (partial.mId == idx)
            return i;
    }
    
    return -1;
}

void
PartialTracker5::AssociatePartials(const vector<PartialTracker5::Partial> &prevPartials,
                                   vector<PartialTracker5::Partial> *currentPartials,
                                   vector<PartialTracker5::Partial> *remainingPartials)
{
    // Sort current partials and prev partials by decreasing amplitude
    vector<Partial> currentPartialsSort = *currentPartials;
    sort(currentPartialsSort.begin(), currentPartialsSort.end(), Partial::AmpLess);
    reverse(currentPartialsSort.begin(), currentPartialsSort.end());
    
    vector<Partial> prevPartialsSort = prevPartials;
    sort(prevPartialsSort.begin(), prevPartialsSort.end(), Partial::AmpLess);
    reverse(prevPartialsSort.begin(), prevPartialsSort.end());
 
    // Associate
    
    // Associated partials
    vector<Partial> currentPartialsAssoc;
    for (int i = 0; i < prevPartialsSort.size(); i++)
    {
        const Partial &prevPartial = prevPartialsSort[i];
        
        for (int j = 0; j < currentPartialsSort.size(); j++)
        {
            Partial &currentPartial = currentPartialsSort[j];
            
            if (currentPartial.mId != -1)
                // Already assigned
                continue;
            
#if USE_KALMAN_FOR_ASSOC
            BL_FLOAT diffFreq = std::fabs(prevPartial.mPredictedFreq - currentPartial.mFreq);
#else
            BL_FLOAT diffFreq = std::fabs(prevPartial.mFreq - currentPartial.mFreq);
#endif
            
            int binNum = currentPartial.mFreq*mBufferSize*0.5;
            BL_FLOAT diffCoeff = GetFreqDiffCoeff(binNum);
            if (diffFreq < MAX_FREQ_DIFF_ASSOC*diffCoeff*mDbgParam)
            // Associated !
            {
                currentPartial.mId = prevPartial.mId;
                currentPartial.mState = Partial::ALIVE;
                currentPartial.mWasAlive = true;
                
                currentPartial.mAge = prevPartial.mAge + 1;
            
                // Kalman
                currentPartial.mKf = prevPartial.mKf;
                currentPartial.mPredictedFreq =
                            currentPartial.mKf.updateEstimate(currentPartial.mFreq);

                currentPartialsAssoc.push_back(currentPartial);
                
                // We have associated to the prev partial
                // We are done!
                // Stop the search here.
                break;
            }
        }
    }
    
    sort(currentPartialsAssoc.begin(), currentPartialsAssoc.end(), Partial::IdLess);
     *currentPartials = currentPartialsAssoc;
    
    // Add the remaining partials
    remainingPartials->clear();
    for (int i = 0; i < currentPartialsSort.size(); i++)
    {
        const Partial &p = currentPartialsSort[i];
        if (p.mId == -1)
            remainingPartials->push_back(p);
    }
}

void
PartialTracker5::AssociatePartialsFreqAmp(const vector<PartialTracker5::Partial> &prevPartials,
                                          vector<PartialTracker5::Partial> *currentPartials,
                                          vector<PartialTracker5::Partial> *remainingPartials)
{
    // DEBUG
    //AssociatePartials(prevPartials, currentPartials, remainingPartials);
    //return;
    
    // Sort current partials and prev partials by decreasing amplitude
    /*vector<Partial> currentPartialsSort = *currentPartials;
    sort(currentPartialsSort.begin(), currentPartialsSort.end(), Partial::AmpLess);
    reverse(currentPartialsSort.begin(), currentPartialsSort.end());
    
    vector<Partial> prevPartialsSort = prevPartials;
    sort(prevPartialsSort.begin(), prevPartialsSort.end(), Partial::AmpLess);
    reverse(prevPartialsSort.begin(), prevPartialsSort.end());
    */
    
    // Associate
    
    // Associated partials, as best as possible (existing partials)
    vector<Partial> currentPartialsAssoc;
    for (int i = 0; i < prevPartials.size(); i++)
    {
        const Partial &prevPartial = prevPartials[i];
        
        // Find the closest (distance depend on freq and amp)
        int idFound = -1;
        BL_FLOAT minDist2 = BL_INF;
        for (int j = 0; j < currentPartials->size(); j++)
        {
            Partial &currentPartial = (*currentPartials)[j];
            
            if (currentPartial.mId != -1)
                // Already assigned
                continue;
  
#if USE_KALMAN_FOR_ASSOC
            BL_FLOAT df = prevPartial.mPredictedFreq - currentPartial.mFreq;
#else
            BL_FLOAT df = prevPartial.mFreq - currentPartial.mFreq;
#endif

            // TODO: diminish dist as freqs increase
            if (std::fabs(df) > MAX_FREQ_DIFF_ASSOC*mDbgParam)
                // Too far
                continue;
                
            // TODO: kalman on amps
            BL_FLOAT da = prevPartial.mAmp - currentPartial.mAmp;
            
            BL_FLOAT dist2 = std::fabs(df); // simple
            //BL_FLOAT dist2 = df*df + da*da; // freq and amp
            //BL_FLOAT dist2 = df*df + da; // Take more amplitude
            if (dist2 < minDist2)
            {
                minDist2 = dist2;
                idFound = j;
            }
        }
        
        if (idFound != -1)
        {
            Partial &currentPartial = (*currentPartials)[idFound];
            
            currentPartial.mId = prevPartial.mId;
            currentPartial.mState = Partial::ALIVE;
            currentPartial.mWasAlive = true;
                
            currentPartial.mAge = prevPartial.mAge + 1;
                
            // Kalman
            currentPartial.mKf = prevPartial.mKf;
            currentPartial.mPredictedFreq =
                currentPartial.mKf.updateEstimate(currentPartial.mFreq);
            
            currentPartialsAssoc.push_back(currentPartial);
        }
    }
    
    //sort(currentPartialsAssoc.begin(), currentPartialsAssoc.end(), Partial::IdLess);
    //*currentPartials = currentPartialsAssoc;
    
    // Now manage the new partials, that can't be associated
    //AssociatePartials(prevPartials, currentPartials, remainingPartials);
    
    // Add the remaining partials
    remainingPartials->clear();
    for (int i = 0; i < currentPartials->size(); i++)
    {
        const Partial &p = (*currentPartials)[i];
        if (p.mId == -1)
            remainingPartials->push_back(p);
    }
    
    sort(currentPartialsAssoc.begin(), currentPartialsAssoc.end(), Partial::IdLess);
    *currentPartials = currentPartialsAssoc;
}

void
PartialTracker5::AssociatePartialsPARSHL(const vector<PartialTracker5::Partial> &prevPartials,
                                         vector<PartialTracker5::Partial> *currentPartials,
                                         vector<PartialTracker5::Partial> *remainingPartials)
{
    // Sort current partials and prev partials by increasing frequency
    sort(currentPartials->begin(), currentPartials->end(), Partial::FreqLess);
    
    vector<PartialTracker5::Partial> prevPartials0 = prevPartials;
    sort(prevPartials0.begin(), prevPartials0.end(), Partial::FreqLess);
    
    // Associated partials
    bool stopFlag = true;
    //int prevNumAssociated = 0;
    do {
        stopFlag = true;
        
        for (int i = 0; i < prevPartials0.size(); i++)
        {
            const Partial &prevPartial = prevPartials0[i];
            for (int j = 0; j < currentPartials->size(); j++)
            {
                Partial &currentPartial = (*currentPartials)[j];
                if (currentPartial.mId != -1)
                    // Already associated, nothing to do on this step!
                    continue;
                
#if USE_KALMAN_FOR_ASSOC
                BL_FLOAT diffFreq =
                    std::fabs(prevPartial.mPredictedFreq - currentPartial.mFreq);
#else
                BL_FLOAT diffFreq =
                    std::fabs(prevPartial.mFreq - currentPartial.mFreq);
#endif
                int binNum = currentPartial.mFreq*mBufferSize*0.5;
                BL_FLOAT diffCoeff = GetFreqDiffCoeff(binNum);
            
                if (diffFreq < MAX_FREQ_DIFF_ASSOC*diffCoeff*mDbgParam)
                    // Associate!
                {
                    int otherIdx =
                        FindPartialById(*currentPartials, (int)prevPartial.mId);
                    
                    if (otherIdx == -1)
                        // This partial is not yet associated
                        // => No fight
                    {
                        currentPartial.mId = prevPartial.mId;
                        currentPartial.mAge = prevPartial.mAge;
                        currentPartial.mKf = prevPartial.mKf; //
                        
                        stopFlag = false;
                    }
                    else // Fight!
                    {
                        Partial &otherPartial = (*currentPartials)[otherIdx];
                        
#if USE_KALMAN_FOR_ASSOC
                        BL_FLOAT otherDiffFreq =
                                std::fabs(prevPartial.mPredictedFreq - otherPartial.mFreq);
#else
                        BL_FLOAT otherDiffFreq =
                            std::fabs(prevPartial.mFreq - otherPartial.mFreq);
#endif
                        
                        if (diffFreq < otherDiffFreq)
                        // Current partial won
                        {
                            currentPartial.mId = prevPartial.mId;
                            currentPartial.mAge = prevPartial.mAge;
                            currentPartial.mKf = prevPartial.mKf; //
                            
                            // Detach the other
                            otherPartial.mId = -1;
                            
                            stopFlag = false;
                        }
                        else
                        // Other partial won
                        {
                            // Just keep it like it is!
                        }
                    }
                }
            }
        }
        
#if 0
        // Check if we have associated all the prev partials
        int numAssociated = 0;
        for (int j = 0; j < currentPartials->size(); j++)
        {
            const Partial &currentPartial = (*currentPartials)[j];
            if (currentPartial.mId != -1)
                numAssociated++;
        }
        if (numAssociated >= prevPartials0.size())
        {
            break;
        }
        
        if (numAssociated <= prevNumAssociated)
            break;
        prevNumAssociated = numAssociated;
#endif
        
    } while (!stopFlag);
    
    
    vector<PartialTracker5::Partial> newPartials;
    for (int j = 0; j < currentPartials->size(); j++)
    {
        Partial &currentPartial = (*currentPartials)[j];
        
        if (currentPartial.mId != -1)
        {
            currentPartial.mState = Partial::ALIVE;
            currentPartial.mWasAlive = true;
    
            // Increment age
            currentPartial.mAge = currentPartial.mAge + 1;
            currentPartial.mPredictedFreq =
                    currentPartial.mKf.updateEstimate(currentPartial.mFreq);
    
            newPartials.push_back(currentPartial);
        }
    }
    
    // Add the remaining partials
    remainingPartials->clear();
    for (int i = 0; i < currentPartials->size(); i++)
    {
        const Partial &p = (*currentPartials)[i];
        if (p.mId == -1)
            remainingPartials->push_back(p);
    }
    
    // Update current partials
    *currentPartials = newPartials;
}

void
PartialTracker5::SmoothPartials(const vector<PartialTracker5::Partial> &prevPartials,
                                vector<PartialTracker5::Partial> *currentPartials)
{
    for (int i = 0; i < currentPartials->size(); i++)
    {
        Partial &currentPartial = (*currentPartials)[i];
        
        int prevIdx = FindPartialById(prevPartials, currentPartial.mId);
        
        if (prevIdx != -1)
        {
            const Partial &prevPartial = prevPartials[prevIdx];
            
            // Smooth
            
            // Smooth freq
            BL_FLOAT freq0 = prevPartial.mFreq;
            BL_FLOAT freq1 = currentPartial.mFreq;
            
            BL_FLOAT newFreq = FILTER_SMOOTH_COEFF*freq0 + (1.0 - FILTER_SMOOTH_COEFF)*freq1;
            
            currentPartial.mFreq = newFreq;
            
            // Smooth amp
            BL_FLOAT amp0 = prevPartial.mAmp;
            BL_FLOAT amp1 = currentPartial.mAmp;
            
            BL_FLOAT newAmp = FILTER_SMOOTH_COEFF*amp0 +
                                    (1.0 - FILTER_SMOOTH_COEFF)*amp1;
            
            currentPartial.mAmp = newAmp;
        }
    }
}

void
PartialTracker5::DBG_DumpPartials(const char *fileName,
                                  const vector<Partial> &partials,
                                  int bufferSize)
{
    WDL_TypedBuf<BL_FLOAT> result;
    result.Resize(bufferSize);
    BLUtils::FillAllZero(&result);
    
    for (int i = 0; i < partials.size(); i++)
    {
        const PartialTracker5::Partial &p = partials[i];
        
        // Width
        for (int j = p.mLeftIndex; j <= p.mRightIndex; j++)
        {
            result.Get()[j] = p.mAmp*0.5;
        }
        
        // Peak
        result.Get()[p.mPeakIndex] = p.mAmp;
    }
    
    BLDebug::DumpData(fileName, result);
}

void
PartialTracker5::DBG_SetDbgParam(BL_FLOAT param)
{
    mDbgParam = param;
}

BL_FLOAT
PartialTracker5::GetThreshold(int binNum)
{
#if 0
    //BL_FLOAT thresholds[3] = { -48.0/*-35.0*/, -78.0, -87.0 };
    
    //BL_FLOAT thresholds[3] = { -56.0, -81.0/*-78.0*/, -80.0 };
    
    BL_FLOAT thresholds[3] = { -79.0, -100.0, -108.0 };
    
    BL_FLOAT t = ((BL_FLOAT)binNum)/(mBufferSize*0.5);
    
    BL_FLOAT thrs = 0.0;
    if (t < 0.5)
    {
        t = t*2.0;
        thrs = (1.0 - t)*thresholds[0] + t*thresholds[1];
    }
    else
    {
        t = 1.0 - (t - 0.5)*2.0;
        thrs = (1.0 - t)*thresholds[1] + t*thresholds[2];
    }
    
    thrs += mThreshold;
    
    //BL_FLOAT thrsNorm = -(MIN_AMP_DB - thrs)/(-MIN_AMP_DB);
#endif
    
    BL_FLOAT thrsNorm = -(MIN_AMP_DB - mThreshold)/(-MIN_AMP_DB);
    
    return thrsNorm;
}

BL_FLOAT
PartialTracker5::GetFreqDiffCoeff(int binNum)
{
#if 0
    return 1.0; // DEBUG
    
    BL_FLOAT diffCoeffs[3] = { 2.0/*1.0*/, 0.35, 0.2 };
    
    BL_FLOAT t = ((BL_FLOAT)binNum)/(mBufferSize*0.5);
    
    BL_FLOAT diffCoeff = 0.0;
    if (t < 0.5)
    {
        t = t*2.0;
        diffCoeff = (1.0 - t)*diffCoeffs[0] + t*diffCoeffs[1];
    }
    else
    {
        t = 1.0 - (t - 0.5)*2.0;
        diffCoeff = (1.0 - t)*diffCoeffs[1] + t*diffCoeffs[2];
    }
    
    return diffCoeff;
#endif
    
#define END_COEFF 0.25
    
    BL_FLOAT t = ((BL_FLOAT)binNum)/(mBufferSize*0.5);
    BL_FLOAT diffCoeff = 1.0 - (1.0 - END_COEFF)*t;
    
    return diffCoeff;
}
