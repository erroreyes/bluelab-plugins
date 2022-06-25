#ifndef SY_SOURCE_H
#define SY_SOURCE_H

#include <vector>
using namespace std;

#include <WaterfallSource.h>

#include <BLUtilsFile.h>
#include <Morpho_defs.h>

// Source for the "synthesis" section
class SySourceImpl;
class View3DPluginInterface;
class MorphoFrame7;
class SySource : public WaterfallSource
{
 public:
    enum Type
    {
        NONE = 0,
        MIX,
        LIVE,
        FILE
    };
    
    SySource(BL_FLOAT sampleRate);
    virtual ~SySource();
    
    void SetTypeFileSource(const char *fileName = NULL);
    void SetTypeLiveSource();
    void SetTypeMixSource();
    
    Type GetType() const;

    void GetName(char name[FILENAME_SIZE]);

    bool CanOutputSound() const;
    
    // Get normalized play pos
    void PlayAdvance(BL_FLOAT timeStretchCoeff);
    bool IsPlayFinished() const;
    BL_FLOAT GetPlayPos() const;
    void SetPlayPos(BL_FLOAT t);
    
    void SetFileMorphoFrames(const vector<MorphoFrame7> &frames);
    void SetLiveMorphoFrame(const MorphoFrame7 &frame);
    void SetMixMorphoFrame(const MorphoFrame7 &frame);

    void ComputeCurrentMorphoFrame(MorphoFrame7 *frame);
        
    // Parameters
    void SetSourceSolo(bool flag);
    bool GetSourceSolo() const;
    
    void SetSourceMute(bool flag);
    bool GetSourceMute() const;
    
    void SetSourceMaster(bool flag);
    bool GetSourceMaster() const;

    void SetAmp(BL_FLOAT amp);
    BL_FLOAT GetAmp() const;

    void SetAmpSolo(bool flag);
    bool GetAmpSolo() const;
    
    void SetAmpMute(bool flag);
    bool GetAmpMute() const;
    
    void SetPitch(BL_FLOAT pitch);
    BL_FLOAT GetPitch() const;

    void SetPitchSolo(bool flag);
    bool GetPitchSolo() const;
    
    void SetPitchMute(bool flag);
    bool GetPitchMute() const;
    
    void SetColor(BL_FLOAT color);
    BL_FLOAT GetColor() const;

    void SetColorSolo(bool flag);
    bool GetColorSolo() const;
    
    void SetColorMute(bool flag);
    bool GetColorMute() const;

    void SetWarping(BL_FLOAT warping);
    BL_FLOAT GetWarping() const;

    void SetWarpingSolo(bool flag);
    bool GetWarpingSolo() const;
    
    void SetWarpingMute(bool flag);
    bool GetWarpingMute() const;

    void SetNoise(BL_FLOAT noise);
    BL_FLOAT GetNoise() const;

    void SetNoiseSolo(bool flag);
    bool GetNoiseSolo() const;
    
    void SetNoiseMute(bool flag);
    bool GetNoiseMute() const;

    void SetReverse(bool flag);
    bool GetReverse() const;

    void SetPingPong(bool flag);
    bool GetPingPong() const;

    void SetFreeze(bool flag);
    bool GetFreeze() const;
    
    void SetSynthType(SySourceSynthType type);
    SySourceSynthType GetSynthType() const;

    void SetMixPos(BL_FLOAT tx, BL_FLOAT ty);
    void GetMixPos(BL_FLOAT *tx, BL_FLOAT *ty) const;

    void SetSoSourceType(SoSourceType type);
    SoSourceType GetSoSourceType() const;

    // For file sources
    void SetNormSelection(BL_FLOAT x0, BL_FLOAT x1);
    
 protected:
    void UpdateGUI(const MorphoFrame7 &frame);

    void UpdatNewSourceImplFactors();
        
    //
    Type mType;
    SoSourceType mSoSourceType;
    
    SySourceImpl *mSourceImpl;

    // Parameters
    bool mSourceSolo;
    bool mSourceMute;
    bool mSourceMaster;

    BL_FLOAT mAmp;
    bool mAmpSolo;
    bool mAmpMute;

    BL_FLOAT mPitch;
    bool mPitchSolo;
    bool mPitchMute;

    BL_FLOAT mColor;
    bool mColorSolo;
    bool mColorMute;

    BL_FLOAT mWarping;
    bool mWarpingSolo;
    bool mWarpingMute;

    BL_FLOAT mNoise;
    bool mNoiseSolo;
    bool mNoiseMute;

    SySourceSynthType mSynthType;
    
    bool mFreeze;

    // Mix pos
    BL_FLOAT mMixPosX;
    BL_FLOAT mMixPosY;
};

#endif
