set(BLUELAB_DEPS ${IPLUG2_DIR}/BL-Dependencies)

#
add_library(_bluelab-lib INTERFACE)
set(BLUELAB_LIB_SRC "${BLUELAB_DEPS}/bluelab-lib/")
set(_bluelab-lib_src
  AirProcess2.cpp
  AirProcess2.h
  AirProcess.cpp
  AirProcess.h
  AnticlickObj.cpp
  AnticlickObj.h
  AudioFile.cpp
  AudioFile.h
  AutoGainObj.cpp
  AutoGainObj.h
  AvgHistogram.cpp
  AvgHistogram.h
  AWeighting.cpp
  AWeighting.h
  Axis3D.cpp
  Axis3DFactory2.cpp
  Axis3DFactory2.h
  Axis3DFactory.cpp
  Axis3DFactory.h
  Axis3D.h
  BatFftObj2.cpp
  BatFftObj2.h
  BatFftObj3.cpp
  BatFftObj3.h
  BatFftObj4.cpp
  BatFftObj4.h
  BatFftObj5.cpp
  BatFftObj5.h
  BatFftObj.cpp
  BatFftObj.h
  BlaTimer.cpp
  BlaTimer.h
  BLBitmap.cpp
  BLBitmap.h
  BLCircleGraphDrawer.cpp
  BLCircleGraphDrawer.h
  BLCorrelationComputer.cpp
  BLCorrelationComputer.h
  BLDebug.cpp
  BLDebug.h
  BLFireworks.cpp
  BLFireworks.h
  BLImage.cpp
  BLImage.h
  BLLissajousGraphDrawer.cpp
  BLLissajousGraphDrawer.h
  BLProfiler.cpp
  BLProfiler.h
  BLReverb.cpp
  BLReverb.h
  BLReverbIR.cpp
  BLReverbIR.h
  BLReverbSndF.cpp
  BLReverbSndF.h
  BLReverbViewer.cpp
  BLReverbViewer.h
  BLSpectrogram4.cpp
  BLSpectrogram4.h
  BLTypes.h
  BLUpmixGraphDrawer.cpp
  BLUpmixGraphDrawer.h
  BLUtils.cpp
  BLUtils.h
  BLVectorscope.cpp
  BLVectorscope.h
  BLVectorscopeProcess.cpp
  BLVectorscopeProcess.h
  BLVumeter2SidesControl.cpp
  BLVumeter2SidesControl.h
  BLVumeterControl.cpp
  BLVumeterControl.h
  BLVumeterNeedleControl.cpp
  BLVumeterNeedleControl.h
  Bufferizer.cpp
  Bufferizer.h
  BufProcessObj.h
  BufProcessObjSmooth2.cpp
  BufProcessObjSmooth2.h
  BufProcessObjSmooth.cpp
  BufProcessObjSmooth.h
  ChromaFftObj2.cpp
  ChromaFftObj2.h
  ChromaFftObj.cpp
  ChromaFftObj.h
  ChunkHelper.cpp
  CMA2Smoother.cpp
  CMA2Smoother.h
  CMAParamSmooth2.cpp
  CMAParamSmooth2.h
  CMAParamSmooth.cpp
  CMAParamSmooth.h
  CMASmoother.cpp
  CMASmoother.h
  ColorMap2.cpp
  ColorMap2.h
  ColorMap3.cpp
  ColorMap3.h
  ColorMap4.cpp
  ColorMap4.h
  ColorMap.cpp
  ColorMapFactory.cpp
  ColorMapFactory.h
  ColorMap.h
  CParamSmooth.h
  CrossoverSplitter2Bands.cpp
  CrossoverSplitter2Bands.h
  CrossoverSplitter3Bands.cpp
  CrossoverSplitter3Bands.h
  CrossoverSplitter5Bands.cpp
  CrossoverSplitter5Bands.h
  CrossoverSplitterNBands2.cpp
  CrossoverSplitterNBands2.h
  CrossoverSplitterNBands3.cpp
  CrossoverSplitterNBands3.h
  CrossoverSplitterNBands4.cpp
  CrossoverSplitterNBands4.h
  CrossoverSplitterNBands.cpp
  CrossoverSplitterNBands.h
  DbgSpectrogram.cpp
  DbgSpectrogram.h
  DebugDataDumper.cpp
  DebugDataDumper.h
  DebugGraph.cpp
  DebugGraph.h
  DelayLinePhaseShift2.cpp
  DelayLinePhaseShift2.h
  DelayLinePhaseShift.cpp
  DelayLinePhaseShift.h
  DelayObj2.cpp
  DelayObj2.h
  DelayObj3.cpp
  DelayObj3.h
  DelayObj4.cpp
  DelayObj4.h
  DelayObj.cpp
  DelayObj.h
  DemoModeManager.cpp
  DemoModeManager.h
  Denoiser_defs.h
  DenoiserObj.cpp
  DenoiserObj.h
  DenoiserPostTransientObj.cpp
  DenoiserPostTransientObj.h
  DiracGenerator.cpp
  DiracGenerator.h
  DitherMaker.cpp
  DitherMaker.h
  DNNModelDarknetMc.cpp
  DNNModelDarknetMc.h
  DNNModel.h
  DNNModelMc.h
  DualDelayLine.cpp
  DualDelayLine.h
  DUETCustomControl.cpp
  DUETCustomControl.h
  DUETCustomDrawer.cpp
  DUETCustomDrawer.h
  DUETFftObj2.cpp
  DUETFftObj2.h
  DUETFftObj.cpp
  DUETFftObj.h
  DUETHistogram2.cpp
  DUETHistogram2.h
  DUETHistogram.cpp
  DUETHistogram.h
  DUETPlugInterface.h
  DUETSeparator2.cpp
  DUETSeparator2.h
  DUETSeparator3.cpp
  DUETSeparator3.h
  DUETSeparator.cpp
  DUETSeparator.h
  EarlyReflect.cpp
  EarlyReflect.h
  EarlyReflections2.cpp
  EarlyReflections2.h
  EarlyReflections.cpp
  EarlyReflections.h
  EnvelopeGenerator2.cpp
  EnvelopeGenerator2.h
  EnvelopeGenerator3.cpp
  EnvelopeGenerator3.h
  EnvelopeGenerator.cpp
  EnvelopeGenerator.h
  EQHackFftObj.cpp
  EQHackFftObj.h
  EQHackPluginInterface.h
  FastRTConvolver3.cpp
  FastRTConvolver3.h
  FftConvolver2.cpp
  FftConvolver2.h
  FftConvolver3.cpp
  FftConvolver3.h
  FftConvolver4.cpp
  FftConvolver4.h
  FftConvolver5.cpp
  FftConvolver5.h
  FftConvolver6.cpp
  FftConvolver6.h
  FftConvolver.cpp
  FftConvolver.h
  FftConvolverSmooth2.cpp
  FftConvolverSmooth2.h
  FftConvolverSmooth3.cpp
  FftConvolverSmooth3.h
  FftConvolverSmooth4.cpp
  FftConvolverSmooth4.h
  FftConvolverSmooth.cpp
  FftConvolverSmooth.h
  FftProcessBufObj.cpp
  FftProcessBufObj.h
  FftProcessObj16.cpp
  FftProcessObj16.h
  FifoDecimator2.cpp
  FifoDecimator2.h
  FifoDecimator.cpp
  FifoDecimator.h
  FillTriangle.hpp
  FilterButterworthLPF.cpp
  FilterButterworthLPF.h
  FilterButterworthLPFNX.cpp
  FilterButterworthLPFNX.h
  FilterFftLowPass.cpp
  FilterFftLowPass.h
  FilterFreqResp.cpp
  FilterFreqResp.h
  FilterIIRLow12dB.cpp
  FilterIIRLow12dB.h
  FilterIIRLow12dBNX.cpp
  FilterIIRLow12dBNX.h
  FilterLR2Crossover.cpp
  FilterLR2Crossover.h
  FilterLR4Crossover.cpp
  FilterLR4Crossover.h
  FilterRBJ1X.cpp
  FilterRBJ1X.h
  FilterRBJ2X2.cpp
  FilterRBJ2X2.h
  FilterRBJ2X.cpp
  FilterRBJ2X.h
  FilterRBJFeed.cpp
  FilterRBJFeed.h
  FilterRBJ.h
  FilterRBJNX.cpp
  FilterRBJNXEx.cpp
  FilterRBJNXEx.h
  FilterRBJNX.h
  FilterSincConvoBandPass.cpp
  FilterSincConvoBandPass.h
  FilterSincConvoLPF.cpp
  FilterSincConvoLPF.h
  FilterTransparentRBJ2X2.cpp
  FilterTransparentRBJ2X2.h
  FilterTransparentRBJ2X.cpp
  FilterTransparentRBJ2X.h
  FilterTransparentRBJNX.cpp
  FilterTransparentRBJNX.h
  FixedBufferObj.cpp
  FixedBufferObj.h
  FreqAdjustObj2.cpp
  FreqAdjustObj2.h
  FreqAdjustObj3.cpp
  FreqAdjustObj3.h
  FreqAdjustObj.cpp
  FreqAdjustObj.h
  GetTimeOfDay.cpp
  GetTimeOfDay.h
  GhostCommandCopyPaste.cpp
  GhostCommandCopyPaste.h
  GhostCommand.cpp
  GhostCommandCut.cpp
  GhostCommandCut.h
  GhostCommandGain.cpp
  GhostCommandGain.h
  GhostCommand.h
  GhostCommandHistory.cpp
  GhostCommandHistory.h
  GhostCommandReplace.cpp
  GhostCommandReplace.h
  GhostCustomControl.cpp
  GhostCustomControl.h
  GhostCustomDrawer.cpp
  GhostCustomDrawer.h
  GhostPluginInterface.cpp
  GhostPluginInterface.h
  GhostTriggerControl.cpp
  GhostTriggerControl.h
  GhostViewerFftObj.cpp
  GhostViewerFftObj.h
  GhostViewerFftObjSubSonic.cpp
  GhostViewerFftObjSubSonic.h
  GraphAmpAxis.cpp
  GraphAmpAxis.h
  GraphAxis2.cpp
  GraphAxis2.h
  GraphControl11.cpp
  GraphControl11.h
  GraphControl12.cpp
  GraphControl12.h
  GraphCurve4.cpp
  GraphCurve4.h
  GraphCurve5.cpp
  GraphCurve5.h
  GraphFader2.cpp
  GraphFader2.h
  GraphFader3.cpp
  GraphFader3.h
  GraphFreqAxis2.cpp
  GraphFreqAxis2.h
  GraphFreqAxis.cpp
  GraphFreqAxis.h
  GraphSwapColor.h
  GraphTimeAxis2.cpp
  GraphTimeAxis2.h
  GraphTimeAxis3.cpp
  GraphTimeAxis3.h
  GraphTimeAxis4.cpp
  GraphTimeAxis4.h
  GraphTimeAxis5.cpp
  GraphTimeAxis5.h
  GraphTimeAxis6.cpp
  GraphTimeAxis6.h
  GraphTimeAxis.cpp
  GraphTimeAxis.h
  GtiInpaint.cpp
  GtiInpaint.h
  GUIHelper11.cpp
  GUIHelper11.h
  GUIHelper12.cpp
  GUIHelper12.h
  HistoMaskLine2.cpp
  HistoMaskLine2.h
  HistoMaskLine.cpp
  HistoMaskLine.h
  Hrtf.cpp
  Hrtf.h
  IBitmapControlAnim.cpp
  IBitmapControlAnim.h
  IGUIResizeButtonControl.cpp
  IGUIResizeButtonControl.h
  IHelpButtonControl.cpp
  IHelpButtonControl.h
  IKeyboardControl.h
  ImageDisplay2.cpp
  ImageDisplay2.h
  ImageDisplayColor.cpp
  ImageDisplayColor.h
  ImageDisplay.cpp
  ImageDisplay.h
  ImageInpaint2.cpp
  ImageInpaint2.h
  ImageInpaint.cpp
  ImageInpaint.h
  ImageSmootherKernel.cpp
  ImageSmootherKernel.h
  ImpulseResponseExtractor.cpp
  ImpulseResponseExtractor.h
  ImpulseResponseSet.cpp
  ImpulseResponseSet.h
  InfraProcess2.cpp
  InfraProcess2.h
  InfraProcess.cpp
  InfraProcess.h
  InfrasonicViewerFftObj2.cpp
  InfrasonicViewerFftObj2.h
  InfrasonicViewerFftObj.cpp
  InfrasonicViewerFftObj.h
  InfraSynth_defs.h
  InfraSynthNotesQueue.cpp
  InfraSynthNotesQueue.h
  InfraSynthProcess.cpp
  InfraSynthProcess.h
  InstantCompressor.cpp
  InstantCompressor.h
  IRadioButtonsControl.cpp
  IRadioButtonsControl.h
  IRolloverButtonControl.cpp
  IRolloverButtonControl.h
  IsophoteInpaint.cpp
  IsophoteInpaint.h
  JReverb.cpp
  JReverb.h
  KalmanParamSmooth.cpp
  KalmanParamSmooth.h
  KemarHrtf.cpp
  KemarHrtf.h
  LinesRender2.cpp
  LinesRender2.h
  LinesRender.cpp
  LinesRender.h
  LUFSMeter.cpp
  LUFSMeter.h
  MelScale.cpp
  MelScale.h
  MiniView2.cpp
  MiniView2.h
  MiniView.cpp
  MiniView.h
  MinMaxAvgHistogram.cpp
  MinMaxAvgHistogramDB.cpp
  MinMaxAvgHistogramDB.h
  MinMaxAvgHistogram.h
  MouseCustomControl.h
  MultiViewer2.cpp
  MultiViewer2.h
  MultiViewer.cpp
  MultiViewer.h
  my-bzero.c
  my-bzero.h
  NotesQueue.cpp
  NotesQueue.h
  OnsetDetector.cpp
  OnsetDetector.h
  OnsetDetectProcess.cpp
  OnsetDetectProcess.h
  Oscillator.cpp
  Oscillator.h
  Oversampler2.cpp
  Oversampler2.h
  Oversampler3.cpp
  Oversampler3.h
  Oversampler4.cpp
  Oversampler4.h
  Oversampler5.cpp
  Oversampler5.h
  Oversampler.cpp
  Oversampler.h
  OversampProcessObj2.cpp
  OversampProcessObj2.h
  OversampProcessObj3.cpp
  OversampProcessObj3.h
  OversampProcessObj4.cpp
  OversampProcessObj4.h
  OversampProcessObj.cpp
  OversampProcessObj.h
  PanoFftObj.cpp
  PanoFftObj.h
  PanogramCustomControl.cpp
  PanogramCustomControl.h
  PanogramCustomDrawer.cpp
  PanogramCustomDrawer.h
  PanogramFftObj.cpp
  PanogramFftObj.h
  PanogramGraphDrawer.cpp
  PanogramGraphDrawer.h
  PanogramPlayFftObj.cpp
  PanogramPlayFftObj.h
  PanoGraphDrawer.cpp
  PanoGraphDrawer.h
  ParamSmoother.cpp
  ParamSmoother.h
  PartialsDiffEstimate.cpp
  PartialsDiffEstimate.h
  PartialsToFreq2.cpp
  PartialsToFreq2.h
  PartialsToFreq3.cpp
  PartialsToFreq3.h
  PartialsToFreq4.cpp
  PartialsToFreq4.h
  PartialsToFreq5.cpp
  PartialsToFreq5.h
  PartialsToFreqCepstrum.cpp
  PartialsToFreqCepstrum.h
  PartialsToFreq.cpp
  PartialsToFreq.h
  PartialTracker2.cpp
  PartialTracker2.h
  PartialTracker3.cpp
  PartialTracker3.h
  PartialTracker4.cpp
  PartialTracker4.h
  PartialTracker5.cpp
  PartialTracker5.h
  PartialTracker.cpp
  PartialTracker.h
  PartialTWMEstimate2.cpp
  PartialTWMEstimate2.h
  PartialTWMEstimate3.cpp
  PartialTWMEstimate3.h
  PartialTWMEstimate.cpp
  PartialTWMEstimate.h
  PbFft.cpp
  PbFft.h
  PeakDetector2D2.cpp
  PeakDetector2D2.h
  PeakDetector2D.cpp
  PeakDetector2D.h
  PhaseCorrectObj.cpp
  PhaseCorrectObj.h
  PhasesDiff.cpp
  PhasesDiff.h
  PhasesUnwrapper.cpp
  PhasesUnwrapper.h
  PitchShiftFftObj3.cpp
  PitchShiftFftObj3.h
  PlaySelectPluginInterface.h
  PolarViz.cpp
  PolarViz.h
  portable_endian.h
  PostTransientFftObj3.cpp
  PostTransientFftObj3.h
  PPMFile.cpp
  PPMFile.h
  PseudoStereoObj2.cpp
  PseudoStereoObj2.h
  PseudoStereoObj.cpp
  PseudoStereoObj.h
  QuadTree.cpp
  QuadTree.h
  RayCaster2.cpp
  RayCaster2.h
  RCKdTree.cpp
  RCKdTree.h
  RCKdTreeVisitor.cpp
  RCKdTreeVisitor.h
  RCQuadTree.cpp
  RCQuadTree.h
  RCQuadTreeVisitor.cpp
  RCQuadTreeVisitor.h
  Rebalance_defs.h
  RebalanceDumpFftObj2.cpp
  RebalanceDumpFftObj2.h
  RebalanceDumpFftObj.cpp
  RebalanceDumpFftObj.h
  RebalanceMaskPredictorComp5.cpp
  RebalanceMaskPredictorComp5.h
  RebalanceMaskPredictorComp6.cpp
  RebalanceMaskPredictorComp6.h
  RebalanceMaskStack2.cpp
  RebalanceMaskStack2.h
  RebalanceMaskStack.cpp
  RebalanceMaskStack.h
  RebalanceProcessFftObjComp2.cpp
  RebalanceProcessFftObjComp2.h
  RebalanceProcessFftObjComp3.cpp
  RebalanceProcessFftObjComp3.h
  RebalanceProcessor.cpp
  RebalanceProcessor.h
  RebalanceTestMultiObj.cpp
  RebalanceTestMultiObj.h
  RELEASE-NOTES.txt
  Resampler2.cpp
  Resampler2.h
  Resampler.cpp
  Resampler.h
  ResampProcessObj.cpp
  ResampProcessObj.h
  ResizeGUIPluginInterface.cpp
  ResizeGUIPluginInterface.h
  SamplesDelayer.cpp
  SamplesDelayer.h
  SamplesProcessObj.h
  SamplesPyramid2.cpp
  SamplesPyramid2.h
  SamplesPyramid.cpp
  SamplesPyramid.h
  SamplesToSpectrogram.cpp
  SamplesToSpectrogram.h
  SASFrame2.cpp
  SASFrame2.h
  SASFrame3.cpp
  SASFrame3.h
  SASFrame.cpp
  SASFrame.h
  SASViewerPluginInterface.h
  SASViewerProcess2.cpp
  SASViewerProcess2.h
  SASViewerProcess.cpp
  SASViewerProcess.h
  SASViewerRender2.cpp
  SASViewerRender2.h
  SASViewerRender.cpp
  SASViewerRender.h
  SaturateOverObj.cpp
  SaturateOverObj.h
  Scale.cpp
  Scale.h
  ScopedNoDenormals2.h
  SecureRestarter.cpp
  SecureRestarter.h
  SGSmooth.cpp
  SGSmooth.h
  SimpleCompressor.cpp
  SimpleCompressor.h
  SimpleSpectrogramFftObj.cpp
  SimpleSpectrogramFftObj.h
  SineSynth2.cpp
  SineSynth2.h
  SineSynth.cpp
  SineSynth.h
  SineSynthSimple.cpp
  SineSynthSimple.h
  SinLUT2.h
  SinLUT.h
  SmoothAvgHistogram.cpp
  SmoothAvgHistogramDB.cpp
  SmoothAvgHistogramDB.h
  SmoothAvgHistogram.h
  SmoothCurveDB.cpp
  SmoothCurveDB.h
  Smoother.cpp
  Smoother.h
  SMVProcess4.cpp
  SMVProcess4.h
  SMVProcessColComputerChroma.cpp
  SMVProcessColComputerChroma.h
  SMVProcessColComputerFreq.cpp
  SMVProcessColComputerFreq.h
  SMVProcessColComputer.h
  SMVProcessColComputerMagn.cpp
  SMVProcessColComputerMagn.h
  SMVProcessColComputerMidSide.cpp
  SMVProcessColComputerMidSide.h
  SMVProcessColComputerPan.cpp
  SMVProcessColComputerPan.h
  SMVProcessColComputerPhasesFreq.cpp
  SMVProcessColComputerPhasesFreq.h
  SMVProcessColComputerPhasesTime.cpp
  SMVProcessColComputerPhasesTime.h
  SMVProcessXComputerChroma.cpp
  SMVProcessXComputerChroma.h
  SMVProcessXComputerDiff.cpp
  SMVProcessXComputerDiffFlat.cpp
  SMVProcessXComputerDiffFlat.h
  SMVProcessXComputerDiff.h
  SMVProcessXComputerFreq.cpp
  SMVProcessXComputerFreq.h
  SMVProcessXComputer.h
  SMVProcessXComputerLissajous.cpp
  SMVProcessXComputerLissajousEXP.cpp
  SMVProcessXComputerLissajousEXP.h
  SMVProcessXComputerLissajous.h
  SMVProcessXComputerScope.cpp
  SMVProcessXComputerScopeFlat.cpp
  SMVProcessXComputerScopeFlat.h
  SMVProcessXComputerScope.h
  SMVProcessYComputerChroma.cpp
  SMVProcessYComputerChroma.h
  SMVProcessYComputerFreq.cpp
  SMVProcessYComputerFreq.h
  SMVProcessYComputer.h
  SMVProcessYComputerMagn.cpp
  SMVProcessYComputerMagn.h
  SMVProcessYComputerPhasesFreq.cpp
  SMVProcessYComputerPhasesFreq.h
  SMVProcessYComputerPhasesTime.cpp
  SMVProcessYComputerPhasesTime.h
  SMVVolRender3.cpp
  SMVVolRender3.h
  SoftFft.cpp
  SoftFft.h
  SoftMasking2.cpp
  SoftMasking2.h
  SoftMaskingComp2.cpp
  SoftMaskingComp2.h
  SoftMaskingComp3.cpp
  SoftMaskingComp3.h
  SoftMaskingComp.cpp
  SoftMaskingComp.h
  SoftMasking.cpp
  SoftMasking.h
  SoftMaskingNComp.cpp
  SoftMaskingNComp.h
  SoftMaskingN.cpp
  SoftMaskingN.h
  SoundMetaViewerPluginInterface.h
  SourceComputer.cpp
  SourceComputer.h
  SourceLocalisationSystem2.cpp
  SourceLocalisationSystem2D.cpp
  SourceLocalisationSystem2D.h
  SourceLocalisationSystem2.h
  SourceLocalisationSystem3.cpp
  SourceLocalisationSystem3.h
  SourceLocalisationSystem.cpp
  SourceLocalisationSystem.h
  SourcePos.cpp
  SourcePos.h
  SparseVolRender.cpp
  SparseVolRender.h
  SpatializerConvolver.cpp
  SpatializerConvolver.h
  SpectralDiffObj.cpp
  SpectralDiffObj.h
  SpectroEditFftObj2.cpp
  SpectroEditFftObj2EXPE.cpp
  SpectroEditFftObj2EXPE.h
  SpectroEditFftObj2.h
  SpectroEditFftObj.cpp
  SpectroEditFftObj.h
  SpectroExpeFftObj.cpp
  SpectroExpeFftObj.h
  SpectrogramDisplay2.cpp
  SpectrogramDisplay2.h
  SpectrogramDisplay.cpp
  SpectrogramDisplay.h
  SpectrogramDisplayScroll2.cpp
  SpectrogramDisplayScroll2.h
  SpectrogramDisplayScroll3.cpp
  SpectrogramDisplayScroll3.h
  SpectrogramDisplayScroll.cpp
  SpectrogramDisplayScroll.h
  SpectrogramFftObj.cpp
  SpectrogramFftObjEXPE.cpp
  SpectrogramFftObjEXPE.h
  SpectrogramFftObj.h
  SpectrogramView.cpp
  SpectrogramView.h
  Spectrum.cpp
  Spectrum.h
  SpectrumViewFftObj.cpp
  SpectrumViewFftObj.h
  StereoDeReverbProcess.cpp
  StereoDeReverbProcess.h
  StereoPhasesProcess.cpp
  StereoPhasesProcess.h
  StereoVizVolRender2.cpp
  StereoVizVolRender2.h
  StereoVizVolRender.cpp
  StereoVizVolRender.h
  StereoWidenProcess.cpp
  StereoWidenProcess.h
  StereoWidthGraphDrawer2.cpp
  StereoWidthGraphDrawer2.h
  StereoWidthGraphDrawer3.cpp
  StereoWidthGraphDrawer3.h
  StereoWidthGraphDrawer.cpp
  StereoWidthGraphDrawer.h
  StereoWidthProcess2.cpp
  StereoWidthProcess2.h
  StereoWidthProcess3.cpp
  StereoWidthProcess3.h
  StereoWidthProcessDisp.cpp
  StereoWidthProcessDisp.h
  StlInsertSorted.h
  SynthPluginInterface.h
  TestFilter.cpp
  TestFilter.h
  TestSimd.cpp
  TestSimd.h
  TimeAxis3D.cpp
  TimeAxis3D.h
  TransientLib4.cpp
  TransientLib4.h
  TransientShaperFftObj3.cpp
  TransientShaperFftObj3.h
  UnstableRemoveIf.h
  UpTime.cpp
  UpTime.h
  USTBrillance.cpp
  USTBrillance.h
  USTCircleGraphDrawer.cpp
  USTCircleGraphDrawer.h
  USTClipper4.cpp
  USTClipper4.h
  USTClipperDisplay4.cpp
  USTClipperDisplay4.h
  USTCorrelationComputer2.cpp
  USTCorrelationComputer2.h
  USTCorrelationComputer3.cpp
  USTCorrelationComputer3.h
  USTCorrelationComputer4.cpp
  USTCorrelationComputer4.h
  USTCorrelationComputer.cpp
  USTCorrelationComputer.h
  USTCorrelationProcess.cpp
  USTCorrelationProcess.h
  USTDepthProcess3.cpp
  USTDepthProcess3.h
  USTDepthProcess4.cpp
  USTDepthProcess4.h
  USTDepthReverbTest.cpp
  USTDepthReverbTest.h
  USTEarlyReflect.cpp
  USTEarlyReflect.h
  USTFireworks.cpp
  USTFireworks.h
  USTLissajousGraphDrawer.cpp
  USTLissajousGraphDrawer.h
  USTMultiBandDisplay.cpp
  USTMultiBandDisplay.h
  USTPluginInterface.h
  USTPolarLevel.cpp
  USTPolarLevel.h
  USTProcess.cpp
  USTProcess.h
  USTPseudoStereoObj3.cpp
  USTPseudoStereoObj3.h
  USTStereoWidener.cpp
  USTStereoWidener.h
  USTUpmixGraphDrawer.cpp
  USTUpmixGraphDrawer.h
  USTVectorscope5.cpp
  USTVectorscope5.h
  USTVumeter.cpp
  USTVumeter.h
  USTVumeterProcess.cpp
  USTVumeterProcess.h
  USTWidthAdjuster5.cpp
  USTWidthAdjuster5.h
  USTWidthAdjuster6.cpp
  USTWidthAdjuster6.h
  USTWidthAdjuster7.cpp
  USTWidthAdjuster7.h
  USTWidthAdjuster8.cpp
  USTWidthAdjuster8.h
  USTWidthAdjuster9.cpp
  USTWidthAdjuster9.h
  View3DPluginInterface.h
  Watermark.cpp
  Watermark.h
  WavesProcess.cpp
  WavesProcess.h
  WavesRender.cpp
  WavesRender.h
  WavetableSynth.cpp
  WavetableSynth.h
  Window.cpp
  Window.h
  )
list(TRANSFORM _bluelab-lib_src PREPEND "${BLUELAB_LIB_SRC}")
iplug_target_add(_bluelab-lib INTERFACE
  INCLUDE ${BLUELAB_DEPS}/bluelab-lib
  SOURCE ${_bluelab-lib_src}
  #DEFINE "REAPER_PLUGIN"
  #LINK iPlug2_VST2
)

#include(${IPLUG2_DIR}/BL-Dependencies/KalmanFilter.cmake)
#iplug_target_add(_bluelab-lib PUBLIC INCLUDE _kalman_filtero LINK _kalman_filter)