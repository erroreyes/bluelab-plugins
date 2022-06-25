#ifndef MORPHO_SY_PIPELINE_H
#define MORPHO_SY_PIPELINE_H

#include <vector>
using namespace std;

#include <BLTypes.h>

#include <MorphoFrameSynth2.h>

#include <Morpho_defs.h>

class SySourceManager;
class SySourcesView;
class MorphoFrameSynthetizer;
class MorphoMixer;
class MorphoSyPipeline
{
 public:
    MorphoSyPipeline(SoSourceManager *soSourceManager,
                     SySourceManager *sySourceManager,
                     BL_FLOAT xyPadRatio);
    virtual ~MorphoSyPipeline();

    void Reset(BL_FLOAT sampleRate);
    
    void ProcessBlock(vector<WDL_TypedBuf<BL_FLOAT> > &out);

    //
    void SetSynthMode(MorphoFrameSynth2::SynthMode synthMode);
    
    // Parameters
    void SetLoop(bool flag);
    bool GetLoop() const;
    
    void SetTimeStretchFactor(BL_FLOAT factor);
    BL_FLOAT GetTimeStretchFactor() const;
    
    // Out gain
    void SetGain(BL_FLOAT gain);
    BL_FLOAT GetGain() const;
    
 protected:
    MorphoFrameSynthetizer *mFrameSynthetizer;

    MorphoMixer *mMixer;
    
    //
    bool mLoop;
    BL_FLOAT mTimeStretchFactor;
    BL_FLOAT mGain;
};

#endif
