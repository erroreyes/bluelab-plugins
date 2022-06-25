#ifndef __REBALANCE__
#define __REBALANCE__

#include "IPlug_include_in_plug_hdr.h"

#include <ParamSmoother.h>

#include <GraphControl10.h>
#include <SecureRestarter.h>
#include <DemoModeManager.h>

class RebalanceDumpFftObj;
class RebalanceProcessFftObj;
class FftProcessObj14;

// When compiled as standalone application, the plugin is used to dump audio data and binary masks
// When compiled as plugin, it is used to process the sound and rebalance it
class Rebalance : public IPlug
{
public:
  Rebalance(IPlugInstanceInfo instanceInfo);
  virtual ~Rebalance();
  
  void PreResizeGUI() {}
  
  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  
protected:
  void Init(int oversampling, int freqRes);
  
  bool OpenMixFile(const char *fileName);
  bool OpenSourceFile(const char *fileName);
    
  void Generate();
    
  void GenerateMSD100();
    
  FftProcessObj14 *mFftObj;
  
#if SA_API
    // Dump audio file and binary masks
  RebalanceDumpFftObj *mRebalanceDumpFftObj;
#else
    // Rebalance the sound
  vector<RebalanceProcessFftObj *> mRebalanceProcessFftObjs;
#endif
    
  // GUI
  GraphControl10 *mGraph;
    
  // Parameters
  double mVocal;
  double mVocalConfidence;
    
  double mBass;
  double mBassConfidence;
  
  double mDrums;
  double mDrumsConfidence;
  
  double mOther;
  double mOtherConfidence;
    
  // Files data
  WDL_TypedBuf<double> mCurrentMixChannel0;
  WDL_TypedBuf<double> mCurrentSourceChannel0;
    
private:
  DemoModeManager mDemoManager;
  
  // Secure starters
  SecureRestarter mSecureRestarter;
    
BL_PROFILE_DECLARE;
};

#endif
