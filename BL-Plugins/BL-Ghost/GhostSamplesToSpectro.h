#ifndef GHOST_SAMPLES_TO_SPECTRO_H
#define GHOST_SAMPLES_TO_SPECTRO_H

#include <vector>
using namespace std;

#include <BLTypes.h>

#include "IPlug_include_in_plug_hdr.h"

// Take a selection on samples, and generate magns from samples
// Or take a selection on samples, and generate samples for magns
// Do this exactly aligned to 1 sample

class GhostTrack;
class SamplesToMagnPhases;
class GhostSamplesToSpectro
{
 public:
    // editOverlapping if the overlapping factor used when editing
    GhostSamplesToSpectro(GhostTrack *track, int editOverlapping = 4);
    virtual ~GhostSamplesToSpectro();
    
    void ReadSpectroDataSlice(vector<WDL_TypedBuf<BL_FLOAT> > magns[2],
                              vector<WDL_TypedBuf<BL_FLOAT> > phases[2],
                              BL_FLOAT minXNorm, BL_FLOAT maxNormX);

    void WriteSpectroDataSlice(vector<WDL_TypedBuf<BL_FLOAT> > magns[2],
                               vector<WDL_TypedBuf<BL_FLOAT> > phases[2],
                               BL_FLOAT minXNorm, BL_FLOAT maxNormX,
                               int fadeNumSamples = 0);

    // Read samples corresponding to bounds
    // But read them taking into account the selection over the frequencies
    void ReadSelectedSamples(vector<WDL_TypedBuf<BL_FLOAT> > *samples,
                             BL_FLOAT minXNorm, BL_FLOAT maxNormX);
    
 protected:
    void SaveState();
    void RestoreState();
    
    GhostTrack *mTrack;
    int mEditOverlapping;
    SamplesToMagnPhases *mSamplesToMagnPhases;

    // For Save/Restore state
    bool mSaveIsLoadingSaving;
    int mSaveOverlapping;
};
#endif
