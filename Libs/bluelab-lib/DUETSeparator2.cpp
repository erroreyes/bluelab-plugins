//
//  DUETSeparator2.cpp
//  BL-DUET
//
//  Created by applematuer on 5/3/20.
//
//

#include <algorithm>
using namespace std;

#include <DUETHistogram2.h>

#include <BLUtils.h>
#include <BLUtilsComp.h>


#include <ImageSmootherKernel.h>

#include <PeakDetector2D2.h>

#include <SoftMasking2.h>
#include <SoftMaskingComp2.h>

#include "DUETSeparator2.h"


#define TWO_PI 6.28318530717959

// Poderate directly with magn
#define USE_HISTO_VALUE_MAGN 0

#define THRESHOLD_COEFF 0.1 // 0.05

#define SMOOTH_KERNEL_SIZE 5 //3 //15

// Simple coefficient
#define SPREAD_PHASES_X 1
#define SPREAD_PHASES_X_COEFF 1.0 //64.0 //16.0 //4.0

#define SPREAD_PHASES_Y 1
#define SPREAD_PHASES_Y_COEFF 1.0 //16.0 //4.0 //1.0


// Artifial coeff
#define HISTO_COEFF 16.0

#define PEAK_DETECTOR_NUM_POINTS 16 //64 //16

// Seems good with 16, (maybe better with 32? but too CPU costly!)
#define SOFT_MASKING_HISTO_SIZE 8 //4 //16 //32

// NOTE: for the moment, SoftMaskingComp makes strange stereo artifacts
// => the sound is better with the 1 value.
#define APPLY_MONO_MASK 0 //1 //0

#define MAX_MASK_VALUE 1024

// Phase Aliasing Correction
//
#define PAC_OVERSAMPLE 4

// Phase Aliasing Correction, interpolation order
#define PAC_INTERP_D1 1 //0
#define PAC_INTERP_D2 0 //1 //0
#define PAC_INTERP_D3 0 ////1 //0


DUETSeparator2::DUETSeparator2(int histogramSize, BL_FLOAT sampleRate)
{
    mSampleRate = sampleRate;
    
    // Paper: 35x50    
    mHistogram = new DUETHistogram2(histogramSize, histogramSize, 1.0);
    
    mUseKernelSmoothFlag = false;
    
    //
    mThresholdFloor = 0.0;
    mThresholdPeaks = 0.0;
    mThresholdPeaksWidth = 0.0;
    
    mDispThresholded = false;
    mDispMaxima = false;
    mDispMasks = false;
    
    //
    mSmootherKernel = new ImageSmootherKernel(SMOOTH_KERNEL_SIZE);
    
    mPeakDetector = new PeakDetector2D2(PEAK_DETECTOR_NUM_POINTS);
    
    //
    mPickingActive = false;
    
    mPickX = 0.0;
    mPickY = 0.0;
    
    mInvertPickSelection = false;
    
    for (int i = 0; i < 2; i++)
        mSoftMasking[i] = new SoftMasking2(SOFT_MASKING_HISTO_SIZE);
    mUseSoftMasks = false;
    
    for (int i = 0; i < 2; i++)
        mSoftMaskingComp[i] = new SoftMaskingComp2(SOFT_MASKING_HISTO_SIZE);
    mUseSoftMasksComp = false;
    
    mUseGradientMasks = false;
    mThresholdAll = false;
    
    mAlphaZoom = 1.0;
    mDeltaZoom = 1.0;
    
    mMustResetHistogram = false;
    
    mUsePhaseAliasingCorrection = false;
}

DUETSeparator2::~DUETSeparator2()
{
    delete mHistogram;
    
    delete mSmootherKernel;
    
    delete mPeakDetector;
    
    for (int i = 0; i < 2; i++)
        delete mSoftMasking[i];
    
    for (int i = 0; i < 2; i++)
        delete mSoftMaskingComp[i];
}

void
DUETSeparator2::Reset(BL_FLOAT sampleRate)
{
    mSampleRate = sampleRate;
    
    mHistogram->Reset();
    
    for (int i = 0; i < 2; i++)
        mSoftMasking[i]->Reset();
    
    for (int i = 0; i < 2; i++)
        mSoftMaskingComp[i]->Reset();
    
    for (int i = 0; i < 2; i++)
        mPACOversampledFft[i].Resize(0);
}

void
DUETSeparator2::SetHistogramSize(int histoSize)
{
    mHistogram->Reset(histoSize, histoSize, 1.0);
    
    Reset(mSampleRate);
}

void
DUETSeparator2::SetInputData(const WDL_TypedBuf<BL_FLOAT> magns[2],
                            const WDL_TypedBuf<BL_FLOAT> phases[2])
{
    mInputMagns[0] = magns[0];
    mInputMagns[1] = magns[1];
    
    mInputPhases[0] = phases[0];
    mInputPhases[1] = phases[1];
    
#if 0
    //FillHistogram();
    FillHistogram2();
#endif
}

void
DUETSeparator2::SetInputDataComp(const WDL_TypedBuf<WDL_FFT_COMPLEX> fftData[2])
{
    mInputFftData[0] = fftData[0];
    mInputFftData[1] = fftData[1];
}

