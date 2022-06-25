#include <MorphoFrameSynthetizer.h>
#include <MorphoMixer.h>
#include <MorphoFrame7.h>

#include "MorphoSyPipeline.h"


MorphoSyPipeline::MorphoSyPipeline(SoSourceManager *soSourceManager,
                                   SySourceManager *sySourceManager,
                                   BL_FLOAT xyPadRatio)
{
    mLoop = false;
    mTimeStretchFactor = 1.0;
    mGain = 1.0;

    BL_FLOAT sampleRate = 44100.0;
    mFrameSynthetizer = new MorphoFrameSynthetizer(sampleRate);

    mMixer = new MorphoMixer(soSourceManager,
                             sySourceManager,
                             xyPadRatio);
}

MorphoSyPipeline::~MorphoSyPipeline()
{    
    delete mFrameSynthetizer;
    delete mMixer;
}

void
MorphoSyPipeline::Reset(BL_FLOAT sampleRate)
{
    mFrameSynthetizer->Reset(sampleRate);
}

void
MorphoSyPipeline::ProcessBlock(vector<WDL_TypedBuf<BL_FLOAT> > &out)
{
    MorphoFrame7 frame;
    mMixer->Mix(&frame);

    // Apply gain
    BL_FLOAT ampFactor = frame.GetAmpFactor();
    ampFactor *= mGain;
    frame.SetAmpFactor(ampFactor);
    
    mFrameSynthetizer->AddMorphoFrame(frame);
    mFrameSynthetizer->ProcessBlock(out);
}

void
MorphoSyPipeline::SetSynthMode(MorphoFrameSynth2::SynthMode synthMode)
{
    mFrameSynthetizer->SetSynthMode(synthMode);
}

void
MorphoSyPipeline::SetLoop(bool flag)
{
    mMixer->SetLoop(flag);
}

bool
MorphoSyPipeline::GetLoop() const
{
    return mMixer->GetLoop();
}

void
MorphoSyPipeline::SetTimeStretchFactor(BL_FLOAT factor)
{
    mTimeStretchFactor = factor;

    mMixer->SetTimeStretchFactor(mTimeStretchFactor);
}

BL_FLOAT
MorphoSyPipeline::GetTimeStretchFactor() const
{
    return mTimeStretchFactor;
}

void
MorphoSyPipeline::SetGain(BL_FLOAT gain)
{
    mGain = gain;
}

BL_FLOAT
MorphoSyPipeline::GetGain() const
{
    return mGain;
}
