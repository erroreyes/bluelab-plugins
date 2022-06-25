#!/bin/bash

###
# use ':' as separator, otherwise the following loop will fail
AIR_TESTS="BL-Air:00 BL-Air:01 BL-Air:02 BL-Air:03 BL-Air:04 BL-Air:05 BL-Air:06 BL-Air:07"

AUTOGAIN_TESTS="BL-AutoGain:00 BL-AutoGain:01 BL-AutoGain:02 BL-AutoGain:03 BL-AutoGain:04 BL-AutoGain:05 BL-AutoGain:06 BL-AutoGain:07"

CHROMA_TESTS="BL-Chroma:00 BL-Chroma:01 BL-Chroma:02 BL-Chroma:03 BL-Chroma:04 BL-Chroma:05 BL-Chroma:06 BL-Chroma:07"

DENOISER_TESTS="BL-Denoiser:00 BL-Denoiser:01 BL-Denoiser:02 BL-Denoiser:03 BL-Denoiser:04 BL-Denoiser:05 BL-Denoiser:06 BL-Denoiser:07"

EQHACK_TESTS="BL-EQHack:00 BL-EQHack:01 BL-EQHack:02 BL-EQHack:03 BL-EQHack:04 BL-EQHack:05 BL-EQHack:06 BL-EQHack:07"

GAIN12_TESTS="BL-Gain12:00 BL-Gain12:01 BL-Gain12:02 BL-Gain12:03 BL-Gain12:04 BL-Gain12:05 BL-Gain12:06 BL-Gain12:07"

GAIN24_TESTS="BL-Gain24:00 BL-Gain24:01 BL-Gain24:02 BL-Gain24:03 BL-Gain24:04 BL-Gain24:05 BL-Gain24:06 BL-Gain24:07"

GAIN60_TESTS="BL-Gain60:00 BL-Gain60:01 BL-Gain60:02 BL-Gain60:03 BL-Gain60:04 BL-Gain60:05 BL-Gain60:06 BL-Gain60:07"

GHOST_TESTS="BL-Ghost:00 BL-Ghost:01 BL-Ghost:02 BL-Ghost:03 BL-Ghost:04 BL-Ghost:05 BL-Ghost:06 BL-Ghost:07"

GHOSTVIEWER_TESTS="BL-GhostViewer:00 BL-GhostViewer:01 BL-GhostViewer:02 BL-GhostViewer:03 BL-GhostViewer:04 BL-GhostViewer:05 BL-GhostViewer:06 BL-GhostViewer:07"

IMPULSES_TESTS="BL-Impulses:00 BL-Impulses:01 BL-Impulses:02 BL-Impulses:03 BL-Impulses:04 BL-Impulses:05 BL-Impulses:06 BL-Impulses:07"

INFRA_TESTS="BL-Infra:00 BL-Infra:01 BL-Infra:02 BL-Infra:03 BL-Infra:04 BL-Infra:05 BL-Infra:06 BL-Infra:07"

INFRASONICVIEWER_TESTS="BL-InfrasonicViewer:00 BL-InfrasonicViewer:01 BL-InfrasonicViewer:02 BL-InfrasonicViewer:03 BL-InfrasonicViewer:04 BL-InfrasonicViewer:05 BL-InfrasonicViewer:06 BL-InfrasonicViewer:07"

LOFI_TESTS="BL-LoFi:00 BL-LoFi:01 BL-LoFi:02 BL-LoFi:03 BL-LoFi:04 BL-LoFi:05 BL-LoFi:06 BL-LoFi:07"

PANOGRAM_TESTS="BL-Panogram:00 BL-Panogram:01 BL-Panogram:02 BL-Panogram:03 BL-Panogram:04 BL-Panogram:05 BL-Panogram:06 BL-Panogram:07"

PITCHSHIFT_TESTS="BL-PitchShift:00 BL-PitchShift:01 BL-PitchShift:02 BL-PitchShift:03 BL-PitchShift:04 BL-PitchShift:05 BL-PitchShift:06 BL-PitchShift:07"

PRECEDENCE_TESTS="BL-Precedence:00 BL-Precedence:01 BL-Precedence:02 BL-Precedence:03 BL-Precedence:04 BL-Precedence:05 BL-Precedence:06 BL-Precedence:07"