void
DUETSeparator2::Process()
{
    // Set input
    mCurrentMagns[0] = mInputMagns[0];
    mCurrentMagns[1] = mInputMagns[1];
    
    mCurrentPhases[0] = mInputPhases[0];
    mCurrentPhases[1] = mInputPhases[1];
    
    mCurrentFftData[0] = mInputFftData[0];
    mCurrentFftData[1] = mInputFftData[1];
    
    if (mMustResetHistogram)
    {
        mHistogram->Reset();
        
        mMustResetHistogram = false;
    }
    
    // Samples that are out of the bounds of the histogram
    vector<int> oobSamples;
    
#if 1
    if (!mUsePhaseAliasingCorrection)
    {
        //FillHistogram(&oobSamples);
        FillHistogram2(&oobSamples);
    }
    else
    {
        FillHistogramPAC(&oobSamples);
    }
#endif
    
    // Debug
    WDL_TypedBuf<int> dbgOobSamples;
    //dbgEobSamples.Resize(eobSamples.size());
    //for (int i = 0; i < eobSamples.size(); i++)
    //    dbgEobSamples.Get()[i] = eobSamples[i];
    dbgOobSamples.Resize(mCurrentMagns[0].GetSize());
    BLUtils::FillAllZero(&dbgOobSamples);
    for (int i = 0; i < oobSamples.size(); i++)
    {
        int idx = oobSamples[i];
        dbgOobSamples.Get()[idx] = 1e-5;
    }
    
    // The magns corresponding to out of bound samples will be set to zero.
    //
    // If we take the inverse of selection, keep magns corresponding to out ouf bounds samples.
    if (mInvertPickSelection)
        oobSamples.clear();
    
    int width = mHistogram->GetWidth();
    int height = mHistogram->GetHeight();
    mHistogram->GetData(&mCurrentHistogramData);
    
    BLUtils::MultValues(&mCurrentHistogramData, (BL_FLOAT)HISTO_COEFF);
    
    if (mUseKernelSmoothFlag)
    {
        // Smooth
        mSmootherKernel->SmoothImage(width, height, &mCurrentHistogramData);
    }
    
    WDL_TypedBuf<BL_FLOAT> thresholdedHistogram = mCurrentHistogramData;
    ThresholdHistogram(width, height, &thresholdedHistogram,
                       mThresholdFloor);
    
    bool useOnlyThreshold = false;
    if (mThresholdFloor > 0.0)
        useOnlyThreshold = true;
    
    WDL_TypedBuf<BL_FLOAT> floatMask;
    if (useOnlyThreshold)
    {
        // Create float mask directly, do not compute peaks
        HistogramToFloatMask(thresholdedHistogram, &floatMask);
        
        if (mInvertPickSelection)
        {
            InvertFloatMask(&floatMask);
        }
        
        if (mUseGradientMasks)
        {
            // Smooth
            mSmootherKernel->SmoothImage(width, height, &floatMask);
        }
        
        // Display
        if (mDispMasks)
        {
            mCurrentHistogramData = floatMask;
            
            BLUtils::MultValues(&mCurrentHistogramData, (BL_FLOAT)0.01);
        }
    }
    
    if (!useOnlyThreshold)
    {
        // Detect peaks
        //
        
        // Detect peaks
        WDL_TypedBuf<BL_FLOAT> maximaHistogram = mCurrentHistogramData;
        DetectPeaks(width, height, &maximaHistogram);
    
        WDL_TypedBuf<BL_FLOAT> maximaHistogramThrs = maximaHistogram;
        ThresholdHistogram(width, height, &maximaHistogramThrs,
                           mThresholdPeaks);
    
        vector<PeakDetector2D2::Peak> peaks;
        ExtractPeaks(width, height, maximaHistogramThrs, &peaks);
    
        WDL_TypedBuf<int> mask;
        //CreateMaskDist(width, height, thresholdedHistogram, peaks, &mask);
        CreateMaskPeakDetector(width, height, thresholdedHistogram, peaks, &mask);
    
        ComputePeaksAreas(mask, &peaks);
    
        // Display
        WDL_TypedBuf<BL_FLOAT> dbgMaskImage;
        DBG_MaskToImage(mask, &dbgMaskImage);
    
        //WDL_TypedBuf<BL_FLOAT> floatMask;
        MaskToFloatMask(mask, &floatMask);
    
        int pickValue = -1;
        if (!mThresholdAll)
            pickValue = SelectMaskPicking(width, height, &mask, &floatMask);
    
        if (mUseGradientMasks)
        {
            CreateGradientMask(width, height, mask, peaks, &floatMask);
        }
    
        // Maybe reverse 2 times, depending on the options
        // (reverse, then reverse back in some case)
        if (pickValue == 0)
        {
            InvertFloatMask(&floatMask);
        }
        
        if (mInvertPickSelection)
        {
            InvertFloatMask(&floatMask);
        }
        
        if (mPickingActive || mUseGradientMasks)
        {
            dbgMaskImage = floatMask;
            
            BLUtils::MultValues(&dbgMaskImage, (BL_FLOAT)0.01);
        }
     
        if (!mPickingActive)
        {
            // Select all
            BLUtils::FillAllValue(&floatMask, (BL_FLOAT)1.0);
        }
        
        // Display
        if (mDispThresholded)
            mCurrentHistogramData = thresholdedHistogram;
        
        if (mDispMaxima)
            mCurrentHistogramData = maximaHistogramThrs;
        
        if (mDispMasks)
            mCurrentHistogramData = dbgMaskImage;
    }
    
    if (mUseSoftMasks && !mUseSoftMasksComp)
    {
        WDL_TypedBuf<BL_FLOAT> softMasks[2];
        CreateSoftMasks(mCurrentMagns, floatMask, softMasks, &oobSamples);
        
        for (int i = 0; i < 2; i++)
        {
            int maskIndex = i;
#if APPLY_MONO_MASK
            maskIndex = 0;
#endif
            
            ApplySoftMask(width, height, softMasks[maskIndex], &mCurrentMagns[i]);
        }
    }
    else if (mUseSoftMasksComp)
    {
        WDL_TypedBuf<WDL_FFT_COMPLEX> softMasksComp[2];
        CreateSoftMasksComp(mCurrentFftData, floatMask, softMasksComp, &oobSamples);
        
        for (int i = 0; i < 2; i++)
        {
            int maskIndex = i;
#if APPLY_MONO_MASK
            maskIndex = 0;
#endif
            
            ApplySoftMaskComp(width, height, softMasksComp[maskIndex], &mCurrentFftData[i]);
        }
    }
    else
    {
        ThresholdSound(width, height, floatMask, mCurrentMagns);
    }
}

