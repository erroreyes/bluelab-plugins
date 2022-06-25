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

set PLUG_LIST=(AutoGain-v5.0.2 Denoiser-v5.0.1 EQHack-v5.0.2 Gain12-v5.0.1 Gain24-v5.0.1 Gain60-v5.0.1 Impulses-v5.0.1 PitchShift-v5.0.1 SampleDelay-v5.0.1 Saturate-v5.0.1 Shaper-v5.0.1 Spatializer-v5.0.2 SpectralDiff-v5.0.2 StereoPan-v5.0.1 StereoWidth-v5.0.1 Transient-v5.0.1)
rem set PLUG_LIST=(Transient-v5.0.1)
