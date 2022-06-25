#ifndef SY_SOURCE_IMPL_H
#define SY_SOURCE_IMPL_H

#include <BLTypes.h>
#include <BLUtilsFile.h>

#include <Morpho_defs.h>

class MorphoFrame7;
class SySourceImpl
{
 public:
    SySourceImpl();
    virtual ~SySourceImpl();

    virtual void GetName(char name[FILENAME_SIZE]) = 0;

    virtual void ComputeCurrentMorphoFrame(MorphoFrame7 *frame) = 0;

    void SetAmpFactor(BL_FLOAT amp);
    void SetPitchFactor(BL_FLOAT pitch);
    void SetColorFactor(BL_FLOAT color);
    void SetWarpingFactor(BL_FLOAT warping);
    void SetNoiseFactor(BL_FLOAT noise);
    
protected:
    BL_FLOAT mAmpFactor;
    BL_FLOAT mFreqFactor;
    BL_FLOAT mColorFactor;
    BL_FLOAT mWarpingFactor;
    BL_FLOAT mNoiseFactor;
};

#endif
