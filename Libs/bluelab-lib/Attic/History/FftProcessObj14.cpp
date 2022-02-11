//
//  FftProcessObj14.cpp
//  Transient
//
//  Created by Apple m'a Tuer on 21/05/17.
//
//

#include <Window.h>
#include <BLUtils.h>

#include "FftProcessObj14.h"


#define INF 1e15
#define EPS 1e-15


#define NORMALIZE_FFT 1

// WARNING: this is buggy and false, need some fix and reconsidered if want to use it
// When using freq res > 1, add the tail of the fft to the future samples
// (it seems to add aliasing when activated)
#define ADD_TAIL 0

///

// TEST: quickly tested to remove the DC offset before fft
// and re-add it after ifft
// (changes a little the waveform)
//
// (not integrated because needs more tests)
#define DC_OFFSET_PROCESS 0


ProcessObj::ProcessObj(int bufferSize)
: mInput(true),
  mOutput(true)
{
    mBufferSize = bufferSize;
    
    mTrackInput = false;
    mTrackOutput = false;
}
    
ProcessObj::~ProcessObj() {}
    
void
ProcessObj::Reset(int oversampling, int freqRes, BL_FLOAT sampleRate)
{
    // TESTS (for Ghost)
    if (oversampling > 0)
        mOverlapping = oversampling;
    if (freqRes > 0)
        mFreqRes = freqRes;
    if (sampleRate > 0)
        mSampleRate = sampleRate;
    
    Reset();
}

void
ProcessObj::Reset()
{
    // Tracking
    mInput.Reset();
    mOutput.Reset();
}

void
ProcessObj::PreProcessSamplesBuffer(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                    const WDL_TypedBuf<BL_FLOAT> *scBuffer)
{
    mInput.AddValues(*ioBuffer);
}
    
void
ProcessObj::PreProcessSamplesBufferWin(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                       const WDL_TypedBuf<BL_FLOAT> *scBuffer) {}

void
ProcessObj::ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                             const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer) {}

void
ProcessObj::ProcessSamplesBuffer(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                 WDL_TypedBuf<BL_FLOAT> *scBuffer) {}

void
ProcessObj::ProcessSamplesBufferWin(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                    const WDL_TypedBuf<BL_FLOAT> *scBuffer) {}

void
ProcessObj::ProcessSamplesBufferEnergy(WDL_TypedBuf<BL_FLOAT> *ioBuffer,
                                       const WDL_TypedBuf<BL_FLOAT> *scBuffer)
{
    mOutput.AddValues(*ioBuffer);
}

void
ProcessObj::SetTrackIO(int maxNumPoints, BL_FLOAT decimFactor,
                       bool trackInput, bool trackOutput)
{
    mTrackInput = trackInput;
    mTrackOutput = trackOutput;
        
    if (mTrackInput)
        mInput.SetParams(maxNumPoints, decimFactor);
        
    if (mTrackOutput)
        mOutput.SetParams(maxNumPoints, decimFactor);
}
    
void
ProcessObj::GetCurrentInput(WDL_TypedBuf<BL_FLOAT> *outInput)
{
    mInput.GetValues(outInput);
}
    
void
ProcessObj::GetCurrentOutput(WDL_TypedBuf<BL_FLOAT> *outOutput)
{
    mOutput.GetValues(outOutput);
}

///

class ProcessObjChannel
{
public:
    ProcessObjChannel(ProcessObj *processObj, int bufferSize);
    
    virtual ~ProcessObjChannel();
    
    virtual void Reset(int overlapping, int freqRes, BL_FLOAT sampleRate);
    
    virtual void Reset();
    
    void SetSideChain(ProcessObjChannel *scChan);
    
    ProcessObj *GetProcessObject();
    void SetProcessObject(ProcessObj *obj);
    
    // Setters
    void SetAnalysisWindow(FftProcessObj14::WindowType type);
    
    void SetSynthesisWindow(FftProcessObj14::WindowType type);
    
    void SetKeepSynthesisEnergy(bool flag);
    
    void SetSkipFft(bool flag);
    
    
    // Processing mechanism
    void AddSamples(const WDL_TypedBuf<BL_FLOAT> &samples);
    
    bool HasSamplesToProcess();
    
    void BufferProcessed();
    
    // Steps
    void PrepareBufferStep();
    
    void MakeAnaWindowStep();
    
    void MakeFftStep();
    
    void ProcessFftStep();
    
    void MakeIFftStep();
    
    void MakeResultSamplesStep();
    
    void CommitResultStep();
    
    // Update the position
    bool Shift();
    
    bool GetResult(WDL_TypedBuf<BL_FLOAT> *output, int numRequested);
    
    // Accessors for processing horizontally
    WDL_TypedBuf<BL_FLOAT> *GetSamples();
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> *GetFft();
    
    WDL_TypedBuf<BL_FLOAT> *GetResultSamples();
    
protected:
    void ProcessOneBuffer(const WDL_TypedBuf<BL_FLOAT> &inBuffer,
                          const WDL_TypedBuf<BL_FLOAT> &inScBuffer,
                          WDL_TypedBuf<BL_FLOAT> *outBuffer);
    
    
    // Shift to next buffer.
    void NextOutBuffer();
    
    // Shift to next buffer.
    void NextSamplesBuffer(WDL_TypedBuf<BL_FLOAT> *samples);
    
    // Get a buffer and consume it from the result samples
    void GetResultOutBuffer(WDL_TypedBuf<BL_FLOAT> *output, int numRequested);
    
    
    void ApplyAnalysisWindow(WDL_TypedBuf<BL_FLOAT> *samples);
    
    void ApplySynthesisWindow(WDL_TypedBuf<BL_FLOAT> *samples);
    
    //
    ProcessObj *mProcessObj;
    
    int mBufferSize;
    int mOverlapping;
    BL_FLOAT mSampleRate;
    
    // NOTE: tested hanning root: it is not COLA !
    
    // If mFreqRes == 2, and we are doing convolution (which is not the case...)
    // We take care to avoid cyclic processing, by growing the
    // buffers by 2 and padding with zeros
    // And we sum also the second part of the buffers, which contains
    // "future" sample contributions
    // See: See: http://eeweb.poly.edu/iselesni/EL713/zoom/overlap.pdf
    
    // With mFreqRes > 1, we padd the input signal with zeros increasing
    // the resolution of the fft, so we decreasing the aliasing.
    int mFreqRes;
    
    // Windows
    enum FftProcessObj14::WindowType mAnalysisMethod;
    enum FftProcessObj14::WindowType mSynthesisMethod;
    
    WDL_TypedBuf<BL_FLOAT> mAnalysisWindow;
    WDL_TypedBuf<BL_FLOAT> mSynthesisWindow;
    
    
    bool mKeepSynthesisEnergy;
    bool mSkipFFT;
    
    int mShift;
    int mBufOffset;
    
    // Input samples (bufferized)
    WDL_TypedBuf<BL_FLOAT> mSamplesIn;
    
    // Current result
    WDL_TypedBuf<BL_FLOAT> mResult;
    
