/* Copyright (C) 2022 Nicolas Dittlo <deadlab.plugins@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this software; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
#include <BLUtils.h>
#include <BLUtilsComp.h>
#include <BLUtilsFft.h>
#include <BLUtilsMath.h>

#include <SISplitter.h>
#include <Scale.h>

#include <SoftMaskingComp4.h>

#include <SmoothAvgHistogram3.h>

#include <BLDebug.h>

#include "NoiseRemoverObj.h"

// 8 gives more gating, but less musical noise remaining
#define SOFT_MASKING_HISTO_SIZE 8

// Set bin #0 to 0 after soft masking
//
// FIX: fixed output peak at bin 0 when harmo only
// (this was due to noise result not at 0 for bin #0)
#define SOFT_MASKING_FIX_BIN0 1

#define DEFAULT_SMOOTH_TIME_MS 0.0

NoiseRemoverObj::NoiseRemoverObj(int bufferSize, int overlap,
                                 BL_FLOAT sampleRate)
: ProcessObj(bufferSize)
{
    // Output only signal
    _ratio = 1.0;
    _noiseFloorOffset = 0.0;
    
    _splitter = new SISplitter();

    _scale = new Scale();

    mUseSoftMasks = false;
    mSoftMaskingComp = new SoftMaskingComp4(bufferSize, overlap,
                                            SOFT_MASKING_HISTO_SIZE);

    //_sigHisto = new SmoothAvgHistogram3(sampleRate, bufferSize*0.5,
    //                                    DEFAULT_SMOOTH_TIME_MS, 0.0);
    _noiseHisto = new SmoothAvgHistogram3(sampleRate, bufferSize*0.5,
                                          DEFAULT_SMOOTH_TIME_MS, 0.0);
}
    
NoiseRemoverObj::~NoiseRemoverObj()
{
    delete _splitter;
    delete _scale;

    if (mSoftMaskingComp != NULL)
        delete mSoftMaskingComp;

    //delete _sigHisto;
    delete _noiseHisto;
}

void
NoiseRemoverObj::Reset(int bufferSize, int overlap,
                       int freqRes, BL_FLOAT sampleRate)
{
    ProcessObj::Reset(bufferSize, overlap, freqRes, sampleRate);

    if (mSoftMaskingComp != NULL)
        mSoftMaskingComp->Reset(bufferSize, overlap);

    //_sigHisto->Reset(sampleRate);
    _noiseHisto->Reset(sampleRate);
}

void
NoiseRemoverObj::SetRatio(BL_FLOAT ratio)
{
    _ratio = ratio;
}

void
NoiseRemoverObj::SetUseSoftMasks(bool flag)
{
    mUseSoftMasks = flag;
}

void
NoiseRemoverObj::SetNoiseFloorOffset(BL_FLOAT offset)
{
    //_splitter->setOffset(offset);
    _noiseFloorOffset = offset;
}

void
NoiseRemoverObj::SetResolution(int reso)
{
    _splitter->setResolution(reso);
}

void
NoiseRemoverObj::SetNoiseSmoothTimeMs(BL_FLOAT smoothTimeMs)
{
    //_sigHisto->SetSmoothTimeMs(smoothTimeMs);
    _noiseHisto->SetSmoothTimeMs(smoothTimeMs, false);
}

void
NoiseRemoverObj::
ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                 const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
    WDL_TypedBuf<WDL_FFT_COMPLEX> &ioBuffer0 = mTmpBuf0;
    BLUtils::TakeHalf(*ioBuffer, &ioBuffer0);

    WDL_TypedBuf<BL_FLOAT> &magns = mTmpBuf1;
    WDL_TypedBuf<BL_FLOAT> &phases = mTmpBuf2;
    BLUtilsComp::ComplexToMagnPhase(&magns, &phases, ioBuffer0);
    
    // Process
    if (!mUseSoftMasks)
    {
        Process(&magns);
        BLUtilsComp::MagnPhaseToComplex(&ioBuffer0, magns, phases);
    }
    else
        ProcessAndSoftMasks(magns, &ioBuffer0);
    
    BLUtilsFft::FillSecondFftHalf(ioBuffer0, ioBuffer);
}

void
NoiseRemoverObj::GetInputSignalFft(WDL_TypedBuf<BL_FLOAT> *magns)
{
    *magns = _inputSignalFft;
}

void
NoiseRemoverObj::GetSignalFft(WDL_TypedBuf<BL_FLOAT> *magns)
{
    *magns = _signalFft;
}

void
NoiseRemoverObj::GetNoiseFft(WDL_TypedBuf<BL_FLOAT> *magns)
{
    *magns = _noiseFft;
}

int
NoiseRemoverObj::GetLatency()
{
    if (mUseSoftMasks)
    {
        int latency = mSoftMaskingComp->GetLatency();
   
        return latency;
    }
    
    return 0;
}

void
NoiseRemoverObj::Process(WDL_TypedBuf<BL_FLOAT> *magns)
{
    WDL_TypedBuf<BL_FLOAT> magnsCopy0 = *magns;
    _scale->ApplyScaleFilterBank(Scale::FILTER_BANK_MEL,
                                 magns, magnsCopy0,
                                 mSampleRate,
                                 magns->GetSize());

    // Display
    _inputSignalFft.Resize(magns->GetSize());
    BLUtils::CopyBuf(&_inputSignalFft, *magns);
    
    vector<BL_FLOAT> vec_magns;
    BLUtils::CopyBuf(&vec_magns, *magns);
    
    vector<BL_FLOAT> sig;
    vector<BL_FLOAT> noise;
    _splitter->split(vec_magns, &sig, &noise);

    SmoothTimeSigNoise(vec_magns, &sig, &noise);

    ApplyOffset(vec_magns, &sig, &noise);
        
    // Display
    BLUtils::CopyBuf(&_signalFft, sig);
    BLUtils::CopyBuf(&_noiseFft, noise);
    
    BLUtils::CopyBuf(magns, vec_magns);
    for (int i = 0; i < magns->GetSize(); i++)
    {
        float s = sig[i];
        float n = noise[i];

        float m = s*_ratio + n*(1.0 - _ratio);

        magns->Get()[i] = m;
    }
            // Result
    WDL_TypedBuf<BL_FLOAT> magnsCopy1 = *magns;
    _scale->ApplyScaleFilterBankInv(Scale::FILTER_BANK_MEL,
                                    magns, magnsCopy1,
                                    mSampleRate,
                                    magns->GetSize());
}

void
NoiseRemoverObj::ProcessAndSoftMasks(const WDL_TypedBuf<BL_FLOAT> &magns,
                                     WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer)
{    
    WDL_TypedBuf<BL_FLOAT> magnsMel = magns;
    _scale->ApplyScaleFilterBank(Scale::FILTER_BANK_MEL,
                                 &magnsMel, magns,
                                 mSampleRate,
                                 magns.GetSize());
    
    // Display
    _inputSignalFft.Resize(magnsMel.GetSize());
    BLUtils::CopyBuf(&_inputSignalFft, magnsMel);
    
    vector<BL_FLOAT> vec_magns;
    BLUtils::CopyBuf(&vec_magns, magnsMel);
    
    vector<BL_FLOAT> sig;
    vector<BL_FLOAT> noise;
    _splitter->split(vec_magns, &sig, &noise);

    SmoothTimeSigNoise(vec_magns, &sig, &noise);

    ApplyOffset(vec_magns, &sig, &noise);
    
    // Display
    BLUtils::CopyBuf(&_signalFft, sig);
    BLUtils::CopyBuf(&_noiseFft, noise);

    // inverse mel on signal masl
    WDL_TypedBuf<BL_FLOAT> wdlSig = _signalFft;
    
    WDL_TypedBuf<BL_FLOAT> mask = wdlSig;
    
    _scale->ApplyScaleFilterBankInv(Scale::FILTER_BANK_MEL,
                                    &mask, wdlSig,
                                    mSampleRate,
                                    mask.GetSize());

    for (int i = 0; i < mask.GetSize(); i++)
    {
        BL_FLOAT input = magns.Get()[i];

        if (input > BL_EPS)
            mask.Get()[i] /= input;
    }
        
    WDL_TypedBuf<WDL_FFT_COMPLEX> &softMaskedResult0 = mTmpBuf3;
    WDL_TypedBuf<WDL_FFT_COMPLEX> &softMaskedResult1 = mTmpBuf4;
    mSoftMaskingComp->ProcessCentered(ioBuffer, mask,
                                      &softMaskedResult0, &softMaskedResult1);

    for (int i = 0; i < ioBuffer->GetSize(); i++)
    {
        WDL_FFT_COMPLEX s = softMaskedResult0.Get()[i];
        WDL_FFT_COMPLEX n = softMaskedResult1.Get()[i];

        WDL_FFT_COMPLEX m;
        m.re = s.re*_ratio + n.re*(1.0 - _ratio);
        m.im = s.im*_ratio + n.im*(1.0 - _ratio);

        ioBuffer->Get()[i] = m;
    }
}

void
NoiseRemoverObj::SmoothTimeSigNoise(const vector<BL_FLOAT> &magns,
                                    vector<BL_FLOAT> *sig,
                                    vector<BL_FLOAT> *noise)
{
    // smooth noise and recopute sig
    _noiseHisto->AddValues(*noise);
    _noiseHisto->GetValues(noise);
    for (int i = 0; i < sig->size(); i++)
    {
        (*sig)[i] = magns[i] - (*noise)[i];
        if ((*sig)[i] < 0.0)
            (*sig)[i] = 0.0;
    }
}

void
NoiseRemoverObj::ApplyOffset(const vector<BL_FLOAT> &magns,
                             vector<BL_FLOAT> *sig,
                             vector<BL_FLOAT> *noise)
{    
    for (int i = 0; i < noise->size(); i++)
    {
        (*noise)[i] += _noiseFloorOffset;
        if ((*noise)[i] > 1.0)
            (*noise)[i] = 1.0;
        if ((*noise)[i] < 0.0)
            (*noise)[i] = 0.0;

        (*sig)[i] = magns[i] - (*noise)[i];
        if ((*sig)[i] > 1.0)
            (*sig)[i] = 1.0;
        if ((*sig)[i] < 0.0)
            (*sig)[i] = 0.0;
    }
}