void
DUETSeparator2::GetOutputData(WDL_TypedBuf<BL_FLOAT> magns[2],
                              WDL_TypedBuf<BL_FLOAT> phases[2])
{
    magns[0] = mCurrentMagns[0];
    magns[1] = mCurrentMagns[1];
    
    phases[0] = mCurrentPhases[0];
    phases[1] = mCurrentPhases[1];
}

void
DUETSeparator2::GetOutputDataComp(WDL_TypedBuf<WDL_FFT_COMPLEX> fftData[2])
{
    fftData[0] = mCurrentFftData[0];
    fftData[1] = mCurrentFftData[1];
}

int
DUETSeparator2::GetHistogramWidth()
{
    int width = mHistogram->GetWidth();
    
    return width;
}

int
DUETSeparator2::GetHistogramHeight()
{
    int height = mHistogram->GetHeight();
    
    return height;
}

void
DUETSeparator2::GetHistogramData(WDL_TypedBuf<BL_FLOAT> *data)
{
    *data = mCurrentHistogramData;
}

void
DUETSeparator2::SetThresholdFloor(BL_FLOAT threshold)
{
    mThresholdFloor = threshold;
    
    // Hack
    //mThresholdFloor *= 2.0;
}

void
DUETSeparator2::SetThresholdPeaks(BL_FLOAT threshold)
{
    mThresholdPeaks = threshold;
}

void
DUETSeparator2::SetThresholdPeaksWidth(BL_FLOAT threshold)
{
    mThresholdPeaksWidth = threshold;
}

void
DUETSeparator2::SetDisplayThresholded(bool flag)
{
    mDispThresholded = flag;
}

void
DUETSeparator2::SetDisplayMaxima(bool flag)
{
    mDispMaxima = flag;
}

void
DUETSeparator2::SetDisplayMasks(bool flag)
{
    mDispMasks = flag;
}

void
DUETSeparator2::SetUseSoftMasks(bool flag)
{
    mUseSoftMasks = flag;
}

void
DUETSeparator2::SetUseSoftMasksComp(bool flag)
{
    mUseSoftMasksComp = flag;
}

void
DUETSeparator2::SetSoftMaskSize(int size)
{
    for (int i = 0; i < 2; i++)
        mSoftMasking[i]->SetHistorySize(size);
    
    for (int i = 0; i < 2; i++)
        mSoftMaskingComp[i]->SetHistorySize(size);
}

//
void
DUETSeparator2::SetUseGradientMasks(bool flag)
{
    mUseGradientMasks = flag;
}

void
DUETSeparator2::SetThresholdAll(bool flag)
{
    mThresholdAll = flag;
}

void
DUETSeparator2::SetPickingActive(bool flag)
{
    mPickingActive = flag;
}

void
DUETSeparator2::SetPickPosition(BL_FLOAT x, BL_FLOAT y)
{
    mPickX = x;
    mPickY = y;
}

void
DUETSeparator2::SetAlphaZoom(BL_FLOAT zoom)
{
    if (zoom > 0.0)
        mAlphaZoom = 1.0/zoom;
    
    mMustResetHistogram = true;
}

void
DUETSeparator2::SetDeltaZoom(BL_FLOAT zoom)
{
    if (zoom > 0.0)
        mDeltaZoom = 1.0/zoom;
    
    mMustResetHistogram = true;
}

void
DUETSeparator2::SetUsePhaseAliasingCorrection(bool flag)
{
    mUsePhaseAliasingCorrection = flag;
    
    //
    //Reset(mSampleRate);
    mHistogram->Reset();
}

int
DUETSeparator2::GetPACOversampling()
{
    return PAC_OVERSAMPLE;
}

void
DUETSeparator2::SetPACOversamplesFft(const WDL_TypedBuf<WDL_FFT_COMPLEX> oversampledFft[2])
{
    for (int i = 0; i < 2; i++)
        mPACOversampledFft[i] = oversampledFft[i];
}

void
DUETSeparator2::SetInvertPickSelection(bool flag)
{
    mInvertPickSelection = flag;
}

void
DUETSeparator2::SetTimeSmooth(BL_FLOAT smoothFactor)
{
    mHistogram->SetTimeSmooth(smoothFactor);
}

void
DUETSeparator2::SetUseKernelSmooth(bool kernelSmoothFlag)
{
    mUseKernelSmoothFlag = kernelSmoothFlag;
}

void
DUETSeparator2::FillHistogram(vector<int> *oobSamples)
{
#define EPS 1e-15
    
    mHistogram->Clear();
    
    for (int i = 0; i < mCurrentMagns[0].GetSize(); i++)
    {
        // Alpha
        BL_FLOAT magn0 = mCurrentMagns[0].Get()[i];
        BL_FLOAT magn1 = mCurrentMagns[1].Get()[i];
        
        //
        BL_FLOAT alpha = 0.0;
        if ((magn0 > EPS) && (magn1 > EPS))
        {
            if (magn0 >= magn1)
            {
                alpha = magn1/magn0;
                
                alpha = ((1.0 - alpha) + 1.0)*0.5;
            }
            
            if (magn1 > magn0)
            {
                alpha = magn0/magn1;
                
                alpha = alpha*0.5;
            }
        }
        
        alpha = 1.0 - alpha;
        
#if SPREAD_PHASES_Y
        alpha = (alpha - 0.5)*SPREAD_PHASES_Y_COEFF + 0.5;
#endif

        if (oobSamples != NULL)
        {
            if ((alpha < 0.0) || (alpha > 1.0))
            {
                oobSamples->push_back(i);
                
                continue;
            }
        }
        
        // Delta
        BL_FLOAT phase0 = mCurrentPhases[0].Get()[i];
        BL_FLOAT phase1 = mCurrentPhases[1].Get()[i];
        
        BL_FLOAT delta = (phase0 - phase1)*(1.0/TWO_PI);
        
#if SPREAD_PHASES_X
        delta *= SPREAD_PHASES_X_COEFF;
#endif

#if SPREAD_PHASES
        if (delta > 0.0)
            delta = BLUtils::ApplyParamShape(delta, 2.0);
        else
        {
            delta = -delta;
            delta = BLUtils::ApplyParamShape(delta, 2.0);
            delta = -delta;
        }
#endif
        
        /// Normalize
        delta = (delta + 1.0)*0.5;
        
        if (oobSamples != NULL)
        {
            if ((delta < 0.0) || (delta > 1.0))
            {
                oobSamples->push_back(i);
                
                continue;
            }
        }
        
        // Add to histogram
        BL_FLOAT value = 0.001;
#if USE_HISTO_VALUE_MAGN
        value = magn0;
#endif
        
        mHistogram->AddValue(alpha, delta, value, i);
    }
    
    //
    mHistogram->Process();
}

