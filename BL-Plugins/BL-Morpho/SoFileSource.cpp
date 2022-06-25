#include <GhostTrack2.h>

#include <Morpho_defs.h>

#include "SoFileSource.h"

SoFileSource::SoFileSource(GhostTrack2 *ghostTrack)
{
    memset(mFileName, '\0', FILENAME_SIZE);

    mSelectionType = RECTANGLE;

    mGhostTrack = ghostTrack;

    // Behaves as the standalone Ghost version (load file etc.)
    mGhostTrack->SetAppAPIHack(true);
    
    mGhostTrack->UpdateParamMode(GhostTrack2::EDIT);
    mGhostTrack->ModeChanged(GhostTrack2::EDIT); // Necessary?
}

SoFileSource::~SoFileSource() {}

void 
SoFileSource::SetFileName(const char *fileName)
{
    strcpy(mFileName, fileName);

    mGhostTrack->OpenFile(fileName);
}

void
SoFileSource::GetFileName(char fileName[FILENAME_SIZE])
{
    strcpy(fileName, mFileName);
}

void
SoFileSource::SetSpectroSelectionType(SelectionType type)
{
    mSelectionType = type;

    mGhostTrack->UpdateParamSelectionType(type);
}

SelectionType
SoFileSource::GetSpectroSelectionType() const
{
    return mSelectionType;
}

void
SoFileSource::GetNormSelection(BL_FLOAT *x0, BL_FLOAT *x1)
{
    BL_FLOAT y0;
    BL_FLOAT y1;
    bool res = mGhostTrack->GetNormDataSelection(x0, &y0, x1, &y1);
}

void
SoFileSource::GetName(char name[FILENAME_SIZE])
{
    GetFileName(name);
}
