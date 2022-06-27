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
 
#ifndef NOISE_REMOVER_OBJ_H 
#define NOISE_REMOVER_OBJ_H 

 #include <deque>
using namespace std;

#include "IPlug_include_in_plug_hdr.h"

#include <BLTypes.h>
#include <FftProcessObj16.h>

using namespace iplug;

//class SoftMaskingComp4;
class SISplitter;
class Scale;
class SoftMaskingComp4;
class SmoothAvgHistogram3;

class NoiseRemoverObj : public ProcessObj
{
public:
    NoiseRemoverObj(int bufferSize, int overlap, BL_FLOAT sampleRate);
    
    virtual ~NoiseRemoverObj();
    
    void Reset(int bufferSize, int ovelap, int freqRes,
               BL_FLOAT sampleRate) override;

    void SetRatio(BL_FLOAT ratio);
    void SetUseSoftMasks(bool flag);
    void SetNoiseFloorOffset(BL_FLOAT offset);
    void SetResolution(int reso);
    void SetNoiseSmoothTimeMs(BL_FLOAT smoothTimeMs);
    
    void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                          const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer = NULL) override;

    void GetInputSignalFft(WDL_TypedBuf<BL_FLOAT> *magns);
    void GetSignalFft(WDL_TypedBuf<BL_FLOAT> *magns);
    void GetNoiseFft(WDL_TypedBuf<BL_FLOAT> *magns);
    
    int GetLatency();
    
protected:
    // w/o soft masking
    void Process(WDL_TypedBuf<BL_FLOAT> *magns);
    // w/ soft masking
    void ProcessAndSoftMasks(const WDL_TypedBuf<BL_FLOAT> &magns,
                             WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer);

    void SmoothTimeSigNoise(const vector<BL_FLOAT> &magns,
                            vector<BL_FLOAT> *sig,
                            vector<BL_FLOAT> *noise);

    void ApplyOffset(const vector<BL_FLOAT> &magns,
                     vector<BL_FLOAT> *sig,
                     vector<BL_FLOAT> *noise);
        
    SISplitter *_splitter;
    
    BL_FLOAT _ratio;
    BL_FLOAT _noiseFloorOffset;
    
    Scale *_scale;

    bool mUseSoftMasks;
    SoftMaskingComp4 *mSoftMaskingComp;
    
    // magns
    WDL_TypedBuf<BL_FLOAT> _inputSignalFft;
    WDL_TypedBuf<BL_FLOAT> _signalFft;
    WDL_TypedBuf<BL_FLOAT> _noiseFft;

    //SmoothAvgHistogram2 *_sigHisto;
    SmoothAvgHistogram3 *_noiseHisto;
    
private:
    WDL_TypedBuf<WDL_FFT_COMPLEX> mTmpBuf0;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf1;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf2;
    WDL_TypedBuf<WDL_FFT_COMPLEX> mTmpBuf3;
    WDL_TypedBuf<WDL_FFT_COMPLEX> mTmpBuf4;
    WDL_TypedBuf<WDL_FFT_COMPLEX> mTmpBuf5;
};

#endif
