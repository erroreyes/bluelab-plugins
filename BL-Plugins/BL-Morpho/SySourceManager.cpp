#include <SySource.h>

#include "SySourceManager.h"

SySourceManager::SySourceManager()
{
    mCurrentSourceIdx = -1;

    mSampleRate = 44100.0;
    
    // Create the "mix" source
    NewSource();
    SySource *source = GetCurrentSource();
    if (source != NULL)
        source->SetTypeMixSource();
}

SySourceManager::~SySourceManager()
{
    for (int i = 0; i < mSources.size(); i++)
    {
        SySource *source = mSources[i];
        delete source;
    }
}

void
SySourceManager::NewSource()
{
    SySource *source = new SySource(mSampleRate);
    mSources.push_back(source);

    mCurrentSourceIdx = mSources.size() - 1;
}

int
SySourceManager::GetNumSources() const
{
    return mSources.size();
}

SySource *
SySourceManager::GetSource(int index)
{
    if (index >= mSources.size())
        return NULL;

    return mSources[index];
}

void
SySourceManager::RemoveSource(int index)
{
    if (index >= mSources.size())
        return;

    SySource *source = mSources[index];

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
SySourceManager::GetCurrentSourceIdx() const
{
    return mCurrentSourceIdx;
}

void
SySourceManager::SetCurrentSourceIdx(int index)
{
    mCurrentSourceIdx = index;
}

SySource *
SySourceManager::GetCurrentSource()
{
    if ((mCurrentSourceIdx != -1) &&
        (mCurrentSourceIdx < mSources.size()))
        return mSources[mCurrentSourceIdx];
    
    return NULL;
}

void
SySourceManager::Reset(BL_FLOAT sampleRate)
{
    for (int i = 0; i < mSources.size(); i++)
    {
        SySource *source = mSources[i];
        source->Reset(sampleRate);
    }
}
