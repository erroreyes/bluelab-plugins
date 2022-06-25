rem Sometimes a project fails when post buid step copy/xcopy
rem This is random, and stops
rem so split the build to finish to compile without re-processing the plugs from which we already have the installers

rem Full list
rem set PLUG_LIST=(AutoGain-v5.0.1 Denoiser-v5.0 EQHack-v5.0.1 Gain12-v5.0 Gain24-v5.0 Gain60-v5.0 Impulses-v5.0 PitchShift-v5.0 SampleDelay-v5.0 Saturate-v5.0 Shaper-v5.0 Spatializer-v5.0.1 SpectralDiff-v5.0.1 StereoPan-v5.0 StereoWidth-v5.0 Transient-v5.0)

rem set PLUG_LIST=(Shaper-v5.0 Spatializer-v5.0.1 SpectralDiff-v5.0.1 StereoPan-v5.0 StereoWidth-v5.0 Transient-v5.0)

rem Remaining list
rem set PLUG_LIST=(StereoPan-v5.0 StereoWidth-v5.0 Transient-v5.0)

rem Plugs not to build
rem Zarlino-v5.0
rem Spectrogram-v5.0

rem set PLUG_LIST=(AutoGain-v5.0.2 Denoiser-v5.0.1 EQHack-v5.0.2 Gain12-v5.0.1 Gain24-v5.0.1 Gain60-v5.0.1 Impulses-v5.0.1 PitchShift-v5.0.1 SampleDelay-v5.0.1 Saturate-v5.0.1 Shaper-v5.0.1 Spatializer-v5.0.2 SpectralDiff-v5.0.2 StereoPan-v5.0.1 StereoWidth-v5.0.1 Transient-v5.0.1)

rem set PLUG_LIST=(Ghost-v5.8.7)
rem set PLUG_LIST=(Impulses-v5.1.0)

rem set PLUG_LIST=(LoFi-v5.1.0)

rem set PLUG_LIST=(AutoGain-v5.0.3)

rem set PLUG_LIST=(StereoWidth-v5.2.2 Sine-v5.0.0)

rem set PLUG_LIST=(Rebalance-v5.5.2)

rem set PLUG_LIST=(Ghost-v5.8.8)
rem set PLUG_LIST=(Saturate-v5.0.2 Shaper-v5.0.2 Sine-v5.0.1 Spatializer-v5.0.3 SpectralDiff-v5.0.3 StereoPan-v5.0.2 StereoWidth-v5.2.3 Transient-v5.0.2 StereoViz-5.3.7)

rem set PLUG_LIST=(StereoViz-5.3.7) 

rem set PLUG_LIST=(SampleDelay-v5.0.2 Saturate-v5.0.2 Shaper-v5.0.2 Sine-v5.0.1 Spatializer-v5.0.3 SpectralDiff-v5.0.3 StereoPan-v5.0.2 StereoWidth-v5.2.3 Transient-v5.0.2)

rem set PLUG_LIST=(SpectralDiff-v5.0.3)

rem set PLUG_LIST=(Impulses-v5.1.1)


rem set PLUG_LIST=(EQHack-v5.0.3)

rem set PLUG_LIST=(Ghost-v5.8.8)

rem set PLUG_LIST=(AutoGain-v5.0.5 Denoiser-v5.0.3 EQHack-v5.0.3 Gain12-v5.0.2 Gain24-v5.0.2 Gain60-v5.0.2 Ghost-v5.8.8 Impulses-v5.1.1 LoFi-v5.1.1 PitchShift-v5.0.2 Rebalance-v5.5.3 SampleDelay-v5.0.2 Saturate-v5.0.2 Shaper-v5.0.2 Sine-v5.0.1 Spatializer-v5.0.3 SpectralDiff-v5.0.3 StereoPan-v5.0.2 StereoWidth-v5.2.3 Transient-v5.0.2)

rem set PLUG_LIST=(SampleDelay-v5.0.2 Saturate-v5.0.2 Shaper-v5.0.2 Sine-v5.0.1 Spatializer-v5.0.3 SpectralDiff-v5.0.3 StereoPan-v5.0.2 StereoWidth-v5.2.3 Transient-v5.0.2)

rem set PLUG_LIST=(Ghost-v5.8.8 Impulses-v5.1.1 Saturate-v5.0.2 Shaper-v5.0.2)

rem set PLUG_LIST=(Waves-v5.3.0)

rem set PLUG_LIST=(Waves-v5.4.1)

rem set PLUG_LIST=(EQHack-v5.0.5 SpectralDiff-v5.0.5 AutoGain-v5.0.8 Denoiser-v5.0.5)

rem set PLUG_LIST=(Waves-v5.4.2)

rem set PLUG_LIST=(Ghost-v5.8.9)
 
rem set PLUG_LIST=(Precedence-v5.0.4 SampleDelay-v5.0.3)

rem set PLUG_LIST=(Sine-v5.0.2)