REBALANCE_TESTS="BL-Rebalance:00 BL-Rebalance:01 BL-Rebalance:02 BL-Rebalance:03 BL-Rebalance:04 BL-Rebalance:05 BL-Rebalance:06 BL-Rebalance:07"

SAMPLEDELAY_TESTS="BL-SampleDelay:00 BL-SampleDelay:01 BL-SampleDelay:02 BL-SampleDelay:03 BL-SampleDelay:04 BL-SampleDelay:05 BL-SampleDelay:06 BL-SampleDelay:07"

SATURATE_TESTS="BL-Saturate:00 BL-Saturate:01 BL-Saturate:02 BL-Saturate:03 BL-Saturate:04 BL-Saturate:05 BL-Saturate:06 BL-Saturate:07"

SHAPER_TESTS="BL-Shaper:00 BL-Shaper:01 BL-Shaper:02 BL-Shaper:03 BL-Shaper:04 BL-Shaper:05 BL-Shaper:06 BL-Shaper:07"

SINE_TESTS="BL-Sine:00 BL-Sine:01 BL-Sine:02 BL-Sine:03 BL-Sine:04 BL-Sine:05 BL-Sine:06 BL-Sine:07"

SPATIALIZER_TESTS="BL-Spatializer:00 BL-Spatializer:01 BL-Spatializer:02 BL-Spatializer:03 BL-Spatializer:04 BL-Spatializer:05 BL-Spatializer:06 BL-Spatializer:07"

SPECTRALDIFF_TESTS="BL-SpectralDiff:00 BL-SpectralDiff:01 BL-SpectralDiff:02 BL-SpectralDiff:03 BL-SpectralDiff:04 BL-SpectralDiff:05 BL-SpectralDiff:06 BL-SpectralDiff:07"

STEREOPAN_TESTS="BL-StereoPan:00 BL-StereoPan:01 BL-StereoPan:02 BL-StereoPan:03 BL-StereoPan:04 BL-StereoPan:05 BL-StereoPan:06 BL-StereoPan:07"

STEREOWIDTH_TESTS="BL-StereoWidth:00 BL-StereoWidth:01 BL-StereoWidth:02 BL-StereoWidth:03 BL-StereoWidth:04 BL-StereoWidth:05 BL-StereoWidth:06 BL-StereoWidth:07"

STEREOWIDTH2_TESTS="BL-StereoWidth2:00 BL-StereoWidth2:01 BL-StereoWidth2:02 BL-StereoWidth2:03 BL-StereoWidth2:04 BL-StereoWidth2:05 BL-StereoWidth2:06 BL-StereoWidth2:07"

TRANSIENT_TESTS="BL-Transient:00 BL-Transient:01 BL-Transient:02 BL-Transient:03 BL-Transient:04 BL-Transient:05 BL-Transient:06 BL-Transient:07"

WAVES_TESTS="BL-Waves:00 BL-Waves:01 BL-Waves:02 BL-Waves:03 BL-Waves:04 BL-Waves:05 BL-Waves:06 BL-Waves:07"

WAV3S_TESTS="BL-Wav3s:00 BL-Wav3s:01 BL-Wav3s:02 BL-Wav3s:03 BL-Wav3s:04 BL-Wav3s:05 BL-Wav3s:06 BL-Wav3s:07"

STEREODEREVERB_TESTS="BL-StereoDeReverb:00 BL-StereoDeReverb:01 BL-StereoDeReverb:02 BL-StereoDeReverb:03 BL-StereoDeReverb:04 BL-StereoDeReverb:05"

