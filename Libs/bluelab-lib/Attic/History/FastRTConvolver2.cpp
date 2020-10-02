//
//  FastRTConvolver2.cpp
//  UST
//
//  Created by applematuer on 12/28/19.
//
//

#include <ConvolutionManager.h>

#include "FastRTConvolver2.h"

// NOTE: bad, too slow to change buffer size on the fly
//
// On Reaper, when the computer slows down, the samples provided
// is less thant the declared blockSize
#define FIX_VARIABLE_BUFFER_SIZE 0 //1

// NOTE: makes a shift after some time, when playing a short loop
// several times (Reaper)
//
// FIX: On Reaper, at end of selection, the provided samples size
// can be less thant the official blockSize
// In this case, pad the input with zeros to match the blockSize
#define FIX_VARIABLE_BUFFER_SIZE2 0 //1

// GOOD: makes no shift when looping
//
// FIX: On Reaper, at end of selection, the provided samples size
// can be less thant the official blockSize
// In this case, return zeros if no input is available
//
// (crash sometimes if not set)
#define FIX_VARIABLE_BUFFER_SIZE3 1

// Make a small fade when we complete with zeros, to avoid a click
// (for example at the end of a loop)
// NOTE: useless if we managed to compute  0 values
#define FIX_VARIABLE_BUFFER_SIZE3_FADE 0 //1

// FIX: FLStudio, AU, Impulses: launch Impulses, choose Apply, play => crash
// FLStudio makes strange things with nFrames and block size
// (it seems to not declare correctly block size...)
// So make a security check.
// It seems to fix and work well like this.
#define FIX_CRASH_FLSTUDIO_MAC_AU 1

FastRTConvolver2::FastRTConvolver2(int bufferSize,
                                   const WDL_TypedBuf<BL_FLOAT> &ir,
                                   Mode mode)
{
    mBufferSize = bufferSize;
    mCurrentIr = ir;
    
    mConvMan = new ConvolutionManager(ir.Get(), ir.GetSize(), bufferSize);
    
    //mBlockSize = BUFFER_SIZE; // 512
    mBlockSize = bufferSize; // From FastRTConvolver3
    
    mSampleNum = 0;
    
    mMode = mode;
}

FastRTConvolver2::~FastRTConvolver2()
{
    delete mConvMan;
}

void
FastRTConvolver2::Reset(int blockSize)
{
    delete mConvMan;
    
    mConvMan = new ConvolutionManager(mCurrentIr.Get(),
                                      mCurrentIr.GetSize(),
                                      mBufferSize);
    
    mInSamples.Resize(0);
    mOutSamples.Resize(0);
    
    mBlockSize = blockSize;
    ComputeBufferSize(blockSize);
    
    mSampleNum = 0;
}

int
FastRTConvolver2::GetLatency()
{
    //int latency = mBlockSize - mBufferSize;
    int latency = 0;
    if (mBlockSize != mBufferSize)
    {
        latency = mBlockSize;
    }
    
    return latency;
}

void
FastRTConvolver2::SetBufferSize(int bufferSize)
{
    mBufferSize = bufferSize;
    
    mConvMan->setBufferSize(bufferSize);
}

void
FastRTConvolver2::SetIR(const WDL_TypedBuf<BL_FLOAT> &ir)
{
    mCurrentIr = ir;
    
    mConvMan->setImpulseResponse(ir.Get(), ir.GetSize());
}

void
FastRTConvolver2::Process(const WDL_TypedBuf<BL_FLOAT> &samples,
                          WDL_TypedBuf<BL_FLOAT> *result)
{
    // check size
    if (result->GetSize() != samples.GetSize())
        result->Resize(samples.GetSize());
    
#if FIX_VARIABLE_BUFFER_SIZE
    // Check, in case samples size is not the same as the blockSize
    // declared by the host
    ComputeBufferSize(samples.GetSize());
#endif
    
    // Add in samples
    mInSamples.Add(samples.Get(), samples.GetSize());

#if FIX_VARIABLE_BUFFER_SIZE2
    if (samples.GetSize() != mBlockSize)
    {
        int numZeros = mBlockSize - samples.GetSize();
        if (numZeros > 0)
            BLUtils::PadZerosRight(&mInSamples, numZeros);
    }
#endif
    
    // Process as many samples as possible
    while(mInSamples.GetSize() >= mBufferSize)
    {
        // Prepare the buffer to process
        WDL_TypedBuf<BL_FLOAT> inBuf;
        inBuf.Add(mInSamples.Get(), mBufferSize);
        
        BLUtils::ConsumeLeft(&mInSamples, mBufferSize);
        
        WDL_TypedBuf<BL_FLOAT> outBuf;
        if (mMode == NORMAL)
        {
            // Process
            mConvMan->processInput(inBuf.Get());
        
            // Output
            outBuf.Resize(mBufferSize);
            const BL_FLOAT *res = mConvMan->getOutputBuffer();
        
            memcpy(outBuf.Get(), res, outBuf.GetSize()*sizeof(BL_FLOAT));
        }
        else
        {
            outBuf = inBuf;
        }
        
        mOutSamples.Add(outBuf.Get(), outBuf.GetSize());
    }
    
    // Manage latency
    int latency = GetLatency();
    
    int numZeros = latency - mSampleNum;
    if (numZeros < 0)
        numZeros = 0;
    
    WDL_TypedBuf<BL_FLOAT> outRes;
    if (numZeros > 0)
        BLUtils::ResizeFillZeros(&outRes, numZeros);
    
    int numOutValues = samples.GetSize() - numZeros;
    
#if FIX_VARIABLE_BUFFER_SIZE3
    if (mOutSamples.GetSize() < numOutValues)
        numOutValues = mOutSamples.GetSize();
#endif
    
#if FIX_CRASH_FLSTUDIO_MAC_AU
    if (numOutValues < 0)
    {
        // Something went very wrong...
        numOutValues = 0;
    }
#endif
    
    outRes.Add(mOutSamples.Get(), numOutValues);
    BLUtils::ConsumeLeft(&mOutSamples, numOutValues);
    
    // result
    memcpy(result->Get(), outRes.Get(), numOutValues*sizeof(BL_FLOAT));
    
#if FIX_VARIABLE_BUFFER_SIZE3
    // Fill the rest with zeros if not enough out values are available
    // (can be the case if the blockSize suddently changed)
    if (numOutValues < result->GetSize())
    {
        int numZeros = result->GetSize() - numOutValues;
        memset(&result->Get()[numOutValues], 0, numZeros*sizeof(BL_FLOAT));
        
#if FIX_VARIABLE_BUFFER_SIZE3_FADE
        
#define NUM_SAMPLES_FADE 10
        BLUtils::FadeOut(result, numOutValues - NUM_SAMPLES_FADE, numOutValues);
#endif
        
    }
#endif
    
    //
    mSampleNum += samples.GetSize();
}

void
FastRTConvolver2::ComputeBufferSize(int blockSize)
{
    // Update buffer size
    int bufferSize = blockSize;
    bufferSize = BLUtils::NextPowerOfTwo(bufferSize);
    if (bufferSize > blockSize)
        bufferSize *= 0.5;
    
    if (bufferSize != mBufferSize)
    {
        mBufferSize = bufferSize;
        
        mConvMan->setBufferSize(mBufferSize);
    }
}