    // Result with overlap add
    WDL_TypedBuf<BL_FLOAT> mResultSum;
    
    // REsult output that is totally ready
    WDL_TypedBuf<BL_FLOAT> mResultOut;
    
    
    WDL_TypedBuf<BL_FLOAT> mPreparedBuffer;
    WDL_TypedBuf<WDL_FFT_COMPLEX> mFftBuf;
    BL_FLOAT mEnergy0;
    
    // Used to pad the samples with zeros on the left,
    // for correct overlapping of the beginning
    bool mMustPadSamples;
    bool mMustUnpadResult;
    
    // Lazy evaluation
    bool mMustMakeWindows;
    
#if DC_OFFSET_PROCESS
    BL_FLOAT mDCOffset;
#endif

private:
    ProcessObjChannel *mScChan;
};

ProcessObjChannel::ProcessObjChannel(ProcessObj *processObj, int bufferSize)
{
    mProcessObj = processObj;
    
    mBufferSize = bufferSize;
    
    mOverlapping = 1;
    mFreqRes = 1;
    mSampleRate = 44100.0;
    
    mShift = mBufferSize/mOverlapping;
    
    mAnalysisMethod = FftProcessObj14::WindowRectangular;
    mSynthesisMethod = FftProcessObj14::WindowRectangular;
    
    mKeepSynthesisEnergy = false;
    
    mSkipFFT = false;
    
    mMustPadSamples = true;
    mMustUnpadResult = false;
    
    mScChan = NULL;
    
    mMustMakeWindows = true;
    
#if DC_OFFSET_PROCESS
    mDCOffset = 0.0;
#endif
}

ProcessObjChannel::~ProcessObjChannel() {}

void
ProcessObjChannel::Reset(int overlapping, int freqRes, BL_FLOAT sampleRate)
{
    mOverlapping = overlapping;
    mFreqRes = freqRes;
    mSampleRate = sampleRate;
    
    Reset();
}

void
ProcessObjChannel::Reset()
{
    mShift = mBufferSize/mOverlapping;
    
    FftProcessObj14::MakeWindows(mBufferSize, mOverlapping,
                                 mAnalysisMethod, mSynthesisMethod,
                                 &mAnalysisWindow, &mSynthesisWindow);
    // Set the buffers
    mSamplesIn.Resize(0);
    
    BLUtils::ResizeFillZeros(&mResult, mBufferSize);
    
    int resultSize = 2*mBufferSize*mFreqRes;
    BLUtils::ResizeFillZeros(&mResultSum, resultSize);
    
    mResultOut.Resize(0);
    
    int bufSize = mBufferSize*mFreqRes;
    BLUtils::ResizeFillZeros(&mFftBuf, bufSize);
    
    mMustPadSamples = true;
    mMustUnpadResult = false;
    
    mBufOffset = 0;
    
    if (mProcessObj != NULL)
        mProcessObj->Reset(mOverlapping, mFreqRes, mSampleRate);
}

void
ProcessObjChannel::SetSideChain(ProcessObjChannel *scChan)
{
    mScChan = scChan;
}

ProcessObj *
ProcessObjChannel::GetProcessObject()
{
    return mProcessObj;
}

void
ProcessObjChannel::SetProcessObject(ProcessObj *obj)
{
    mProcessObj = obj;
}


void
ProcessObjChannel::SetAnalysisWindow(FftProcessObj14::WindowType type)
{
    mAnalysisMethod = type;
    
    mMustMakeWindows = true;
}

void
ProcessObjChannel::SetSynthesisWindow(FftProcessObj14::WindowType type)
{
    mSynthesisMethod = type;
    
    mMustMakeWindows = true;
}

void
ProcessObjChannel::SetKeepSynthesisEnergy(bool flag)
{
    mKeepSynthesisEnergy = flag;
}

void
ProcessObjChannel::SetSkipFft(bool flag)
{
    mSkipFFT = flag;
}

bool
ProcessObjChannel::HasSamplesToProcess()
{
    bool result = (mSamplesIn.GetSize() >= 2*mBufferSize - mShift);
    
    return result;
}

void
ProcessObjChannel::BufferProcessed()
{
    NextSamplesBuffer(&mSamplesIn);
    NextOutBuffer();
}

void
ProcessObjChannel::AddSamples(const WDL_TypedBuf<BL_FLOAT> &samples)
{
    int samplesSize = samples.GetSize();
    mSamplesIn.Add(samples.Get(), samplesSize);
    
    if (mMustPadSamples)
    {
        BLUtils::PadZerosLeft(&mSamplesIn, mAnalysisWindow.GetSize());
        
        mMustPadSamples = false;
        mMustUnpadResult = true;
    }
}

bool
ProcessObjChannel::GetResult(WDL_TypedBuf<BL_FLOAT> *output, int numRequested)
{
    // With that (x1.5), this was impossible to setup the latency correctly
    //if (mResultOut.GetSize() >= mAnalysisWindow.GetSize()*1.5)
    
    // This one (x2) is correct for setting the latency correctly
    if (mResultOut.GetSize() >= mAnalysisWindow.GetSize()*2)
    {
        if (mMustUnpadResult)
        {
            BLUtils::ConsumeLeft(&mResultOut, mAnalysisWindow.GetSize());
            
            mMustUnpadResult = false;
        }
    }
    
    // Get the computed result
    int numOutSamples = mResultOut.GetSize();
    if ((numRequested <= numOutSamples) && !mMustUnpadResult)
    {
        //if ((output != NULL) && (output->GetSize() >= numRequested))
        if (output != NULL)
        {
            GetResultOutBuffer(output, numRequested);
        }
        
        return true;
    }
    else
        // Fill with zeros
        // (as required when there is latency and data is not yet available)
    {
        //if ((output != NULL) && (output->GetSize() >= numRequested))
        if (output != NULL)
        {
            BLUtils::ResizeFillZeros(output, numRequested);
        }
        
        return false;
    }
}

// Accessors for processing horizontally
WDL_TypedBuf<BL_FLOAT> *
ProcessObjChannel::GetSamples()
{
    return &mSamplesIn;
}

WDL_TypedBuf<WDL_FFT_COMPLEX> *
ProcessObjChannel::GetFft()
{
    return &mFftBuf;
}

WDL_TypedBuf<BL_FLOAT> *
ProcessObjChannel::GetResultSamples()
{
    return &mResult;
}

void
ProcessObjChannel::PrepareBufferStep()
{
    // This can be an empty channel !
    // i.e without samples added
    // (e.g in the case we create 2 channels, and we are in mono)
    if (mSamplesIn.GetSize() < mBufferSize + mBufOffset)
        return;
    
    mPreparedBuffer.Resize(mBufferSize);
    for (int i = 0; i < mBufferSize; i++)
        mPreparedBuffer.Get()[i] = mSamplesIn.Get()[i + mBufOffset];
    
    if (mProcessObj != NULL)
    {
        WDL_TypedBuf<BL_FLOAT> *scBuf = NULL;
        if (mScChan != NULL)
            scBuf = &mScChan->mPreparedBuffer;
        
        mProcessObj->PreProcessSamplesBuffer(&mPreparedBuffer, scBuf);
    }
}

