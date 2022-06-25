#ifndef MORPHO_FRAME_ANALYZER_H
#define MORPHO_FRAME_ANALYZER_H

#include <vector>
using namespace std;

#include <BLTypes.h>

#include <MorphoFrame7.h>

class FftProcessObj16;
class MorphoFrameAnalyzerFftObj;
class PartialTracker8; // TMP
class MorphoFrameAnalyzer
{
 public:
    MorphoFrameAnalyzer(BL_FLOAT sampleRate, bool storeDetectDataInFrames);
    virtual ~MorphoFrameAnalyzer();

    void Reset(BL_FLOAT sampleRate);
    
    void ProcessBlock(const vector<WDL_TypedBuf<BL_FLOAT> > &in);

    void GetMorphoFrames(vector<MorphoFrame7> *frames);

    // Parameters
    void SetDetectThreshold(BL_FLOAT detectThrs);
    void SetFreqThreshold(BL_FLOAT freqThrs);
    void SetTimeSmoothCoeff(BL_FLOAT coeff);
    
 protected:
    FftProcessObj16 *mFftObj;

    MorphoFrameAnalyzerFftObj *mFrameAnalyzerObj;

private:
    vector<WDL_TypedBuf<BL_FLOAT> > mTmpBuf0;
};

#endif
