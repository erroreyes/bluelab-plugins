#ifndef SO_SOURCE_H
#define SO_SOURCE_H

#include <stdlib.h>

#include <vector>
using namespace std;

#include "IPlug_include_in_plug_hdr.h"
using namespace iplug;
using namespace igraphics;

#include <BLUtilsFile.h>

#include <WaterfallSource.h>

#include <MorphoFrame7.h>

#include <Morpho_defs.h>

// Source for the "sources" section
class SoSourceImpl;
class GhostTrack2;
class MorphoSoPipeline;
class MorphoWaterfallGUI;
class GUIHelper12;
class View3DPluginInterface;
class SoSource : public WaterfallSource
{
 public:
    enum Type
    {
        NONE = 0,
        LIVE,
        FILE
    };
    
    SoSource(BL_FLOAT sampleRate);
    virtual ~SoSource();

    void SetTypeFileSource(const char *fileName = NULL);    
    void SetTypeLiveSource();

    Type GetType() const;
    
    void GetName(char name[FILENAME_SIZE]);

    // For file sources / "apply"
    void ComputeFileFrames(vector<MorphoFrame7> *frames);
    
    // For live sources, compute ~1 frame
    bool ComputeLiveFrame(const vector<WDL_TypedBuf<BL_FLOAT> > &in,
                          MorphoFrame7 *frame);
    
    //
    void Reset(BL_FLOAT sampleRate);

    void ProcessBlock(vector<WDL_TypedBuf<BL_FLOAT> > &in,
                      vector<WDL_TypedBuf<BL_FLOAT> > &out,
                      bool isTransportPlaying,
                      BL_FLOAT transportSamplePos);

    void OnUIOpen();
    void OnUIClose();

    void SetViewEnabled(bool flag);

    void CreateSpectroControls(GUIHelper12 *guiHelper,
                               Plugin *plug,
                               int grapX, int graphY,
                               const char *graphBitmapFN);
    
    void CheckRecomputeSpectro();
    
    // Per source parameters
    //
    void SetPlaying(bool flag);
    bool IsPlaying() const;
    
    void SetSpectroBrightness(BL_FLOAT brightness);
    BL_FLOAT GetSpectroBrightness() const;
    
    void SetSpectroContrast(BL_FLOAT contrast);
    BL_FLOAT GetSpectroContrast() const;
    
    void SetSpectroSpecWave(BL_FLOAT specWave);
    BL_FLOAT GetSpectroSpecWave() const;
    
    void SetSpectroWaveformScale(BL_FLOAT waveScale);
    BL_FLOAT GetSpectroWaveformScale() const;
    
    void SetSpectroSelectionType(SelectionType type);
    SelectionType GetSpectroSelectionType() const;
    
    void SetSourceMaster(bool sourceMaster);
    bool GetSourceMaster() const;
    
    void SetSourceType(SoSourceType sourceType);
    SoSourceType GetSourceType() const;
    
    void SetTimeSmoothCoeff(BL_FLOAT timeSmooth);
    BL_FLOAT GetTimeSmoothCoeff() const;
    
    void SetDetectThreshold(BL_FLOAT detectThrs);
    BL_FLOAT GetDetectThreshold() const;
    
    void SetFreqThreshold(BL_FLOAT freqThrs);
    BL_FLOAT GetFreqThreshold() const;
    
    void SetSourceGain(BL_FLOAT sourceGain);
    BL_FLOAT GetSourceGain() const;

    // If is touched, will need to push "apply"
    bool IsTouched() const;

    // For file sources
    void GetNormSelection(BL_FLOAT *x0, BL_FLOAT *x1);
    
 protected:
    Type mType;

    SoSourceImpl *mSourceImpl;

    // Parameters
    bool mIsPlaying;
    
    BL_FLOAT mBrightness;
    BL_FLOAT mContrast;
    BL_FLOAT mSpecWave;
    BL_FLOAT mWaveScale;

    bool mSourceMaster;

    SoSourceType mSourceType;

    BL_FLOAT mTimeSmooth;
    BL_FLOAT mDetectThreshold;
    BL_FLOAT mFreqThreshold;
    BL_FLOAT mSourceGain;
    
    //
    BL_FLOAT mSampleRate;

    //
    GhostTrack2 *mGhostTrack;
    MorphoSoPipeline *mPipeline;

    bool mIsTouched;
    
private:
    vector<WDL_TypedBuf<BL_FLOAT> > mTmp0;
    vector<WDL_TypedBuf<BL_FLOAT> > mTmp1;
};

#endif
