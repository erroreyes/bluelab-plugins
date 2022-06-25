#ifndef SO_FILE_SOURCE_H
#define SO_FILE_SOURCE_H

#include <BLUtilsFile.h>

#include <SoSourceImpl.h>

#include <Morpho_defs.h>

class GhostTrack2;
class SoFileSource : public SoSourceImpl
{
 public:
    SoFileSource(GhostTrack2 *ghostTrack);
    virtual ~SoFileSource();

    void SetFileName(const char *fileName);
    void GetFileName(char fileName[FILENAME_SIZE]);

    void GetName(char name[FILENAME_SIZE]) override;
    
    // Parameters
    void SetSpectroSelectionType(SelectionType type);
    SelectionType GetSpectroSelectionType() const;

    // Get selection on x only
    // Square selection for later
    void GetNormSelection(BL_FLOAT *x0, BL_FLOAT *x1);
    
protected:
    char mFileName[FILENAME_SIZE];

    // Parameters
    SelectionType mSelectionType;

    //
    GhostTrack2 *mGhostTrack;
};

#endif