// More close to the paper
void
DUETSeparator2::FillHistogram2(vector<int> *oobSamples)
{
#define KEEP_OUT_BOUNDS_SAMPLES 1
    
#define EPS 1e-15
    
    mHistogram->Clear();
        
    for (int i = 0; i < mCurrentMagns[0].GetSize(); i++)
    {
        // Alpha
        BL_FLOAT magn0 = mCurrentMagns[0].Get()[i];
        BL_FLOAT magn1 = mCurrentMagns[1].Get()[i];
        
        //
        BL_FLOAT alpha = (magn1 + EPS)/(magn0 + EPS);
        
        // Symetric attenuation
        alpha = alpha - 1.0/alpha;
        
        // Niko
#define MAX_ALPHA 2.0 //5.0 //20.0 //1.0 //20.0
        // Paper
//#define MAX_ALPHA 0.7
        alpha *= 1.0/MAX_ALPHA;
        
        alpha *= mAlphaZoom;
        
        alpha = (alpha + 1.0)*0.5;
        
#if KEEP_OUT_BOUNDS_SAMPLES
        if (oobSamples == NULL)
        {
            if (alpha < 0.0)
                alpha = 0.0;
            if (alpha > 1.0)
                alpha = 1.0;
        }
        else
        {
            if ((alpha < 0.0) || (alpha > 1.0))
            {
                oobSamples->push_back(i);
                
                continue;
            }
        }
#endif
        
        // Delta
        BL_FLOAT phase0 = mCurrentPhases[0].Get()[i];
        BL_FLOAT phase1 = mCurrentPhases[1].Get()[i];
        
        BL_FLOAT delta = (phase0 - phase1);
        //delta /= TWO_PI*((BL_FLOAT)i)/(mCurrentMagns[0].GetSize() + 1);
        
        // TEST
        // See: http://mural.maynoothuniversity.ie/2318/1/Final_Thesis_Ye_Liang.pdf
        //delta = 1.0/(delta + EPS);
        //delta *= 0.01;
        
        // Niko
#define MAX_DELTA 5.0 //10.0 //20.0 //80.0 //40.0 //10.0 //40.0
        // Paper
        //#define MAX_DELTA 3.6
        delta *= 1.0/MAX_DELTA;
        
        delta *= mDeltaZoom;
        
        delta = (delta + 1.0)*0.5;
        
#if KEEP_OUT_BOUNDS_SAMPLES
        if (oobSamples == NULL)
        {
            if (delta < 0.0)
                delta = 0.0;
            if (delta > 1.0)
                delta = 1.0;
        }
        else
        {
            if ((delta < 0.0) || (delta > 1.0))
            {
                oobSamples->push_back(i);
                
                continue;
            }
        }
#endif
        
        // Weight, from the paper
        //
        // p=0,q= 0: the counting histogram proposed in the original DUETalgorithm
        // p=1,q= 0: motivated by the ML symmetric attenuation estimator
        // p=1,q= 2: motivated by the ML delay estimator
        // p=2,q= 0: in order to reduce delay estimator bias
        // p=2,q= 2: for low signal-to-noise ratio or speech mixtures
        //
        
        // Paper: (1, 0)
#define WEIGHT_P 1
#define WEIGHT_Q 0
        BL_FLOAT w0 = magn0*magn1;
        if (WEIGHT_P == 0)
        {
            w0 = 1.0;
        }
        else if (WEIGHT_P > 1)
        {
	  w0 = std::pow(w0, WEIGHT_P);
        }
        
        // See Euler formula
        // See: https://blog.endaq.com/fourier-transform-basics
        BL_FLOAT w1 = TWO_PI*((BL_FLOAT)i)/(mCurrentMagns[0].GetSize() + 1);
        if (WEIGHT_Q == 0)
        {
            w1 = 1.0;
        }
        else if (WEIGHT_Q > 1)
        {
	  w1 = std::pow(w1, WEIGHT_Q);
        }
        
        // Final weight
        BL_FLOAT weight = w0*w1;
        
        // Niko
#define WEIGHT_COEFF 128.0 //32.0
        if (WEIGHT_P > 0) // Hack
            weight *= WEIGHT_COEFF;
        else
            weight *= 0.001;
        
        mHistogram->AddValue(alpha, delta, weight, i);
    }
    
    //
    mHistogram->Process();
}