void
ProcessObjChannel::MakeAnaWindowStep()
{
    if (mMustMakeWindows)
    {
        FftProcessObj14::MakeWindows(mBufferSize, mOverlapping,
                                     mAnalysisMethod, mSynthesisMethod,
                                     &mAnalysisWindow, &mSynthesisWindow);
    }
    mMustMakeWindows = false;
    
    mResult.Resize(mPreparedBuffer.GetSize());
    
    // Apply analysis windows before resizing !
    ApplyAnalysisWindow(&mPreparedBuffer);
    
    if (mProcessObj != NULL)
    {
        WDL_TypedBuf<BL_FLOAT> *scBuf = NULL;
        if (mScChan != NULL)
            scBuf = &mScChan->mPreparedBuffer;
        
        mProcessObj->PreProcessSamplesBufferWin(&mPreparedBuffer, scBuf);
    }
}

void
ProcessObjChannel::MakeFftStep()
{
    // Compute "energy" just after analysis windowing
    // then just after synthesis windowing
    // Compute and apply a coefficient to avoid loosing energy
    mEnergy0 = 0.0;
    if (mKeepSynthesisEnergy)
        mEnergy0 = BLUtils::ComputeAbsAvg(mPreparedBuffer);
    
    // windowing before zero padding
    // See: http://www.bitweenie.com/listings/fft-zero-padding/
    // (last links of the page)
    if (mFreqRes > 1)
    {
        // Non-cyclic technique, to avoid aliasing
        BLUtils::ResizeFillZeros(&mPreparedBuffer, mPreparedBuffer.GetSize()*mFreqRes);
    }
    
    if (mSkipFFT)
    {
        // Be careful to not add the beginning of the buffer
        // (otherwise the windows will be applied twice sometimes).
        mResult.Add(mPreparedBuffer.Get(), mBufferSize);
    }
    else
    {
#if DC_OFFSET_PROCESS
        mDCOffset = BLUtils::ComputeAvg(mPreparedBuffer);
        BLUtils::AddValues(&mPreparedBuffer, -mDCOffset);
#endif
        
        // NOTE: Here, we have checked that mFreqRes didn't modify the amplitude
        // of the out samples, neither the intermediate magnitude !
        FftProcessObj14::ComputeFft(mPreparedBuffer, &mFftBuf, mFreqRes);
    }
}

void
ProcessObjChannel::ProcessFftStep()
{
    if (!mSkipFFT)
    {
        if (mProcessObj != NULL)
        {
            WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuf = NULL;
            if (mScChan != NULL)
                scBuf = &mScChan->mFftBuf;
            
            mProcessObj->ProcessFftBuffer(&mFftBuf, scBuf);
        }
    }
}

void
ProcessObjChannel::MakeIFftStep()
{
    FftProcessObj14::ComputeInverseFft(mFftBuf, &mResult, mFreqRes);
    
#if DC_OFFSET_PROCESS
    BLUtils::AddValues(&mResult, mDCOffset);
#endif

    if (mProcessObj != NULL)
    {
        WDL_TypedBuf<BL_FLOAT> *scBuf = NULL;
        if (mScChan != NULL)
            scBuf = &mScChan->mResult;
        
        mProcessObj->ProcessSamplesBuffer(&mResult, scBuf);
    }
}

void
ProcessObjChannel::MakeResultSamplesStep()
{
#if 0
    if (mProcessObj != NULL)
    {
        WDL_TypedBuf<BL_FLOAT> *scBuf = NULL;
        if (mScChan != NULL)
            scBuf = &mScChan->mResult;
        
        //mProcessObj->ProcessSamplesBuffer(&mResult, scBuf);
        mProcessObj->ProcessSamplesBufferWin(&mResult, scBuf);
    }
#endif
    
    if (mKeepSynthesisEnergy)
    {
        BL_FLOAT energy1 = BLUtils::ComputeAbsAvg(mResult);
        
        BL_FLOAT energyCoeff = 1.0;
        if (energy1 > 0.0)
            energyCoeff = mEnergy0/energy1;
        
        // If mOverlapping is 1, then div is 0
        if (mOverlapping > 1)
        {
            // ORIGIN: not commented
            // NOTE: strange coeff...
            //BL_FLOAT div = std::log(mOverlapping)/std::log(2.0);
            //energyCoeff /= div;
            
            BLUtils::MultValues(&mResult, energyCoeff);
        }
    }
    
    if (mProcessObj != NULL)
    {
        WDL_TypedBuf<BL_FLOAT> *scBuf = NULL;
        if (mScChan != NULL)
            scBuf = &mScChan->mResult;
        
        mProcessObj->ProcessSamplesBufferEnergy(&mResult, scBuf);
    }
    
    if (mMustMakeWindows)
    {
        FftProcessObj14::MakeWindows(mBufferSize, mOverlapping,
                                     mAnalysisMethod, mSynthesisMethod,
                                     &mAnalysisWindow, &mSynthesisWindow);
    }
    mMustMakeWindows = false;
    
    // Apply synthesis window
    ApplySynthesisWindow(&mResult);
}

void
ProcessObjChannel::CommitResultStep()
{
    if (mProcessObj != NULL)
    {
        WDL_TypedBuf<BL_FLOAT> *scBuf = NULL;
        if (mScChan != NULL)
            scBuf = &mScChan->mResult;
        
        mProcessObj->ProcessSamplesBufferWin(&mResult, scBuf);
    }
    
#if !ADD_TAIL
    // Standard size
    int loopCount = mBufferSize;
#else
    // Size to add tail
    int loopCount = copySamplesBuf.GetSize();
#endif
    
    // Channel can be empty
    if (mResult.GetSize() < loopCount)
        return;
    
    if (mResultSum.GetSize() < loopCount + mBufOffset)
        return;
    
    // Make the sum (overlap)
    for (int i = 0; i < loopCount; i++)
    {
        mResultSum.Get()[i + mBufOffset] += mResult.Get()[i];
    }
}

void
ProcessObjChannel::NextSamplesBuffer(WDL_TypedBuf<BL_FLOAT> *samples)
{
    // Reduce the size by mOverlap
    int size = samples->GetSize();
    if (size == mBufferSize)
    {
        samples->Resize(0);
    }
    else if (size > mBufferSize)
    {
        // Resize down, skipping left
        BLUtils::ConsumeLeft(samples, mBufferSize);
    }
}

void
ProcessObjChannel::NextOutBuffer()
{
    // NOTE: commented for new version, and Reaper
    // otherwise we never start to get the result
    //
    // Reduce the size by mOverlap
    int minSize = mBufferSize*2*mFreqRes;
    if (mResultSum.GetSize() < minSize)
        return;
    
    WDL_TypedBuf<BL_FLOAT> newBuffer;
    mResultOut.Add(mResultSum.Get(), mBufferSize);
    
    if (mResultSum.GetSize() == mBufferSize)
    {
        mResultSum.Resize(0);
    }
    else if (mResultSum.GetSize() > mBufferSize)
    {
        // Resize down, skipping left
        BLUtils::ConsumeLeft(&mResultSum, mBufferSize);
    }
    
    // Grow the output with zeros
    BLUtils::ResizeFillZeros(&mResultSum,
                           mResultSum.GetSize() + mBufferSize);
}

