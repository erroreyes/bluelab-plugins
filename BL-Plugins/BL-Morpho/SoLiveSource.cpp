#include <GhostTrack2.h>

#include "SoLiveSource.h"

SoLiveSource::SoLiveSource(GhostTrack2 *ghostTrack)
{
    mGhostTrack = ghostTrack;
    mGhostTrack->UpdateParamMode(GhostTrack2::VIEW);
    mGhostTrack->ModeChanged(GhostTrack2::EDIT); // Necessary!
}

SoLiveSource::~SoLiveSource() {}

void
SoLiveSource::GetName(char name[FILENAME_SIZE])
{
    strcpy(name, "Live");
}