// Phase aliasing correction
// See: file:///Users/applematuer/Downloads/1-s2.0-S1063520313000043-main.pdf
//
// NOTES:
// - works well to separate peaks in BlueLab-DUET-TestReaper-VST2-TwoSine.RPP
// (we have only 2 peaks, better separated)
// - Similar to the original algorithm for BlueLab-DUET-TestReaper-VST2-DeNoise.RPP
// and BlueLab-DUET-TestReaper-VST2-DeReverb.RPP
// (to get it work, we must increase the alpha zoom to maximum, 10)
// (maybe this is due to samples out of the histogram, that make it fail a bit)
//
// => For De-Reverb and De-Noise, with global threshold, no need to use PAC version
// => For peaks detection one by one, the PAC version will be better, and ill avoid
// false peaks.
void
DUETSeparator2::FillHistogramPAC(vector<int> *oobSamples)
{
#define KEEP_OUT_BOUNDS_SAMPLES 1
    
#define EPS 1e-15
    
    mHistogram->Clear();
    
    //BL_FLOAT epsilon = TWO_PI/mPACOversampledFft[0].GetSize();
    BL_FLOAT epsilon = 1.0;
    
    for (int i = 0; i < mPACOversampledFft[0].GetSize(); i += PAC_OVERSAMPLE)
    {
        WDL_FFT_COMPLEX comp0 = mPACOversampledFft[0].Get()[i];
        WDL_FFT_COMPLEX comp1 = mPACOversampledFft[1].Get()[i];
     
        // Compute F
        WDL_FFT_COMPLEX div;
        if ((std::fabs(comp1.re) < EPS) && (std::fabs(comp1.im) < EPS))
            continue;
        COMP_DIV(comp0, comp1, div);
        
        // Alpha
        BL_FLOAT alpha = COMP_MAGN(div);
        
        // Symetric attenuation
        alpha = alpha - 1.0/alpha;
        
        // Niko
#define MAX_ALPHA 2.0 //5.0 //20.0 //1.0 //20.0
        // Paper
        //#define MAX_ALPHA 0.7
        alpha *= 1.0/MAX_ALPHA;
        
        alpha *= mAlphaZoom;
        
        alpha = (alpha + 1.0)*0.5;
        
        // For PAC only
        alpha = 1.0 - alpha;
        
#if KEEP_OUT_BOUNDS_SAMPLES
        if (oobSamples == NULL)
        {
            if (alpha < 0.0)
                alpha = 0.0;
            if (alpha > 1.0)
                alpha = 1.0;
        }
        else
        {
            if ((alpha < 0.0) || (alpha > 1.0))
            {
                oobSamples->push_back(i);
                
                continue;
            }
        }
#endif
        
        // Delta
        BL_FLOAT delta = COMP_PHASE(div);
#if PAC_INTERP_D1
        if (0) //(i < mPACOversampledFft[0].GetSize() - 1)
        {
            bool success;
            BL_FLOAT delta1 = ComputePACDelta(mPACOversampledFft, i + 1, &success);
            if (!success)
                continue;
            
            BL_FLOAT coeff = 1.0/epsilon;
            
            delta = coeff*(delta - delta1);
            
            delta = fmod(delta, mPACOversampledFft[0].GetSize());
        }
#endif

#if PAC_INTERP_D2
        if ((i > 0) && (i < mPACOversampledFft[0].GetSize() - 1))
        {
            // + epsilon
            bool success;
            BL_FLOAT delta1 = ComputePACDelta(mPACOversampledFft, i + 1, &success);
            if (!success)
                continue;
            
            // - epsilon
            BL_FLOAT deltaM1 = ComputePACDelta(mPACOversampledFft, i - 1, &success);
            if (!success)
                continue;
            
            BL_FLOAT coeff = 1.0/(2.0*epsilon);
            delta = coeff*(delta1 - deltaM1);
            delta = fmod(delta, mPACOversampledFft[0].GetSize());
        }
#endif

#if PAC_INTERP_D3
        if ((i > 1) && (i < mPACOversampledFft[0].GetSize() - 2))
        {
            // + 2*epsilon
            bool success;
            BL_FLOAT delta2 = ComputePACDelta(mPACOversampledFft, i + 2, &success);
            if (!success)
                continue;
            
            // + epsilon
            BL_FLOAT delta1 = ComputePACDelta(mPACOversampledFft, i + 1, &success);
            if (!success)
                continue;
            
            // - epsilon
            BL_FLOAT deltaM1 = ComputePACDelta(mPACOversampledFft, i - 1, &success);
            if (!success)
                continue;
            
            // - 2*epsilon
            BL_FLOAT deltaM2 = ComputePACDelta(mPACOversampledFft, i - 2, &success);
            if (!success)
                continue;
            
            BL_FLOAT coeff = 1.0/(12.0*epsilon);
            delta = coeff*(-delta2 + 8.0*delta1 - 8.0*deltaM1 + deltaM2);
            delta = fmod(delta, mPACOversampledFft[0].GetSize());
        }
#endif
        
        // Niko
#define MAX_DELTA 5.0 //10.0 //20.0 //80.0 //40.0 //10.0 //40.0
        // Paper
        //#define MAX_DELTA 3.6
        delta *= 1.0/MAX_DELTA;
        
        // Hack for PAC only
        //delta *= 4.0;
        
        delta *= mDeltaZoom;
        
        delta = (delta + 1.0)*0.5;
        
#if KEEP_OUT_BOUNDS_SAMPLES
        if (oobSamples == NULL)
        {
            if (delta < 0.0)
                delta = 0.0;
            if (delta > 1.0)
                delta = 1.0;
        }
        else
        {
            if ((delta < 0.0) || (delta > 1.0))
            {
                oobSamples->push_back(i/PAC_OVERSAMPLE);
                
                continue;
            }
        }
#endif
        
        // Weight, from the paper
        //
        // p=0,q= 0: the counting histogram proposed in the original DUETalgorithm
        // p=1,q= 0: motivated by the ML symmetric attenuation estimator
        // p=1,q= 2: motivated by the ML delay estimator
        // p=2,q= 0: in order to reduce delay estimator bias
        // p=2,q= 2: for low signal-to-noise ratio or speech mixtures
        //
        
        // Paper: (1, 0)
#define WEIGHT_P 1
#define WEIGHT_Q 0
        BL_FLOAT w0 = COMP_MAGN(comp0)*COMP_MAGN(comp1);
        if (WEIGHT_P == 0)
        {
            w0 = 1.0;
        }
        else if (WEIGHT_P > 1)
        {
	  w0 = std::pow(w0, WEIGHT_P);
        }
        
        // See Euler formula
        // See: https://blog.endaq.com/fourier-transform-basics
        BL_FLOAT w1 = TWO_PI*((BL_FLOAT)i)/(mPACOversampledFft[0].GetSize() + 1);
        if (WEIGHT_Q == 0)
        {
            w1 = 1.0;
        }
        else if (WEIGHT_Q > 1)
        {
	  w1 = std::pow(w1, WEIGHT_Q);
        }
        
        // Final weight
        BL_FLOAT weight = w0*w1;
        
        // Niko
#define WEIGHT_COEFF 128.0 //32.0
        if (WEIGHT_P > 0) // Hack
            weight *= WEIGHT_COEFF;
        else
            weight *= 0.001;
        
        mHistogram->AddValue(alpha, delta, weight, i/PAC_OVERSAMPLE);
    }
    
    //
    mHistogram->Process();
}