void
ProcessObjChannel::GetResultOutBuffer(WDL_TypedBuf<BL_FLOAT> *output,
                                      int numRequested)
{
    int size = mResultOut.GetSize();
    
    // Should not happen
    if (size < numRequested)
        return;
    
    //
    output->Resize(numRequested);
    
    // Copy the result
    memcpy(output->Get(), mResultOut.Get(), numRequested*sizeof(BL_FLOAT));
    
    // Resize down, skipping left
    BLUtils::ConsumeLeft(&mResultOut, numRequested);
}

bool
ProcessObjChannel::Shift()
{
    mBufOffset += mShift;
    
    if (mBufOffset >= mBufferSize)
    {
        mBufOffset = 0;
        
        return true;
    }
    
    return false;
}

void
ProcessObjChannel::ApplyAnalysisWindow(WDL_TypedBuf<BL_FLOAT> *samples)
{
    // Multiply the samples by analysis window
    // The window can be identity if necessary (no analysis window, or no overlap)
    for (int i = 0; i < samples->GetSize(); i++)
    {
        BL_FLOAT winCoeff = mAnalysisWindow.Get()[i];
        samples->Get()[i] *= winCoeff;
    }
}

void
ProcessObjChannel::ApplySynthesisWindow(WDL_TypedBuf<BL_FLOAT> *samples)
{
    // Multiply the samples by synthesis window
    // The window can be identity if necessary
    // (no synthesis window, or no overlap)
    for (int i = 0; i < samples->GetSize(); i++)
    {
        BL_FLOAT winCoeff = mSynthesisWindow.Get()[i];
        samples->Get()[i] *= winCoeff;
    }
}

void
FftProcessObj14::Init()
{
    // Init WDL FFT
    WDL_fft_init();
}

// NOTE: With freqRes == 2, and overlapping = 1 => better
// the transient signal is decreased / the other signal stays constant
// => that's what we want !
// With previous version, the other signal increased
FftProcessObj14::FftProcessObj14(const vector<ProcessObj *> &processObjs,
                                 int numChannels, int numScInputs,
                                 int bufferSize, int overlapping, int freqRes,
                                 BL_FLOAT sampleRate)
{
    mBufferSize = bufferSize;
    
    Reset(overlapping, freqRes, sampleRate);
    
    mNumScInputs = numScInputs;
    
    mChannels.resize(numChannels + numScInputs);
    for (int i = 0; i < mChannels.size(); i++)
    {
        ProcessObj *obj = (i < processObjs.size()) ? processObjs[i] : NULL;
        ProcessObjChannel *chan = new ProcessObjChannel(obj, bufferSize);
        
        chan->Reset(mOverlapping, mFreqRes, mSampleRate);
        
        mChannels[i] = chan;
    }
    
    SetupSideChain();
}

FftProcessObj14::~FftProcessObj14()
{
    for (int i = 0; i < mChannels.size(); i++)
    {
        ProcessObjChannel *chan = mChannels[i];
     
        delete chan;
    }
}

void
FftProcessObj14::Reset(int overlapping, int freqRes, BL_FLOAT sampleRate)
{
    mOverlapping = overlapping;
    mFreqRes = freqRes;
    mSampleRate = sampleRate;
    
    Reset();
}

void
FftProcessObj14::Reset()
{
    for (int i = 0; i < mChannels.size(); i++)
    {
        ProcessObjChannel *chan = mChannels[i];
        chan->Reset(mOverlapping, mFreqRes, mSampleRate);
    }
    
    for (int i = 0; i < mMcProcesses.size(); i++)
    {
        MultichannelProcess *mcProcess = mMcProcesses[i];
        mcProcess->Reset(mOverlapping, mFreqRes, mSampleRate);
    }
}

int
FftProcessObj14::GetBufferSize()
{
    return mBufferSize;
}

int
FftProcessObj14::GetOverlapping()
{
    return mOverlapping;
}

int
FftProcessObj14::GetFreqRes()
{
    return mFreqRes;
}

BL_FLOAT
FftProcessObj14::GetSampleRate()
{
    return mSampleRate;
}

void
FftProcessObj14::SetAnalysisWindow(int channelNum, WindowType type)
{
    if ((channelNum != ALL_CHANNELS) &&
        (channelNum < mChannels.size()))
        mChannels[channelNum]->SetAnalysisWindow(type);
    else
    {
        for (int i = 0; i < mChannels.size(); i++)
            mChannels[i]->SetAnalysisWindow(type);
    }
}

void
FftProcessObj14::SetSynthesisWindow(int channelNum, WindowType type)
{
    if ((channelNum != ALL_CHANNELS) &&
        (channelNum < mChannels.size()))
        mChannels[channelNum]->SetSynthesisWindow(type);
    else
    {
        for (int i = 0; i < mChannels.size(); i++)
            mChannels[i]->SetSynthesisWindow(type);
    }
}

void
FftProcessObj14::SetKeepSynthesisEnergy(int channelNum, bool flag)
{
    if ((channelNum != ALL_CHANNELS) &&
        (channelNum < mChannels.size()))
        mChannels[channelNum]->SetKeepSynthesisEnergy(flag);
    else
    {
    for (int i = 0; i < mChannels.size(); i++)
        mChannels[i]->SetKeepSynthesisEnergy(flag);
    }
}

void
FftProcessObj14::SetSkipFft(int channelNum, bool flag)
{
    if ((channelNum != ALL_CHANNELS) &&
        (channelNum < mChannels.size()))
        mChannels[channelNum]->SetSkipFft(flag);
    else
    {
        for (int i = 0; i < mChannels.size(); i++)
            mChannels[i]->SetSkipFft(flag);
    }
}

void
FftProcessObj14::AddMultichannelProcess(MultichannelProcess *mcProcess)
{
    mMcProcesses.push_back(mcProcess);
}

void
FftProcessObj14::InputSamplesReady()
{
    if (mMcProcesses.empty())
        return;
    
    vector<WDL_TypedBuf<BL_FLOAT> * > samples;
    GetAllSamples(&samples);
    
    for (int i = 0; i < mMcProcesses.size(); i++)
    {
        MultichannelProcess *mcProcess = mMcProcesses[i];
        
        mcProcess->ProcessInputSamples(&samples);
    }
}

void
FftProcessObj14::InputSamplesWinReady()
{
    if (mMcProcesses.empty())
        return;
    
    vector<WDL_TypedBuf<BL_FLOAT> * > samples;
    GetAllSamples(&samples);
    
    for (int i = 0; i < mMcProcesses.size(); i++)
    {
        MultichannelProcess *mcProcess = mMcProcesses[i];
        
        mcProcess->ProcessInputSamplesWin(&samples);
    }
}

