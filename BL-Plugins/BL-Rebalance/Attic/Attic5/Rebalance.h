#ifndef __REBALANCE__
#define __REBALANCE__

#include "IPlug_include_in_plug_hdr.h"

#include <ParamSmoother.h>

#include <SecureRestarter.h>
#include <DemoModeManager.h>

#define PROFILE_RNN 0 //1

class RebalanceDumpFftObj;
class RebalanceProcessFftObj;
class MaskPredictor;
class FftProcessObj14;

// When compiled as standalone application, the plugin is used to dump audio data and binary masks
// When compiled as plugin, it is used to process the sound and rebalance it
class Rebalance : public IPlug
{
public:
  enum Mode
  {
      SOFT = 0,
      HARD
  };
    
  Rebalance(IPlugInstanceInfo instanceInfo);
  virtual ~Rebalance();
  
  void PreResizeGUI() {}
  
  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  
protected:
  void Init(int oversampling, int freqRes);
  
  void InitSaBinaryMasks(int oversampling, int freqRes);
  void InitSaNormMasks(int oversampling, int freqRes);
  void InitPlug(int oversampling, int freqRes);
    
  bool OpenMixFile(const char *fileName);
  bool OpenSourceFile(const char *fileName);
    
  bool OpenSourceFile2(const char *fileName, WDL_TypedBuf<double> *result);
    
  void Generate();
    
  void GenerateMSD100();
  
  // Normalized version
  void GenerateNorm();
    
  void GenerateMSD100Norm();
    
  void DumpNormFiles();
    
  void NormalizeSourceSlices(vector<vector<WDL_TypedBuf<double> > > sourceSlices[4]);
    
    
  // Will use only mFftObjs[0] if we compute ordinary binary masks
  FftProcessObj14 *mFftObjs[4];
  

  // For SA_API
  
    // Dump audio file and binary masks
  RebalanceDumpFftObj *mRebalanceDumpFftObjs[4];

  // For PLUG_API
    
  // Rebalance the sound
  vector<RebalanceProcessFftObj *> mRebalanceProcessFftObjs;
    
  MaskPredictor *mMaskPred;
    
  // Mode
  Mode mMode;
    
  // Parameters
  
  // Mix
  double mVocal;
  double mBass;
  double mDrums;
  double mOther;
  
  // Precision
  double mVocalPrecision;
  double mBassPrecision;
  double mDrumsPrecision;
  double mOtherPrecision;
    
  // Files data
  WDL_TypedBuf<double> mCurrentMixChannel0;
  WDL_TypedBuf<double> mCurrentSourceChannels0[4];
    
private:
  DemoModeManager mDemoManager;
  
  // Secure starters
  SecureRestarter mSecureRestarter;
    
#if PROFILE_RNN
    BlaTimer __timer0;
    long __timerCount0;
#endif
    
BL_PROFILE_DECLARE;
};

#endif
