#ifndef SY_LIVE_SOURCE_H
#define SY_LIVE_SOURCE_H

#include <MorphoFrame7.h>

#include <SySourceImpl.h>

class SyLiveSource : public SySourceImpl
{
 public:
    SyLiveSource();
    virtual ~SyLiveSource();

    void GetName(char name[FILENAME_SIZE]) override;

    void SetLiveMorphoFrame(const MorphoFrame7 &frame);
    void ComputeCurrentMorphoFrame(MorphoFrame7 *frame) override;

protected:
    MorphoFrame7 mCurrentMorphoFrame;
};

#endif