rem set PLUG_LIST=(Air-v5.3.4 AutoGain-v5.1.1 Chroma-v5.0.6 Denoiser-v5.0.6 EQHack-v5.0.6 Gain12-v5.0.3 Gain24-v5.0.3 Gain60-v5.0.3 Ghost-v5.9.0 GhostViewer-v5.0.4 Impulses-v5.1.3 LoFi-v5.1.2 PitchShift-v5.0.3 Rebalance-v5.5.4 SampleDelay-v5.0.5 Saturate-v5.0.3 Shaper-v5.0.4 Sine-v5.0.3 Spatializer-v5.0.8 SpectralDiff-v5.0.6 StereoPan-v5.0.3 StereoWidth-v5.2.4 Transient-v5.0.3 Waves-v5.4.3)

rem set PLUG_LIST=(Air-v5.3.4 Chroma-v5.0.6 EQHack-v5.0.6 GhostViewer-v5.0.4 Precedence-v5.0.5 Waves-v5.4.3)

rem set PLUG_LIST=(Air-v5.3.4 Waves-v5.4.3)

rem set PLUG_LIST=(Air-v5.3.4)

rem set PLUG_LIST=(Precedence-v5.0.6 SampleDelay-v5.0.6)

rem ---

rem set PLUG_LIST=(Air-v5.3.4 AutoGain-v5.1.1 Chroma-v5.0.6 Denoiser-v5.0.6 EQHack-v5.0.6 Gain12-v5.0.3 Gain24-v5.0.3 Gain60-v5.0.3 Ghost-v5.9.0 GhostViewer-v5.0.4)

rem set PLUG_LIST=(Impulses-v5.1.3 LoFi-v5.1.2 PitchShift-v5.0.3 Precedence-v5.0.6 Rebalance-v5.5.4 SampleDelay-v5.0.6 Saturate-v5.0.3 Shaper-v5.0.4)

rem set PLUG_LIST=(Sine-v5.0.3 Spatializer-v5.0.8 SpectralDiff-v5.0.6 StereoPan-v5.0.3 StereoWidth-v5.2.4 Transient-v5.0.3 Waves-v5.4.3)

rem ---

rem set PLUG_LIST=(Chroma-v5.0.6 Ghost-v5.9.0 GhostViewer-v5.0.4)

rem set PLUG_LIST=(AutoGain-v5.1.1)

rem set PLUG_LIST=(Chroma-v5.0.6 Denoiser-v5.0.6 EQHack-v5.0.6 Ghost-v5.9.0 GhostViewer-v5.0.4 Impulses-v5.1.3 LoFi-v5.1.2 Shaper-v5.0.4)

rem set PLUG_LIST=(SpectralDiff-v5.0.6 StereoWidth-v5.2.4 Transient-v5.0.3 Waves-v5.4.3)

rem set PLUG_LIST=(Chroma-v5.0.6)

rem set PLUG_LIST=(Ghost-v5.9.0 GhostViewer-v5.0.4 Waves-v5.4.3)

rem set PLUG_LIST=(AutoGain-v5.1.1)


rem -------
rem set PLUG_LIST=(Air-v5.3.4 AutoGain-v5.1.1 Chroma-v5.0.6 Denoiser-v5.0.6 EQHack-v5.0.6 Gain12-v5.0.3)

rem set PLUG_LIST=(Gain24-v5.0.3 Gain60-v5.0.3 Ghost-v5.9.0 GhostViewer-v5.0.4 Impulses-v5.1.3 LoFi-v5.1.2 PitchShift-v5.0.3)

rem set PLUG_LIST=(Precedence-v5.0.6 Rebalance-v5.5.4 SampleDelay-v5.0.6 Saturate-v5.0.3 Shaper-v5.0.4 Sine-v5.0.3)

rem set PLUG_LIST=(Spatializer-v5.0.8 SpectralDiff-v5.0.6 StereoPan-v5.0.3 StereoWidth-v5.2.4 Transient-v5.0.3 Waves-v5.4.3)

rem set PLUG_LIST=(AutoGain-v5.1.1 Chroma-v5.0.6 Denoiser-v5.0.6 EQHack-v5.0.6 Ghost-v5.9.0 GhostViewer-v5.0.4 Impulses-v5.1.3 LoFi-v5.1.2)
rem set PLUG_LIST=(Shaper-v5.0.4 SpectralDiff-v5.0.6 StereoWidth-v5.2.4 Transient-v5.0.3 Waves-v5.4.3)


rem set PLUG_LIST=(AutoGain-v5.1.1)

rem set PLUG_LIST=(Chroma-v5.0.6 Ghost-v5.9.0 GhostViewer-v5.0.4 Waves-v5.4.3)reù

rem set PLUG_LIST=(Denoiser-v5.0.6)

rem set PLUG_LIST=(Ghost-v5.9.0)

rem Trials
rem set PLUG_LIST=(Air-v5.3.4 AutoGain-v5.1.1 Chroma-v5.0.6 Denoiser-v5.0.6 EQHack-v5.0.6 GhostViewer-v5.0.4 Impulses-v5.1.3)

rem set PLUG_LIST=(LoFi-v5.1.2 PitchShift-v5.0.3 Precedence-v5.0.6 Rebalance-v5.5.4 Shaper-v5.0.4 Spatializer-v5.0.8)

rem set PLUG_LIST=(SpectralDiff-v5.0.6 StereoWidth-v5.2.4 Transient-v5.0.3)

rem set PLUG_LIST=(Impulses-v5.1.3)

rem set PLUG_LIST=(Infra-v5.0.1)

set PLUG_LIST=(Infra-v5.0.3)
