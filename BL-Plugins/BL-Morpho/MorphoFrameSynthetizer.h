#ifndef MORPHO_FRAME_SYNTHETIZER_H
#define MORPHO_FRAME_SYNTHETIZER_H

#include <vector>
using namespace std;

#include <BLTypes.h>

#include <MorphoFrameSynth2.h>
#include <MorphoFrame7.h>

class FftProcessObj16;
class MorphoFrameSynthetizerFftObj;
class MorphoFrameSynthetizer
{
 public:
    MorphoFrameSynthetizer(BL_FLOAT sampleRate);
    virtual ~MorphoFrameSynthetizer();

    void Reset(BL_FLOAT sampleRate);
    
    void ProcessBlock(vector<WDL_TypedBuf<BL_FLOAT> > &out);

    void SetSynthMode(MorphoFrameSynth2::SynthMode mode);

    void AddMorphoFrame(const MorphoFrame7 &frame);
    
 protected:
    FftProcessObj16 *mFftObj;

    MorphoFrameSynthetizerFftObj *mFrameSynthetizerObj;

private:
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
};

#endif