void
FftProcessObj14::InputFftReady()
{
    if (mMcProcesses.empty())
        return;
    
    vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > samples;
    GetAllFftSamples(&samples);
    
    for (int i = 0; i < mMcProcesses.size(); i++)
    {
        MultichannelProcess *mcProcess = mMcProcesses[i];
        
        mcProcess->ProcessInputFft(&samples);
    }
}

void
FftProcessObj14::ResultFftReady()
{
    if (mMcProcesses.empty())
        return;
    
    vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > samples;
    GetAllFftSamples(&samples);
    
    for (int i = 0; i < mMcProcesses.size(); i++)
    {
        MultichannelProcess *mcProcess = mMcProcesses[i];
        
        mcProcess->ProcessResultFft(&samples);
    }
}

void
FftProcessObj14::ResultSamplesWinReady()
{
    if (mMcProcesses.empty())
        return;
    
    vector<WDL_TypedBuf<BL_FLOAT> * > samples;
    GetAllResultSamples(&samples);
    
    for (int i = 0; i < mMcProcesses.size(); i++)
    {
        MultichannelProcess *mcProcess = mMcProcesses[i];
        
        mcProcess->ProcessResultSamplesWin(&samples);
    }
}

void
FftProcessObj14::ResultSamplesReady()
{
    if (mMcProcesses.empty())
        return;
    
    vector<WDL_TypedBuf<BL_FLOAT> * > samples;
    GetAllResultSamples(&samples);
    
    for (int i = 0; i < mMcProcesses.size(); i++)
    {
        MultichannelProcess *mcProcess = mMcProcesses[i];
        
        mcProcess->ProcessResultSamples(&samples);
    }
}

void
FftProcessObj14::SamplesToMagnPhases(const WDL_TypedBuf<BL_FLOAT> &inSamples,
                                     WDL_TypedBuf<BL_FLOAT> *outFftMagns,
                                     WDL_TypedBuf<BL_FLOAT> *outFftPhases)
{
    int numSamples = inSamples.GetSize();
    int numPaddedSamples = BLUtils::NextPowerOfTwo(numSamples);
    
    WDL_TypedBuf<BL_FLOAT> paddedSamples = inSamples;
    BLUtils::ResizeFillZeros(&paddedSamples, numPaddedSamples);
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    int freqRes = 1;
    ComputeFft(paddedSamples, &fftSamples, freqRes);
    
    WDL_TypedBuf<BL_FLOAT> magns;
    WDL_TypedBuf<BL_FLOAT> phases;
    BLUtils::ComplexToMagnPhase(&magns, &phases, fftSamples);
    
    if (outFftMagns != NULL)
        *outFftMagns = magns;
    
    if (outFftPhases != NULL)
        *outFftPhases = phases;
}

void
FftProcessObj14::SamplesToHalfMagnPhases(const WDL_TypedBuf<BL_FLOAT> &inSamples,
                                         WDL_TypedBuf<BL_FLOAT> *outFftMagns,
                                         WDL_TypedBuf<BL_FLOAT> *outFftPhases)
{
    int numSamples = inSamples.GetSize();
    int numPaddedSamples = BLUtils::NextPowerOfTwo(numSamples);
    
    WDL_TypedBuf<BL_FLOAT> paddedSamples = inSamples;
    BLUtils::ResizeFillZeros(&paddedSamples, numPaddedSamples);
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    int freqRes = 1;
    ComputeFft(paddedSamples, &fftSamples, freqRes);
    
    BLUtils::TakeHalf(&fftSamples);
    
    WDL_TypedBuf<BL_FLOAT> magns;
    WDL_TypedBuf<BL_FLOAT> phases;
    BLUtils::ComplexToMagnPhase(&magns, &phases, fftSamples);
    
    if (outFftMagns != NULL)
        *outFftMagns = magns;
    
    if (outFftPhases != NULL)
        *outFftPhases = phases;
}

void
FftProcessObj14::FftToSamples(const WDL_TypedBuf<WDL_FFT_COMPLEX> &fftBuffer,
                              WDL_TypedBuf<BL_FLOAT> *outSamples)
{
    outSamples->Resize(fftBuffer.GetSize());
    
    int freqRes = 1;
    ComputeInverseFft(fftBuffer, outSamples, freqRes);
}

void
FftProcessObj14::SamplesToFft(const WDL_TypedBuf<BL_FLOAT> &inSamples,
                              WDL_TypedBuf<WDL_FFT_COMPLEX> *outFftBuffer)
{
    outFftBuffer->Resize(inSamples.GetSize());
    
    int freqRes = 1;
    ComputeFft(inSamples, outFftBuffer, freqRes);
}

// See: https://en.wikipedia.org/wiki/Cepstrum
void
FftProcessObj14::MagnsToCepstrum(const WDL_TypedBuf<BL_FLOAT> &halfMagns,
                                 WDL_TypedBuf<BL_FLOAT> *outCepstrum)
{
    // Fill second half
    WDL_TypedBuf<BL_FLOAT> magns = halfMagns;
    magns.Resize(magns.GetSize()*2);
                 
    BLUtils::FillSecondFftHalf(&magns);

#if 1
    // Compute the square
    for (int i = 0; i < magns.GetSize(); i++)
    {
        BL_FLOAT val = magns.Get()[i];
        BL_FLOAT val2 = val*val;
        magns.Get()[i] = val2;
    }
#endif
    
    // Compute the (natural) log
    for (int i = 0; i < magns.GetSize(); i++)
    {
        BL_FLOAT val = magns.Get()[i];
        
        BL_FLOAT logVal = 0.0;
        if (val > EPS)
	  logVal = std::log(val);
        magns.Get()[i] = logVal;
    }
    
    // Compute the inverse fft
    //
    
    // Prepare
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftSamples;
    fftSamples.Resize(magns.GetSize());
    for (int i = 0; i < magns.GetSize(); i++)
    {
        BL_FLOAT val = magns.Get()[i];
        
        fftSamples.Get()[i].re = val;
        fftSamples.Get()[i].im = 0.0;
    }
    
    // Compute
    WDL_TypedBuf<WDL_FFT_COMPLEX> iFftSamples;
    ComputeInverseFft(fftSamples, &iFftSamples);
    
    // Compute the result
    WDL_TypedBuf<BL_FLOAT> result;
    result.Resize(iFftSamples.GetSize());
                  
    BLUtils::ComplexToMagn(&result, iFftSamples);
    
#if 0 // TEST
    for (int i = 0; i < iFftSamples.GetSize(); i++)
    {
        BL_FLOAT val = iFftSamples.Get()[i].re;
    
        result.Get()[i] = val;
    }
#endif
    
    // Power Cepstrum => compute the square
    for (int i = 0; i < result.GetSize(); i++)
    {
        BL_FLOAT val = result.Get()[i];
        BL_FLOAT val2 = val*val;
        result.Get()[i] = val2;
    }
    
    BLUtils::TakeHalf(&result);
    
    *outCepstrum = result;
}