BL_FLOAT
DUETSeparator2::ComputePACDelta(const WDL_TypedBuf<WDL_FFT_COMPLEX> oversampledFft[2],
                                int index, bool *success)
{
    *success = true;
    
    if ((index < 0) || (index > oversampledFft[0].GetSize() - 1))
    {
        *success = false;
        return 0.0;
    }
    
    WDL_FFT_COMPLEX comp0Delta = mPACOversampledFft[0].Get()[index];
    WDL_FFT_COMPLEX comp1Delta = mPACOversampledFft[1].Get()[index];
    
    WDL_FFT_COMPLEX divDelta;
    if ((std::fabs(comp1Delta.re) < EPS) && (std::fabs(comp1Delta.im) < EPS))
    {
        *success = false;
        return 0.0;
    }
    
    COMP_DIV(comp0Delta, comp1Delta, divDelta);
    
    BL_FLOAT delta = COMP_PHASE(divDelta);
    
    // TEST
    //delta /= TWO_PI/((BL_FLOAT)index)/(oversampledFft[0].GetSize() + 1);
    
    return delta;
}

void
DUETSeparator2::ThresholdHistogram(int width, int height, WDL_TypedBuf<BL_FLOAT> *data,
                                  BL_FLOAT threshold)
{
    for (int i = 0; i < data->GetSize(); i++)
    {
        BL_FLOAT val = data->Get()[i];
        
        if (val < threshold*THRESHOLD_COEFF)
            val = 0.0;
        
        data->Get()[i] = val;
    }
}

void
DUETSeparator2::ThresholdSound(int width, int height,
                               const WDL_TypedBuf<BL_FLOAT> &floatMask,
                               WDL_TypedBuf<BL_FLOAT> magns[2])
{
    for (int i = 0; i < floatMask.GetSize(); i++)
    {
        BL_FLOAT maskVal = floatMask.Get()[i];
        
        vector<int> indices;
        mHistogram->GetIndices(i, &indices);
            
        for (int j = 0; j < indices.size(); j++)
        {
            int idx = indices[j];
                
            if ((idx >= 0) && (idx < magns[0].GetSize()))
            {
                magns[0].Get()[idx] *= maskVal;
                magns[1].Get()[idx] *= maskVal;
            }
        }
    }
}

void
DUETSeparator2::ApplySoftMask(int width, int height,
                             const WDL_TypedBuf<BL_FLOAT> &mask,
                             WDL_TypedBuf<BL_FLOAT> *magns)
{
    for (int i = 0; i < mask.GetSize(); i++)
    {
        BL_FLOAT maskVal = mask.Get()[i];
        BL_FLOAT val = magns->Get()[i];
        
        val *= maskVal;
        
        magns->Get()[i] = val;
    }
}

void
DUETSeparator2::ApplySoftMaskComp(int width, int height,
                                 const WDL_TypedBuf<WDL_FFT_COMPLEX> &mask,
                                 WDL_TypedBuf<WDL_FFT_COMPLEX> *fftData)
{
    for (int i = 0; i < mask.GetSize(); i++)
    {
        WDL_FFT_COMPLEX maskVal = mask.Get()[i];
        WDL_FFT_COMPLEX val = fftData->Get()[i];
        
        WDL_FFT_COMPLEX res;
        COMP_MULT(val, maskVal, res);
        
        fftData->Get()[i] = res;
    }
}

void
DUETSeparator2::ApplyGradientMask(int width, int height,
                                  const WDL_TypedBuf<BL_FLOAT> &mask,
                                  WDL_TypedBuf<BL_FLOAT> *magns)
{
    for (int i = 0; i < mask.GetSize(); i++)
    {
        BL_FLOAT maskVal = mask.Get()[i];
        
        vector<int> indices;
        mHistogram->GetIndices(i, &indices);
            
        for (int j = 0; j < indices.size(); j++)
        {
            int idx = indices[j];
                
            if ((idx >= 0) && (idx < magns->GetSize()))
            {
                BL_FLOAT magn = magns->Get()[idx];
                
                magn *= maskVal;
                
                magns->Get()[idx] = magn;
            }
        }
    }
}

