#ifndef MORPHO_SO_PIPELINE_H
#define MORPHO_SO_PIPELINE_H

#include <vector>
using namespace std;

#include <BLTypes.h>

#include <MorphoFrameSynth2.h> // For mode

#include <Morpho_defs.h>

class MorphoFrameAnalyzer;
class MorphoFrameSynthetizer;
class MorphoSoPipeline
{
 public:
    MorphoSoPipeline(BL_FLOAT sampleRate);
    virtual ~MorphoSoPipeline();

    void Reset(BL_FLOAT sampleRate);
    
    void ProcessBlock(const vector<WDL_TypedBuf<BL_FLOAT> > &in,
                      vector<WDL_TypedBuf<BL_FLOAT> > &out,
                      vector<MorphoFrame7> *resultFrames);
    
    // Parameters
    void SetSynthMode(MorphoFrameSynth2::SynthMode mode);
    void SetTimeSmoothCoeff(BL_FLOAT coeff);
    void SetDetectThreshold(BL_FLOAT detectThrs);
    void SetFreqThreshold(BL_FLOAT freqThrs);

    void SetGain(BL_FLOAT gain);
    
 protected:
    MorphoFrameAnalyzer *mFrameAnalyzer;
    MorphoFrameSynthetizer *mFrameSynthetizer;
    
    //
    BL_FLOAT mGain;
};

#endif