ProcessObj *
FftProcessObj14::GetChannelProcessObject(int channelNum)
{
    if ((channelNum < 0) || (channelNum >= mChannels.size()))
        return NULL;
    
    ProcessObj *result = mChannels[channelNum]->GetProcessObject();
    
    return result;
}

void
FftProcessObj14::SetChannelProcessObject(int channelNum, ProcessObj *obj)
{
    if ((channelNum < 0) || (channelNum >= mChannels.size()))
        return;
    
    mChannels[channelNum]->SetProcessObject(obj);
}


WDL_TypedBuf<BL_FLOAT> *
FftProcessObj14::GetChannelSamples(int channelNum)
{
    if ((channelNum < 0) || (channelNum >= mChannels.size()))
        return NULL;
    
    WDL_TypedBuf<BL_FLOAT> *result = mChannels[channelNum]->GetSamples();
    
    return result;
}

WDL_TypedBuf<WDL_FFT_COMPLEX> *
FftProcessObj14::GetChannelFft(int channelNum)
{
    if ((channelNum < 0) || (channelNum >= mChannels.size()))
        return NULL;
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> *result = mChannels[channelNum]->GetFft();
    
    return result;
}

WDL_TypedBuf<BL_FLOAT> *
FftProcessObj14::GetChannelResultSamples(int channelNum)
{
    if ((channelNum < 0) || (channelNum >= mChannels.size()))
        return NULL;
    
    WDL_TypedBuf<BL_FLOAT> *result = mChannels[channelNum]->GetResultSamples();
    
    return result;
}

void
FftProcessObj14::MagnPhasesToSamples(const WDL_TypedBuf<BL_FLOAT> &fftMagns,
                                     const  WDL_TypedBuf<BL_FLOAT> &fftPhases,
                                     WDL_TypedBuf<BL_FLOAT> *outSamples)
{
    outSamples->Resize(fftMagns.GetSize());
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftBuffer;
    BLUtils::MagnPhaseToComplex(&fftBuffer, fftMagns, fftPhases);
    
    int freqRes = 1;
    ComputeInverseFft(fftBuffer, outSamples, freqRes);
}

void
FftProcessObj14::HalfMagnPhasesToSamples(const WDL_TypedBuf<BL_FLOAT> &fftMagns,
                                         const  WDL_TypedBuf<BL_FLOAT> &fftPhases,
                                         WDL_TypedBuf<BL_FLOAT> *outSamples)
{
    outSamples->Resize(fftMagns.GetSize()*2);
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> fftBuffer;
    BLUtils::MagnPhaseToComplex(&fftBuffer, fftMagns, fftPhases);
    
    fftBuffer.Resize(fftBuffer.GetSize()*2);
    BLUtils::FillSecondFftHalf(&fftBuffer);
    
    int freqRes = 1;
    ComputeInverseFft(fftBuffer, outSamples, freqRes);
}


void
FftProcessObj14::AddSamples(const vector<WDL_TypedBuf<BL_FLOAT> > &inputs,
                            const vector<WDL_TypedBuf<BL_FLOAT> > &scInputs)
{
    // Pre-process samples
    
    // Make a copy of the input
    // because it may be modified before adding it
    vector<WDL_TypedBuf<BL_FLOAT> > samplesIn = inputs;
    
    // Convert to the right format for Mc
    vector<WDL_TypedBuf<BL_FLOAT> *> samplesInMc;
    for (int i = 0; i < samplesIn.size(); i++)
    {
        samplesInMc.push_back(&samplesIn[i]);
    }
    
    // Potentially modify the input before adding it
    for (int i = 0; i < mMcProcesses.size(); i++)
    {
        MultichannelProcess *mcProcess = mMcProcesses[i];
        
        mcProcess->ProcessInputSamplesPre(&samplesInMc);
    }
    
    // Add the new input data
    for (int i = 0; i < inputs.size(); i++)
    {
        mChannels[i]->AddSamples(samplesIn[i]);
    }
    
    for (int i = 0; i < scInputs.size()/*mNumScInputs*/; i++)
    {
        int channelNum = mChannels.size() - mNumScInputs + i;
        mChannels[channelNum]->AddSamples(scInputs[i]);
    }
}

void
FftProcessObj14::AddSamples(int startChan,
                            const vector<WDL_TypedBuf<BL_FLOAT> > &inputs)
{
    int numInputs = inputs.size();
    
    // Add the new input data
    for (int i = 0; i < numInputs; i++)
    {
        mChannels[i + startChan]->AddSamples(inputs[i]);
    }
}

void
FftProcessObj14::ProcessSamples()
{
    if (mChannels.size() == 0)
        return;
    
    // All channels should have the same number of samples
    // and the same state
    
    ProcessObjChannel *chan0 = mChannels[0];
    while(chan0->HasSamplesToProcess())
    {
        bool stopFlag = false;
        
        // Compute from the available input data
        while(!stopFlag)
        {
            // Count in the reverse order, to be sure to process the side chains first
            // So when we will process the ordinary oblect, they will have their side
            // chain data ready
            
            // Process step by step for all channels
            // So it will be possible to make operations between channels
            for (int i = mChannels.size() - 1; i >= 0 ; i--)
            {
                ProcessObjChannel *chan = mChannels[i];
                chan->PrepareBufferStep();
            }
            
            InputSamplesReady();
            
            for (int i = mChannels.size() - 1; i >= 0 ; i--)
            {
                ProcessObjChannel *chan = mChannels[i];
                chan->MakeAnaWindowStep();
            }
            
            InputSamplesWinReady();
            
            for (int i = mChannels.size() - 1; i >= 0 ; i--)
            {
                ProcessObjChannel *chan = mChannels[i];
                chan->MakeFftStep();
            }
            
            InputFftReady();
            
            for (int i = mChannels.size() - 1; i >= 0 ; i--)
            {
                ProcessObjChannel *chan = mChannels[i];
                chan->ProcessFftStep();
            }
            
            ResultFftReady();
            
            for (int i = mChannels.size() - 1; i >= 0 ; i--)
            {
                ProcessObjChannel *chan = mChannels[i];
                chan->MakeIFftStep();
            }
            
            //ResultSamplesWinReady();
            ResultSamplesReady();
            
            for (int i = mChannels.size() - 1; i >= 0 ; i--)
            {
                ProcessObjChannel *chan = mChannels[i];
                chan->MakeResultSamplesStep();
            }
            
            //ResultSamplesReady();
            ResultSamplesWinReady();
            
            for (int i = mChannels.size() - 1; i >= 0 ; i--)
            {
                ProcessObjChannel *chan = mChannels[i];
                chan->CommitResultStep();
            }
                
            for (int i = mChannels.size() - 1; i >= 0 ; i--)
            {
                ProcessObjChannel *chan = mChannels[i];
                stopFlag = chan->Shift();
            }
        }
        
        for (int i = mChannels.size() - 1; i >= 0 ; i--)
        {
            ProcessObjChannel *chan = mChannels[i];
            chan->BufferProcessed();
        }
    }
}