void
DUETSeparator2::ApplyGradientMaskComp(int width, int height,
                                      const WDL_TypedBuf<BL_FLOAT> &mask,
                                      WDL_TypedBuf<WDL_FFT_COMPLEX> *fftData)
{
    for (int i = 0; i < mask.GetSize(); i++)
    {
        BL_FLOAT maskVal = mask.Get()[i];
        
        vector<int> indices;
        mHistogram->GetIndices(i, &indices);
        
        for (int j = 0; j < indices.size(); j++)
        {
            int idx = indices[j];
            
            if ((idx >= 0) && (idx < fftData->GetSize()))
            {
                WDL_FFT_COMPLEX fftVal = fftData->Get()[idx];
                
                fftVal.re *= maskVal;
                fftVal.im *= maskVal;
                
                fftData->Get()[idx] = fftVal;
            }
        }
    }
}

void
DUETSeparator2::DetectPeaks(int width, int height,
                           WDL_TypedBuf<BL_FLOAT> *data)
{
    BLUtils::FindMaxima2D(width, height, data, true);
}

void
DUETSeparator2::ExtractPeaks(int width, int height,
                             const WDL_TypedBuf<BL_FLOAT> &histogramData,
                             vector<PeakDetector2D2::Peak> *peaks)
{
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            BL_FLOAT val = histogramData.Get()[i + j*width];
            if (val > 0.0)
            {
                PeakDetector2D2::Peak p;
                p.mX = i;
                p.mY = j;
                
                p.mId = peaks->size() + 1;
                
                p.mIntensity = val;
                
                peaks->push_back(p);
            }
        }
    }
}

void
DUETSeparator2::CreateMaskDist(int width, int height,
                              const WDL_TypedBuf<BL_FLOAT> &histogramData,
                              const vector<PeakDetector2D2::Peak> &peaks,
                              WDL_TypedBuf<BL_FLOAT> *mask)
{
    mask->Resize(histogramData.GetSize());
    BLUtils::FillAllZero(mask);
    
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            BL_FLOAT val = histogramData.Get()[i + j*width];
            
            if (val > 0.0)
            {
                int closestIdx = -1;
                BL_FLOAT closestDistance2 = width*width;
                
                for (int k = 0; k < peaks.size(); k++)
                {
                    const PeakDetector2D2::Peak &p = peaks[k];
                    
                    BL_FLOAT dist2 = (p.mX - i)*(p.mX - i) + (p.mY - j)*(p.mY - j);
                    if (dist2 < closestDistance2)
                    {
                        closestDistance2 = dist2;
                        
                        closestIdx = p.mId;
                    }
                }
                
                mask->Get()[i + j*width] = closestIdx;
            }
        }
    }
}

void
DUETSeparator2::CreateMaskPeakDetector(int width, int height,
                                      const WDL_TypedBuf<BL_FLOAT> &histogramData,
                                      const vector<PeakDetector2D2::Peak> &peaks,
                                      WDL_TypedBuf<int> *mask)
{
    mPeakDetector->DetectPeaksNoOverlap(width, height, histogramData,
                                        peaks, mThresholdPeaks,
                                        mThresholdPeaksWidth, mask);
}

void
DUETSeparator2::ComputePeaksAreas(const WDL_TypedBuf<int> &mask,
                                 vector<PeakDetector2D2::Peak> *peaks)
{    
    for (int i = 0; i < peaks->size(); i++)
    {
        PeakDetector2D2::Peak &peak = (*peaks)[i];
        
        BL_FLOAT area = 0.0;
        for (int j = 0; j < mask.GetSize(); j++)
        {
            int val = mask.Get()[j];
            
            if (val == peak.mId)
                area++;
        }
        
        // Normalize
        if (mask.GetSize() > 0)
            area /= mask.GetSize();
        
        peak.mArea = area;
    }
}

void
DUETSeparator2::MaskToFloatMask(const WDL_TypedBuf<int> &mask,
                                WDL_TypedBuf<BL_FLOAT> *floatMask)
{
    floatMask->Resize(mask.GetSize());
    BLUtils::FillAllZero(floatMask);
    
    for (int i = 0; i < mask.GetSize(); i++)
    {
        int val = mask.Get()[i];
        if (val > 0)
            floatMask->Get()[i] = 1.0;
    }
}

void
DUETSeparator2::SelectMask(WDL_TypedBuf<int> *mask, int peakId)
{
    for (int i = 0; i < mask->GetSize(); i++)
    {
        int val = mask->Get()[i];
        
        if (peakId >= 0)
        {
            if (val != peakId)
                mask->Get()[i] = 0;
        }
        else
        {
            if (val == 0)
                mask->Get()[i] = MAX_MASK_VALUE;
            else
                mask->Get()[i] = 0;
        }
    }
}

int
DUETSeparator2::SelectMaskPicking(int width, int height,
                                  WDL_TypedBuf<int> *mask,
                                  WDL_TypedBuf<BL_FLOAT> *floatMask)
{
    if (!mPickingActive)
    {
        return -1;
    }
    
    int x = mPickX*width;
    int y = mPickY*height;
    
    if ((x < 0) || (x >= width) ||
        (y < 0) || (y >= height))
    {
        return -1;
    }
    
    int pickValue = mask->Get()[x + y*width];
    
    for (int i = 0; i < mask->GetSize(); i++)
    {
        int val = mask->Get()[i];
        
        if (pickValue > 0)
        {
            if (val != pickValue)
            {
                mask->Get()[i] = 0;
                floatMask->Get()[i] = 0.0;
            }
        }
    }
    
    return pickValue;
}

void
DUETSeparator2::DBG_MaskToImage(const WDL_TypedBuf<int> &mask,
                               WDL_TypedBuf<BL_FLOAT> *image)
{
    image->Resize(mask.GetSize());
    BLUtils::FillAllZero(image);
    
    for (int i = 0; i < mask.GetSize(); i++)
    {
        int val = mask.Get()[i];
        if (val > 0)
        {
            // Let's say we won't have more than 100 mask different values
            BL_FLOAT col = (val % 5 + 1)*0.01;
            
            image->Get()[i] = col;
        }
    }
}

