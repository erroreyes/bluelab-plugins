#ifndef SY_MIX_SOURCE_H
#define SY_MIX_SOURCE_H

#include <MorphoFrame7.h>

#include <SySourceImpl.h>

class SyMixSource : public SySourceImpl
{
 public:
    SyMixSource();
    virtual ~SyMixSource();

    void GetName(char name[FILENAME_SIZE]) override;

    void SetMixMorphoFrame(const MorphoFrame7 &frame);
    void ComputeCurrentMorphoFrame(MorphoFrame7 *frame) override;
    
protected:
    MorphoFrame7 mCurrentMorphoFrame;
};

#endif