bool
FftProcessObj14::GetResults(vector<WDL_TypedBuf<BL_FLOAT> > *outputs,
                           int numRequested)
{
    int numInputChannels = mChannels.size() - mNumScInputs;
    
    bool resultReady = true;
    for (int i = 0; i < numInputChannels; i++)
    {
        if (i < outputs->size())
        {
            WDL_TypedBuf<BL_FLOAT> &out = (*outputs)[i];
            bool res = mChannels[i]->GetResult(&out, numRequested);
        
            resultReady = resultReady && res;
        }
    }
    
    return resultReady;
}

void
FftProcessObj14::SetupSideChain()
{
    if (mNumScInputs == 0)
        return;
    
    int numInputs = mChannels.size() - mNumScInputs;
    for (int i = 0; i < numInputs; i++)
    {
        ProcessObjChannel *chan = mChannels[i];
        
        // Setup side chain input
        // If we have stereo side chain, try to assign each side chain channel
        // to each input channel
        if (i /*==*/ < mNumScInputs)
        {
            ProcessObjChannel *sc = mChannels[i + numInputs];
            chan->SetSideChain(sc);
            
            continue;
        }
        
        // Otherwise, take the first side chain input for all
        // the input channels
        
        // Set side chain 0
        ProcessObjChannel *sc = mChannels[numInputs];
        chan->SetSideChain(sc);
    }
}

bool
FftProcessObj14::Process(const vector<WDL_TypedBuf<BL_FLOAT> > &inputs,
                         const vector<WDL_TypedBuf<BL_FLOAT> > &scInputs,
                         vector<WDL_TypedBuf<BL_FLOAT> > *outputs)
{
#if 0
    if (inputs.size() == 0)
        return false;
#endif
    
    vector<WDL_TypedBuf<BL_FLOAT> > inputs0 = inputs;
    
    if (inputs0.size() == 0)
    {
        // Create dummy input samples
        inputs0 = *outputs;
        for (int i = 0; i < inputs0.size(); i++)
        {
            WDL_TypedBuf<BL_FLOAT> &input = inputs0[i];
            BLUtils::FillAllZero(&input);
        }
    }
    
    AddSamples(inputs0/*inputs*/, scInputs);
    
    ProcessSamples();
    
    bool res = true;
    if (outputs != NULL)
    {
        int numRequested = inputs0[0].GetSize();
        res = GetResults(outputs, numRequested);
    }
    
    if (!res)
        // Result is not ready, fill the output with zeros
        // (to avoid undefined numbers sent to the host)
        //
        // NOTE: added for Rebalance
        //
    {
        if (outputs != NULL)
        {
            for (int i = 0; i < outputs->size(); i++)
            {
                BLUtils::FillAllZero(&(*outputs)[i]);
            }
        }
    }
    
    return res;
}

void
FftProcessObj14::MakeWindows(int bufSize, int overlapping,
                             enum WindowType analysisMethod,
                             enum WindowType synthesisMethod,
                             WDL_TypedBuf<BL_FLOAT> *analysisWindow,
                             WDL_TypedBuf<BL_FLOAT> *synthesisWindow)
{
    // Analysis and synthesis Hann windows
    // With Hanning, the gain is not increased when we increase the overlapping
    
    int anaWindowSize = bufSize;
    int synWindowSize = bufSize;
    
#if ADD_TAIL
    synWindowSize *= mFreqRes;
#endif
    
    bool variableHanning = ((analysisMethod == WindowVariableHanning) ||
                            (synthesisMethod == WindowVariableHanning));
    
    // NOTE: this is a simplificiation
    //
    // To simplify the code above and having too many tests
    //
    // We consider that if we use variable hanning, both windows will
    // be variable hanning
    //
    // But in the future, we may have to separate !
    //
    if (variableHanning)
    {
        if (analysisMethod == WindowVariableHanning)
            analysisMethod = WindowHanning;
        
        if (synthesisMethod == WindowVariableHanning)
            synthesisMethod = WindowHanning;
    }
    
    //
    // Hanning pow is used to narrow the analysis and synthesis windows
    // as much as possible, to have more "horizontal blur" in the spectrograms
    // (this reduces the attachs otherwise)
    //
    // Overlapping must be Hanning pow x 2 at the minimum
    // otherwise, we loose COLA condition
    //
    // Be careful, when using both analysis and synthesis windows,
    // it is as the Hanning pow was twice
    // (we have ana*syn equivalent to han*han).
    //
    //
    // NOTE: in the case of overlapping = 2, and two windows,
    // we will apply automatically the root Hanning, which is logical
    //
    // See: https://www.dsprelated.com/freebooks/sasp/overlap_add_ola_stft_processing.html
    // and
    //  https://www.dsprelated.com/freebooks/sasp/overlap_add_ola_stft_processing.html
    // Section: "Weighted Overlap Add"
    //
    // NOTE: here, we choose the maximum narrow value. Maybe we can narrow less by default
    //
    BL_FLOAT hanningFactor = 1.0;
    if (variableHanning && (overlapping > 1))
    {
        if ((analysisMethod == WindowHanning) ||
            (synthesisMethod == WindowHanning))
            // We happly windows twice (before, and after)
            hanningFactor = overlapping/4.0;
        else
            // We apply only one window, so we can narrow more
            hanningFactor = overlapping/2.0;
    }
    
    // Analysis
    //
    // NOTE: if no overlapping, it is sometimes better to even have an analysis window !
    //
    
    // FIX: if we want to analyse only, with overlapping > 1, we need a window !
    // (otherwise there would be some discontinuities, due to rectangular window)
    // (vertical clear bars in the spectrogram for Ghost for example)
    
    //if ((analysisMethod == WindowHanning) && (overlapping > 1))
    if (analysisMethod == WindowHanning)
        Window::MakeHanningPow(anaWindowSize, hanningFactor, analysisWindow);
    else
        Window::MakeSquare(anaWindowSize, 1.0, analysisWindow);
    
#if DEBUG_WINDOWS
    BL_FLOAT cola0 = Window::CheckCOLA(&mAnalysisWindow, mOverlapping);
    fprintf(stderr, "ANALYSIS - over: %d cola: %g\n", mOverlapping, cola0);
#endif
    
    // Synthesis
    
    // FIX (not tested): it should be logical to have synthesis window
    // for synthesis and overlapping == 1
    
    //if ((synthesisMethod == WindowHanning) && (overlapping > 1))
    if (synthesisMethod == WindowHanning)
        Window::MakeHanningPow(synWindowSize, hanningFactor, synthesisWindow);
    else
        Window::MakeSquare(synWindowSize, 1.0, synthesisWindow);
    
#if DEBUG_WINDOWS
    BL_FLOAT cola1 = Window::CheckCOLA(synthesisWindow, overlapping);
    fprintf(stderr, "SYNTHESIS - over: %d cola: %g\n", overlapping, cola1);
    fprintf(stderr, "\n");
#endif
    
    // Normalize
    if ((analysisMethod == WindowHanning) &&
        (synthesisMethod == WindowRectangular))
    {
        Window::NormalizeWindow(analysisWindow, overlapping);
    }
    else if((analysisMethod == WindowRectangular) &&
            (synthesisMethod == WindowHanning))
    {
        Window::NormalizeWindow(synthesisWindow, overlapping);
    }
    else if((analysisMethod == WindowHanning) &&
            (synthesisMethod == WindowHanning))
    {
        // Must make the test
        // Otherwise, when overlapping is 1 (no overlapping),
        // we have two rectangular windows, and cola returns -1
        if (overlapping > 1)
        {
            // We have both windows
            // We must compute the cola of the next hanning factor,
            // then normalize with it
            //
            
            WDL_TypedBuf<BL_FLOAT> win;
            Window::MakeHanningPow(analysisWindow->GetSize(), hanningFactor*2, &win);
            
            BL_FLOAT cola = Window::CheckCOLA(&win, overlapping);
            
            // Normalize only the synthesis window
            Window::NormalizeWindow(synthesisWindow, cola);
        }
    }
    else if ((analysisMethod == WindowRectangular) &&
             (synthesisMethod == WindowRectangular))
    {
        // Normalize only the synthesis window...
        Window::NormalizeWindow(synthesisWindow, overlapping);
    }
}