void
DUETSeparator2::CreateGradientMask(int width, int height,
                                   const WDL_TypedBuf<int> &mask,
                                   const vector<PeakDetector2D2::Peak> &peaks,
                                   WDL_TypedBuf<BL_FLOAT> *floatMask)
{    
    for (int k = 0; k < peaks.size(); k++)
    {
        const PeakDetector2D2::Peak &peak = peaks[k];
        
        int id = peak.mId;
        int x = peak.mX;
        int y = peak.mY;
        
        BL_FLOAT maxDist2 = peak.mArea*mask.GetSize();
        
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                int maskVal = mask.Get()[i + j*width];
                if (maskVal == id)
                {
                    BL_FLOAT dist2 = (i - x)*(i - x) + (j - y)*(j- y);
                    
                    BL_FLOAT t = 1.0;
                    if (maxDist2 > EPS)
                    {
                        t = dist2 / maxDist2;
                        t = 1.0 - t;
                        
                        if (t < 0.0)
                            t = 0.0;
                        if (t > 1.0)
                            t = 1.0;
                    }
                    
                    floatMask->Get()[i + j*width] = t;
                }
            }
        }
    }
}

void
DUETSeparator2::HistogramToFloatMask(const WDL_TypedBuf<BL_FLOAT> &thresholdedHisto,
                                     WDL_TypedBuf<BL_FLOAT> *floatMask)
{
    floatMask->Resize(thresholdedHisto.GetSize());
    BLUtils::FillAllZero(floatMask);
    
    for (int i = 0; i < thresholdedHisto.GetSize(); i++)
    {
        BL_FLOAT val = thresholdedHisto.Get()[i];
        
        if (val > 0.0)
            floatMask->Get()[i] = 1.0;
    }
}

void
DUETSeparator2::InvertFloatMask(WDL_TypedBuf<BL_FLOAT> *floatMask)
{
    for (int i = 0; i < floatMask->GetSize(); i++)
    {
        BL_FLOAT val = floatMask->Get()[i];
        val = 1.0 - val;
        floatMask->Get()[i] = val;
    }
}

void
DUETSeparator2::CreateSoftMasks(const WDL_TypedBuf<BL_FLOAT> magns[2],
                                const WDL_TypedBuf<BL_FLOAT> &mask,
                                WDL_TypedBuf<BL_FLOAT> softMasks[2],
                                const vector<int> *oobSamples)
{
    // Create masks
    //
    
    WDL_TypedBuf<BL_FLOAT> thrsMagns[2];
    
    // By default, fill all
    thrsMagns[0] = magns[0];
    thrsMagns[1] = magns[1];
    
    // Then mute appropriate samples
    for (int i = 0; i < mask.GetSize(); i++)
    {
        BL_FLOAT maskVal = mask.Get()[i];
        
        vector<int> indices;
        mHistogram->GetIndices(i, &indices);
            
        for (int j = 0; j < indices.size(); j++)
        {
            int idx = indices[j];
                
            if ((idx >= 0) && (idx < magns[0].GetSize()))
            {
                thrsMagns[0].Get()[idx] *= maskVal;
                thrsMagns[1].Get()[idx] *= maskVal;
            }
        }
    }
    
    // Set to zero out of bounds samples.
    if (oobSamples != NULL)
    {
        for (int i = 0; i < oobSamples->size(); i++)
        {
            int idx = (*oobSamples)[i];
            
            if ((idx >= 0) && (idx < magns[0].GetSize()))
            {
                thrsMagns[0].Get()[idx] *= 0.0;
                thrsMagns[1].Get()[idx] *= 0.0;
            }
        }
    }
    
    // Create soft masks
    for (int i = 0; i < 2; i++)
    {
        mSoftMasking[i]->Process(magns[i], thrsMagns[i], &softMasks[i]);
    }
}

void
DUETSeparator2::CreateSoftMasksComp(const WDL_TypedBuf<WDL_FFT_COMPLEX> fftData[2],
                                    const WDL_TypedBuf<BL_FLOAT> &mask,
                                    WDL_TypedBuf<WDL_FFT_COMPLEX> softMasks[2],
                                    const vector<int> *oobSamples)
{
    // Create masks
    //
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> thrsFftData[2];
    
    // By default, fill all
    thrsFftData[0] = fftData[0];
    thrsFftData[1] = fftData[1];
    
    // Then mute appropriate samples
    for (int i = 0; i < mask.GetSize(); i++)
    {
        BL_FLOAT maskVal = mask.Get()[i];
        
        vector<int> indices;
        mHistogram->GetIndices(i, &indices);
            
        for (int j = 0; j < indices.size(); j++)
        {
            int idx = indices[j];
                
            if ((idx >= 0) && (idx < fftData[0].GetSize()))
            {
                thrsFftData[0].Get()[idx].re *= maskVal;
                thrsFftData[0].Get()[idx].im *= maskVal;
                    
                thrsFftData[1].Get()[idx].re *= maskVal;
                thrsFftData[1].Get()[idx].im *= maskVal;
            }
        }
    }
    
    // Set to zero out of bounds samples.
    if (oobSamples != NULL)
    {
        for (int i = 0; i < oobSamples->size(); i++)
        {
            int idx = (*oobSamples)[i];
            
            if ((idx >= 0) && (idx < fftData[0].GetSize()))
            {
                thrsFftData[0].Get()[idx].re *= 0.0;
                thrsFftData[0].Get()[idx].im *= 0.0;
                
                thrsFftData[1].Get()[idx].re *= 0.0;
                thrsFftData[1].Get()[idx].im *= 0.0;
            }
        }
    }
    
    // Create soft masks
    for (int i = 0; i < 2; i++)
    {
        mSoftMaskingComp[i]->Process(fftData[i], thrsFftData[i], &softMasks[i]);
    }
}
