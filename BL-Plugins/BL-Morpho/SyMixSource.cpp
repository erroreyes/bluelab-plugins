#include "SyMixSource.h"

SyMixSource::SyMixSource() {}

SyMixSource::~SyMixSource() {}

void
SyMixSource::GetName(char name[FILENAME_SIZE])
{
    strcpy(name, "Mix");
}

void
SyMixSource::ComputeCurrentMorphoFrame(MorphoFrame7 *frame)
{
    *frame = mCurrentMorphoFrame;
}

void
SyMixSource::SetMixMorphoFrame(const MorphoFrame7 &frame)
{
    mCurrentMorphoFrame = frame;

    mCurrentMorphoFrame.ApplyMorphoFactors(mAmpFactor, mFreqFactor,
                                           mColorFactor, mWarpingFactor,
                                           mNoiseFactor);
}