void
FftProcessObj14::GetAllSamples(vector<WDL_TypedBuf<BL_FLOAT> * > *samples)
{
    for (int i = 0; i < mChannels.size(); i++)
    {
        ProcessObjChannel *chan = mChannels[i];
    
        WDL_TypedBuf<BL_FLOAT> *s = chan->GetSamples();
    
        samples->push_back(s);
    }
}

void
FftProcessObj14::GetAllFftSamples(vector<WDL_TypedBuf<WDL_FFT_COMPLEX> * > *samples)
{
    for (int i = 0; i < mChannels.size(); i++)
    {
        ProcessObjChannel *chan = mChannels[i];
        
        WDL_TypedBuf<WDL_FFT_COMPLEX> *s = chan->GetFft();
        
        samples->push_back(s);
    }
}

void
FftProcessObj14::GetAllResultSamples(vector<WDL_TypedBuf<BL_FLOAT> * > *samples)
{
    for (int i = 0; i < mChannels.size(); i++)
    {
        ProcessObjChannel *chan = mChannels[i];
        
        WDL_TypedBuf<BL_FLOAT> *s = chan->GetResultSamples();
        
        samples->push_back(s);
    }
}

void
FftProcessObj14::ComputeFft(const WDL_TypedBuf<BL_FLOAT> &samples,
                            WDL_TypedBuf<WDL_FFT_COMPLEX> *fftSamples,
                            int freqRes)
{
    int bufSize = samples.GetSize();
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> tmpFftBuf;
    tmpFftBuf.Resize(bufSize);

    fftSamples->Resize(bufSize);
    
    // FIX: for FLStudio graphic freeze on AutoGain on Windows
    // This point really fixes the bug !
    //
    // (certainly that the data sent for fft was all zero,
    // and WDL_FFT has strange results when all the input samples are null
    if (BLUtils::IsAllZero(samples))
      {
	BLUtils::FillAllZero(fftSamples);
	
	return;
      }

    BL_FLOAT coeff = 1.0;
#if NORMALIZE_FFT
    if (bufSize > 0)
      // Test just in case (added for AutoGain FLStudio Win graphic freeze)
      coeff = 1.0/bufSize;
#endif
    
    // Fill the fft buf
    for (int i = 0; i < bufSize; i++)
    {
        // No need to divide by mBufSize if we ponderate analysis hanning window
        tmpFftBuf.Get()[i].re = samples.Get()[i]*coeff;
        tmpFftBuf.Get()[i].im = 0.0;
    }
    
    // Take FREQ_RES into account
    // Not sure we must do that only when normalizing or not
    for (int i = 0; i < bufSize; i++)
        tmpFftBuf.Get()[i].re *= freqRes;
    
    // Do the fft
    // Do it on the window but also in following the empty space, to capture remaining waves
    WDL_fft(tmpFftBuf.Get(), bufSize, false);
    
    // Sort the fft buffer
    for (int i = 0; i < bufSize; i++)
    {
        int k = WDL_fft_permute(bufSize, i);

        fftSamples->Get()[i].re = tmpFftBuf.Get()[k].re;
        fftSamples->Get()[i].im = tmpFftBuf.Get()[k].im;
    }
}

void
FftProcessObj14::ComputeInverseFft(const WDL_TypedBuf<WDL_FFT_COMPLEX> &fftSamples,
                                   WDL_TypedBuf<BL_FLOAT> *samples,
                                   int freqRes)
{
    int bufSize = fftSamples.GetSize();
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> tmpFftBuf;
    tmpFftBuf.Resize(bufSize);

    samples->Resize(fftSamples.GetSize());
    
    for (int i = 0; i < bufSize; i++)
    {
        int k = WDL_fft_permute(bufSize, i);
        
        tmpFftBuf.Get()[k].re = fftSamples.Get()[i].re;
        tmpFftBuf.Get()[k].im = fftSamples.Get()[i].im;
    }
    
    // Should not do this step when not necessary (for example for transients)
    
    // Do the ifft
    WDL_fft(tmpFftBuf.Get(), bufSize, true);

    for (int i = 0; i < bufSize; i++)
        samples->Get()[i] = tmpFftBuf.Get()[i].re;
    
    // fft
    BL_FLOAT coeff = 1.0;
#if !NORMALIZE_FFT
    // Not sure about that...
    //coeff = bufSize;
#endif
    
    // Take FREQ_RES into account
    // Not sure we must do that only when normalizing or not
    coeff *= 1.0/freqRes;
    
    for (int i = 0; i < bufSize; i++)
        samples->Get()[i] *= coeff;
}

void
FftProcessObj14::ComputeInverseFft(const WDL_TypedBuf<WDL_FFT_COMPLEX> &fftSamples,
                                   WDL_TypedBuf<WDL_FFT_COMPLEX> *iFftSamples)
{
    int bufSize = fftSamples.GetSize();
    
    WDL_TypedBuf<WDL_FFT_COMPLEX> tmpFftBuf;
    tmpFftBuf.Resize(bufSize);
    
    iFftSamples->Resize(fftSamples.GetSize());
    
    for (int i = 0; i < bufSize; i++)
    {
        int k = WDL_fft_permute(bufSize, i);
        
        tmpFftBuf.Get()[k].re = fftSamples.Get()[i].re;
        tmpFftBuf.Get()[k].im = fftSamples.Get()[i].im;
    }
    
    // Should not do this step when not necessary (for example for transients)
    
    // Do the ifft
    WDL_fft(tmpFftBuf.Get(), bufSize, true);
    
    for (int i = 0; i < bufSize; i++)
    {
        iFftSamples->Get()[i] = tmpFftBuf.Get()[i];
    }
}
