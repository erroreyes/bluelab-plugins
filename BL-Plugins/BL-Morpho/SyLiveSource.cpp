#include "SyLiveSource.h"

SyLiveSource::SyLiveSource() {}

SyLiveSource::~SyLiveSource() {}

void
SyLiveSource::GetName(char name[FILENAME_SIZE])
{
    strcpy(name, "Live");
}

void
SyLiveSource::SetLiveMorphoFrame(const MorphoFrame7 &frame)
{
    mCurrentMorphoFrame = frame;

    mCurrentMorphoFrame.ApplyMorphoFactors(mAmpFactor, mFreqFactor,
                                     mColorFactor, mWarpingFactor,
                                     mNoiseFactor);
}

void
SyLiveSource::ComputeCurrentMorphoFrame(MorphoFrame7 *frame)
{
    *frame = mCurrentMorphoFrame;
}
