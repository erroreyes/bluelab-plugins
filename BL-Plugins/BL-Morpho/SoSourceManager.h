#ifndef SO_SOURCE_MANAGER_H
#define SO_SOURCE_MANAGER_H

#include <vector>
using namespace std;

#include <BLTypes.h>

class SoSource;
class SoSourceManager
{
 public:
    SoSourceManager();
    virtual ~SoSourceManager();

    void NewSource();

    int GetNumSources() const;
    SoSource *GetSource(int index);
    void RemoveSource(int index);

    int GetCurrentSourceIdx() const;
    void SetCurrentSourceIdx(int index);
    SoSource *GetCurrentSource();

    void Reset(BL_FLOAT sampleRate);

 protected:    
    //
    vector<SoSource *> mSources;

    int mCurrentSourceIdx;

    BL_FLOAT mSampleRate;
};

#endif
