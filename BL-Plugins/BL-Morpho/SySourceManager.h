#ifndef SY_SOURCE_MANAGER_H
#define SY_SOURCE_MANAGER_H

#include <vector>
using namespace std;

class SySource;
class SySourceManager
{
 public:
    SySourceManager();
    virtual ~SySourceManager();

    void NewSource();

    int GetNumSources() const;
    SySource *GetSource(int index);
    void RemoveSource(int index);

    int GetCurrentSourceIdx() const;
    void SetCurrentSourceIdx(int index);
    SySource *GetCurrentSource();

    void Reset(BL_FLOAT sampleRate);
    
 protected:
    vector<SySource *> mSources;

    int mCurrentSourceIdx;

    BL_FLOAT mSampleRate;
};

#endif
