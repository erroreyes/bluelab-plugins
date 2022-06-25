#include <SoSource.h>


#include "SoSourceManager.h"

SoSourceManager::SoSourceManager()
{
    mCurrentSourceIdx = -1;

    mSampleRate = 44100.0;
}

SoSourceManager::~SoSourceManager()
{
    for (int i = 0; i < mSources.size(); i++)
    {
        SoSource *source = mSources[i];
        delete source;
    }
}

void
SoSourceManager::NewSource()
{
    SoSource *source = new SoSource(mSampleRate);
    mSources.push_back(source);
}

int
SoSourceManager::GetNumSources() const
{
    return mSources.size();
}

SoSource *
SoSourceManager::GetSource(int index)
{
    if (index >= mSources.size())
        return NULL;

    return mSources[index];
}

void
SoSourceManager::RemoveSource(int index)
{
    if (index >= mSources.size())
        return;

    SoSource *source = mSources[index];

    // Erase dos not call the destructor...
    mSources.erase(mSources.begin() + index);

    if (source != NULL)
        delete source;
    
    if (mCurrentSourceIdx >= mSources.size())
        // We removed the last source
        // Update the index
        mCurrentSourceIdx = mSources.size() - 1;
}

int
SoSourceManager::GetCurrentSourceIdx() const
{
    return mCurrentSourceIdx;
}

void
SoSourceManager::SetCurrentSourceIdx(int index)
{
    mCurrentSourceIdx = index;
}

SoSource *
SoSourceManager::GetCurrentSource()
{
    if ((mCurrentSourceIdx != -1) &&
        (mCurrentSourceIdx < mSources.size()))
        return mSources[mCurrentSourceIdx];
    
    return NULL;
}

void
SoSourceManager::Reset(BL_FLOAT sampleRate)
{
    for (int i = 0; i < mSources.size(); i++)
    {
        SoSource *source = mSources[i];
        source->Reset(sampleRate);
    }
}
