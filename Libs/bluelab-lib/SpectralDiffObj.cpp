#include <BLUtils.h>

#include "SpectralDiffObj.h"

SpectralDiffObj::SpectralDiffObj(int bufferSize, int oversampling, int freqRes,
                                 BL_FLOAT sampleRate)
: ProcessObj(bufferSize)
{
    ProcessObj::Reset(bufferSize, oversampling, freqRes, sampleRate);
}

SpectralDiffObj::~SpectralDiffObj() {}

void
SpectralDiffObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                                  const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer)
{
    // Signal
    WDL_TypedBuf<BL_FLOAT> signalMagns;
    WDL_TypedBuf<BL_FLOAT> signalPhases;
    BLUtils::ComplexToMagnPhase(&signalMagns, &signalPhases, *ioBuffer);
  
    // Side chain in input
    mOutSignal0 = signalMagns;
  
    // Sc signal
    WDL_TypedBuf<WDL_FFT_COMPLEX> scFftSamples;
    scFftSamples.Resize(mBufferSize);
  
    WDL_TypedBuf<BL_FLOAT> scMagns;
    WDL_TypedBuf<BL_FLOAT> scPhases;
    if ((scBuffer != NULL) && (scBuffer->GetSize() == mBufferSize))
    {
        // We have sidechain
        BLUtils::ComplexToMagnPhase(&scMagns, &scPhases, *scBuffer);
    
        mOutSignal1 = scMagns;
    }
  
    WDL_TypedBuf<BL_FLOAT> resultMagns;
  
    if (signalMagns.GetSize() >= mBufferSize/2)
    {
        resultMagns.Resize(mBufferSize/2);
        for (int i = 0; i < mBufferSize/2; i++)
        {
            BL_FLOAT signalMagn = signalMagns.Get()[i];
    
            BL_FLOAT newMagn = signalMagn;
            if (scMagns.GetSize() == mBufferSize)
                // We have sidechain
            {
                BL_FLOAT scMagn = scMagns.Get()[i];
    
                newMagn = scMagn - signalMagn;
                newMagn = fabs(newMagn);
      
                if (newMagn > 1.0)
                    newMagn = 1.0;
            }
    
            resultMagns.Get()[i] = newMagn;
        }
    }
  
    WDL_TypedBuf<BL_FLOAT> resultMagnsSym;
    BLUtils::MakeSymmetry(&resultMagnsSym, resultMagns);
  
    // Result
    mOutDiff = resultMagns;
  
#if !NO_SOUND_OUTPUT
    BLUtils::MagnPhaseToComplex(ioBuffer, resultMagnsSym, signalPhases);
#endif
}

void
SpectralDiffObj::GetSignal0BufferSpect(WDL_TypedBuf<BL_FLOAT> *ioBuffer)
{
    *ioBuffer = mOutSignal0;
}

void
SpectralDiffObj::GetSignal1BufferSpect(WDL_TypedBuf<BL_FLOAT> *ioBuffer)
{
    *ioBuffer = mOutSignal1;
}

void
SpectralDiffObj::GetDiffSpect(WDL_TypedBuf<BL_FLOAT> *ioBuffer)
{
    *ioBuffer = mOutDiff;
}
