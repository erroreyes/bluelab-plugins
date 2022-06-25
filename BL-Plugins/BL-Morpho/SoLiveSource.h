#ifndef SO_LIVE_SOURCE_H
#define SO_LIVE_SOURCE_H

#include <SoSourceImpl.h>

class GhostTrack2;
class SoLiveSource : public SoSourceImpl
{
 public:
    SoLiveSource(GhostTrack2 *ghostTrack);
    virtual ~SoLiveSource();

    void GetName(char name[FILENAME_SIZE]) override;

protected:
    GhostTrack2 *mGhostTrack;
};

#endif
