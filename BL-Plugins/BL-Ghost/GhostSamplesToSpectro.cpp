#include "IControls.h"

#include <GhostTrack.h>

#include <SamplesToMagnPhases.h>

#include "IPlug_include_in_plug_hdr.h"

#include "GhostSamplesToSpectro.h"


GhostSamplesToSpectro::GhostSamplesToSpectro(GhostTrack *track,
                                             int editOverlapping)
{
    mTrack = track;

    mEditOverlapping = editOverlapping;

    mSaveIsLoadingSaving = false;
    mSaveOverlapping = editOverlapping;
    
    mSamplesToMagnPhases = new SamplesToMagnPhases(&track->mSamples,
                                                   track->mEditFftObj,
                                                   track->mSpectroEditObjs,
                                                   track->mSamplesPyramid);
}

GhostSamplesToSpectro::~GhostSamplesToSpectro()
{
    delete mSamplesToMagnPhases;
}

void
GhostSamplesToSpectro::SaveState()
{
    mSaveIsLoadingSaving = mTrack->mIsLoadingSaving;
    mSaveOverlapping = mTrack->mEditFftObj->GetOverlapping();

    mTrack->mEditFftObj->SetOverlapping(mEditOverlapping);
}

void
GhostSamplesToSpectro::RestoreState()
{
    if (mTrack->mCustomDrawer != NULL)
    {
        // FIX: edit during plaback, the playbar disappeared
        if (mTrack->mCustomDrawer->IsPlayBarActive())
        {
            
            mTrack->mCustomDrawer->SetSelPlayBarPos(0.0);
            
            mTrack->ResetPlayBar();
        }
    }
    
    mTrack->mEditFftObj->SetOverlapping(mSaveOverlapping);
    mTrack->mIsLoadingSaving = mSaveIsLoadingSaving;
}

void
GhostSamplesToSpectro::ReadSpectroDataSlice(vector<WDL_TypedBuf<BL_FLOAT> > magns[2],
                                            vector<WDL_TypedBuf<BL_FLOAT> > phases[2],
                                            BL_FLOAT minXNorm, BL_FLOAT maxXNorm)
{
    SaveState();
    mTrack->mIsLoadingSaving = true;

    mSamplesToMagnPhases->ReadSpectroDataSlice(magns, phases, minXNorm, maxXNorm);
    
    RestoreState();
}

void
GhostSamplesToSpectro::
WriteSpectroDataSlice(vector<WDL_TypedBuf<BL_FLOAT> > magns[2],
                      vector<WDL_TypedBuf<BL_FLOAT> > phases[2],
                      BL_FLOAT minXNorm,
                      BL_FLOAT maxXNorm,
                      int fadeNumSamples)
{
    SaveState();
    mTrack->mIsLoadingSaving = true;
    
    mSamplesToMagnPhases->WriteSpectroDataSlice(magns, phases,
                                                minXNorm, maxXNorm, fadeNumSamples);
    
    RestoreState();
}

void
GhostSamplesToSpectro::ReadSelectedSamples(vector<WDL_TypedBuf<BL_FLOAT> > *samples,
                                           BL_FLOAT minXNorm, BL_FLOAT maxNormX)
{
    SaveState();
    mTrack->mIsLoadingSaving = true;
    
    mSamplesToMagnPhases->ReadSelectedSamples(samples, minXNorm, maxNormX);

    RestoreState();
}
