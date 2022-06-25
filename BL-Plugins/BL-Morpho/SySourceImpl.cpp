#include "SySourceImpl.h"

SySourceImpl::SySourceImpl()
{
    mAmpFactor = 1.0;
    mFreqFactor = 1.0;
    mColorFactor = 1.0;
    mWarpingFactor = 1.0;
    mNoiseFactor = 0.0;
}

SySourceImpl::~SySourceImpl() {}

void
SySourceImpl::SetAmpFactor(BL_FLOAT amp)
{
    mAmpFactor = amp;
}

void
SySourceImpl::SetPitchFactor(BL_FLOAT pitch)
{
    mFreqFactor = pitch;
}

void
SySourceImpl::SetColorFactor(BL_FLOAT color)
{
    mColorFactor = color;
}

void
SySourceImpl::SetWarpingFactor(BL_FLOAT warping)
{
    mWarpingFactor = warping;
}

void
SySourceImpl::SetNoiseFactor(BL_FLOAT noise)
{
    mNoiseFactor = noise;
}
