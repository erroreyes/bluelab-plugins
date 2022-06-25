#ifndef SY_FILE_SOURCE_H
#define SY_FILE_SOURCE_H

#include <vector>
using namespace std;

#include <BLUtilsFile.h>

#include <SySourceImpl.h>

class SyFileSource : public SySourceImpl
{
 public:
    SyFileSource();
    virtual ~SyFileSource();

    void SetFileName(const char *fileName);
    void GetFileName(char fileName[FILENAME_SIZE]);

    void GetName(char name[FILENAME_SIZE]) override;

    //
    void SetFileMorphoFrames(const vector<MorphoFrame7> &frames);
    void ComputeCurrentMorphoFrame(MorphoFrame7 *frame) override;

    bool CanOutputSound() const;
    
    BL_FLOAT GetPlayPos() const;
    void SetPlayPos(BL_FLOAT t);
    void PlayAdvance(BL_FLOAT timeStretchCoeff);
    bool IsPlayFinished() const;
    
    // Parameters
    void SetReverse(bool flag);
    bool GetReverse() const;

    void SetPingPong(bool flag);
    bool GetPingPong() const;

    // Get selection on x only
    // Square selection for later
    void SetNormSelection(BL_FLOAT x0, BL_FLOAT x1);
    
protected:
    char mFileName[FILENAME_SIZE];

    // Parameters
    bool mReverse;
    bool mPingPong;
    
    vector<MorphoFrame7> mMorphoFrames;
    
    // Normalized play position
    BL_FLOAT mPlayPos;

    BL_FLOAT mNormSelection[2];
};

#endif