###
TESTS=""
TESTS="$TESTS $AIR_TESTS"
TESTS="$TESTS $AUTOGAIN_TESTS"
TESTS="$TESTS $CHROMA_TESTS" 
TESTS="$TESTS $DENOISER_TESTS"
TESTS="$TESTS $EQHACK_TESTS"
TESTS="$TESTS $GAIN12_TESTS"
TESTS="$TESTS $GAIN24_TESTS"
TESTS="$TESTS $GAIN60_TESTS"
TESTS="$TESTS $GHOST_TESTS"
TESTS="$TESTS $GHOSTVIEWER_TESTS"
TESTS="$TESTS $IMPULSES_TESTS"
TESTS="$TESTS $INFRA_TESTS"
TESTS="$TESTS $INFRASONICVIEWER_TESTS"
TESTS="$TESTS $LOFI_TESTS"
TESTS="$TESTS $PANOGRAM_TESTS"
TESTS="$TESTS $PITCHSHIFT_TESTS"
TESTS="$TESTS $PRECEDENCE_TESTS"
TESTS="$TESTS $REBALANCE_TESTS"
TESTS="$TESTS $SAMPLEDELAY_TESTS"
TESTS="$TESTS $SATURATE_TESTS"
TESTS="$TESTS $SHAPER_TESTS"
TESTS="$TESTS $SINE_TESTS"
TESTS="$TESTS $SPATIALIZER_TESTS"
TESTS="$TESTS $SPECTRALDIFF_TESTS"
TESTS="$TESTS $STEREOPAN_TESTS"
TESTS="$TESTS $STEREOWIDTH_TESTS"
#TESTS="$TESTS $STEREOWIDTH2_TESTS"
#TESTS="$TESTS $TRANSIENT_TESTS"
#TESTS="$TESTS $WAVES_TESTS"
TESTS="$TESTS $WAV3S_TESTS"

#
#TESTS="$TESTS BL-Air:08 BL-AutoGain:08 BL-Denoiser:08 BL-Ghost:08 BL-Impulses:08 BL-PitchShift:08 BL-Rebalance:08 BL-Shaper:08 BL-Spatializer:08 BL-StereoWidth:08 BL-Transient:08"

#TESTS="$TESTS BL-Air:09 BL-AutoGain:09 BL-Denoiser:09 BL-Ghost:09 BL-Impulses:09 BL-PitchShift:09 BL-Rebalance:09 BL-Shaper:09 BL-Spatializer:09 BL-StereoWidth:09 BL-Transient:09"

#TESTS="$TESTS BL-Air:10 BL-AutoGain:10 BL-Denoiser:10 BL-Ghost:10 BL-Impulses:10 BL-PitchShift:10 BL-Rebalance:10 BL-Shaper:10 BL-Spatializer:10 BL-StereoWidth:10 BL-Transient:10"

#TESTS="$SPATIALIZER_TESTS"

###
#BUFFER_SIZES="64 1024 2048 447 999 3000"

#BUFFER_SIZES="64 1024 3000" # passes
#BUFFER_SIZES="64 1024"

#BUFFER_SIZES="447"

#TESTS="BL-Air:02 BL-AutoGain:02 BL-Chroma:02 BL-Denoiser:02 BL-EQHack:02 BL-Gain12:02 BL-Gain24:02 BL-Gain60:02 BL-Ghost:02 BL-GhostViewer:02 BL-Impulses:02 BL-Infra:02 BL-InfrasonicViewer:02 BL-LoFi:02 BL-Panogram:02 BL-PitchShift:02 BL-Precedence:02 BL-Rebalance:02 BL-SampleDelay:02 BL-Saturate:02 BL-Shaper:02 BL-Sine:02 BL-Spatializer:02 BL-SpectralDiff:02 BL-StereoPan:02 BL-StereoWidth:02 BL-Wav3s:02"

#BUFFER_SIZES="64 1024 2048 447 999 3000" # last test

#TESTS="BL-Saturate:02"
#TESTS="BL-PitchShift:02"

#TESTS="BL-Air:02"
#BUFFER_SIZES="512"

#TESTS="BL-StereoWidth2:02"
#BUFFER_SIZES="64 1024 2048 447 999 3000"

#TESTS=$STEREOWIDTH2_TESTS
#BUFFER_SIZES="512"

#BUFFER_SIZES="64 1024 2048 447 999 3000"
#TESTS=$DENOISER_TESTS

#BUFFER_SIZES="64 1024 2048 447 999 3000"
#TESTS=$STEREODEREVERB_TESTS

#BUFFER_SIZES="64 1024 2048 447 999 3000"
#TESTS=$DENOISER_TESTS

BUFFER_SIZES="64 1024 2048 447 999 3000"
TESTS=$REBALANCE_TESTS
