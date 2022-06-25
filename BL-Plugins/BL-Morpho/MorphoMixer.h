#ifndef MORPHO_MIXER_H
#define MORPHO_MIXER_H

#include <vector>
using namespace std;

#include <SySource.h>

class MorphoFrame7;
class SySourceManager;
class SoSourceManager;
class MorphoMixer
{
public:
    MorphoMixer(SoSourceManager *soSourceManager,
                SySourceManager *sySourceManager,
                BL_FLOAT xyPadRatio);
    virtual ~MorphoMixer();

    //
    void SetLoop(bool flag);
    bool GetLoop() const;
    
    void SetTimeStretchFactor(BL_FLOAT factor);

    //
    void Mix(MorphoFrame7 *result);

protected:
    void ComputeSourcesWeights(vector<BL_FLOAT> *weights);
    void NormalizeSourcesWeights(vector<BL_FLOAT> *weights);
    
    BL_FLOAT ComputeSourcesDistance(const SySource &source0,
                                    const SySource &source1);

    SySource *GetMasterSource();

    void PlayAdvance();
    bool IsPlayFinished() const;

    void AddFrame(MorphoFrame7 *result, /*const*/ MorphoFrame7 &frame,
                  const vector<BL_FLOAT> &weights, int index,
                  bool firstFrame);

    BL_FLOAT ComputeAmpFactor(const vector<BL_FLOAT> &weights, int index);
    BL_FLOAT ComputePitchFactor(const vector<BL_FLOAT> &weights, int index);
    BL_FLOAT ComputeColorFactor(const vector<BL_FLOAT> &weights, int index);
    BL_FLOAT ComputeWarpingFactor(const vector<BL_FLOAT> &weights, int index);
    BL_FLOAT ComputeNoiseFactor(const vector<BL_FLOAT> &weights, int index);
    
    bool IsSourceMuted(int index) const;
    bool IsAmpMuted(int index) const;
    bool IsPitchMuted(int index) const;
    bool IsColorMuted(int index) const;
    bool IsWarpingMuted(int index) const;
    bool IsNoiseMuted(int index) const;

    void UpdateSelection();
        
    //
    SoSourceManager *mSoSourceManager; // For source type
    SySourceManager *mSySourceManager;

    BL_FLOAT mXYPadRatio;

    bool mLoop;
    BL_FLOAT mTimeStretchFactor;
};
   
#endif
