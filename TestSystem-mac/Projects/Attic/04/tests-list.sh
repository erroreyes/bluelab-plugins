#!/bin/bash

###
# use ':' as separator, otherwise the following loop will fail
AIR_TESTS="" #BL-Air:00 BL-Air:01 BL-Air:02 BL-Air:03 BL-Air:04 BL-Air:05 BL-Air:06 BL-Air:07"

AUTOGAIN_TESTS="" #BL-AutoGain:00 BL-AutoGain:01 BL-AutoGain:02 BL-AutoGain:03 BL-AutoGain:04 BL-AutoGain:05 BL-AutoGain:06 BL-AutoGain:07"

CHROMA_TESTS="" #BL-Chroma:00 BL-Chroma:01 BL-Chroma:02 BL-Chroma:03 BL-Chroma:04 BL-Chroma:05 BL-Chroma:06 BL-Chroma:07"

DENOISER_TESTS="" #BL-Denoiser:00:10 BL-Denoiser:01:10 BL-Denoiser:02:10 BL-Denoiser:03:10 BL-Denoiser:04:10 BL-Denoiser:05:10 BL-Denoiser:06:10 BL-Denoiser:07:10"

EQHACK_TESTS="" #BL-EQHack:00 BL-EQHack:01 BL-EQHack:02 BL-EQHack:03 BL-EQHack:04 BL-EQHack:05 BL-EQHack:06 BL-EQHack:07"

GAIN12_TESTS="" #BL-Gain12:00 BL-Gain12:01 BL-Gain12:02 BL-Gain12:03 BL-Gain12:04 BL-Gain12:05 BL-Gain12:06 BL-Gain12:07"

GAIN24_TESTS="" #BL-Gain24:00 BL-Gain24:01 BL-Gain24:02 BL-Gain24:03 BL-Gain24:04 BL-Gain24:05 BL-Gain24:06 BL-Gain24:07"

GAIN60_TESTS="" #BL-Gain60:00 BL-Gain60:01 BL-Gain60:02 BL-Gain60:03 BL-Gain60:04 BL-Gain60:05 BL-Gain60:06 BL-Gain60:07"

GHOST_TESTS="" #BL-Ghost:00 BL-Ghost:01 BL-Ghost:02 BL-Ghost:03 BL-Ghost:04 BL-Ghost:05 BL-Ghost:06 BL-Ghost:07"

GHOSTVIEWER_TESTS="" #BL-GhostViewer:00 BL-GhostViewer:01 BL-GhostViewer:02 BL-GhostViewer:03 BL-GhostViewer:04 BL-GhostViewer:05 BL-GhostViewer:06 BL-GhostViewer:07"

IMPULSES_TESTS="" #BL-Impulses:00 BL-Impulses:01 BL-Impulses:02 BL-Impulses:03 BL-Impulses:04 BL-Impulses:05 BL-Impulses:06 BL-Impulses:07"

LOFI_TESTS="" #BL-LoFi:00 BL-LoFi:01 BL-LoFi:02 BL-LoFi:03 BL-LoFi:04 BL-LoFi:05 BL-LoFi:06 BL-LoFi:07"

PITCHSHIFT_TESTS="" #BL-PitchShift:00 BL-PitchShift:01 BL-PitchShift:02 BL-PitchShift:03 BL-PitchShift:04 BL-PitchShift:05 BL-PitchShift:06 BL-PitchShift:07"

PRECEDENCE_TESTS="" #BL-Precedence:00 BL-Precedence:01 BL-Precedence:02 BL-Precedence:03 BL-Precedence:04 BL-Precedence:05 BL-Precedence:06 BL-Precedence:07"

REBALANCE_TESTS="" #BL-Rebalance:00 BL-Rebalance:01 BL-Rebalance:02 BL-Rebalance:03 BL-Rebalance:04 BL-Rebalance:05 BL-Rebalance:06 BL-Rebalance:07"

SAMPLEDELAY_TESTS="" #BL-SampleDelay:00 BL-SampleDelay:01 BL-SampleDelay:02 BL-SampleDelay:03 BL-SampleDelay:04 BL-SampleDelay:05 BL-SampleDelay:06 BL-SampleDelay:07"

SATURATE_TESTS="" #BL-Saturate:00 BL-Saturate:01 BL-Saturate:02 BL-Saturate:03 BL-Saturate:04 BL-Saturate:05 BL-Saturate:06 BL-Saturate:07"

SHAPER_TESTS="" #BL-Shaper:00 BL-Shaper:01 BL-Shaper:02 BL-Shaper:03 BL-Shaper:04 BL-Shaper:05 BL-Shaper:06 BL-Shaper:07"

SINE_TESTS="" #BL-Sine:00 BL-Sine:01 BL-Sine:02 BL-Sine:03 BL-Sine:04 BL-Sine:05 BL-Sine:06 BL-Sine:07"

SPATIALIZER_TESTS="BL-Spatializer:00:10 BL-Spatializer:01:10 BL-Spatializer:02:10 BL-Spatializer:03:25 BL-Spatializer:04:25 BL-Spatializer:05:25 BL-Spatializer:06:10 BL-Spatializer:07:10"

SPECTRALDIFF_TESTS="" #BL-SpectralDiff:00 BL-SpectralDiff:01 BL-SpectralDiff:02 BL-SpectralDiff:03 BL-SpectralDiff:04 BL-SpectralDiff:05 BL-SpectralDiff:06 BL-SpectralDiff:07"

STEREOPAN_TESTS="BL-StereoPan:00 BL-StereoPan:01 BL-StereoPan:02 BL-StereoPan:03 BL-StereoPan:04 BL-StereoPan:05 BL-StereoPan:06 BL-StereoPan:07"

STEREOWIDTH_TESTS="BL-StereoWidth:00 BL-StereoWidth:01 BL-StereoWidth:02 BL-StereoWidth:03 BL-StereoWidth:04 BL-StereoWidth:05 BL-StereoWidth:06 BL-StereoWidth:07"

TRANSIENT_TESTS="BL-Transient:00 BL-Transient:01 BL-Transient:02 BL-Transient:03 BL-Transient:04 BL-Transient:05 BL-Transient:06 BL-Transient:07"

WAVES_TESTS="BL-Waves:00 BL-Waves:01 BL-Waves:02 BL-Waves:03 BL-Waves:04 BL-Waves:05 BL-Waves:06 BL-Waves:07"

###
#BUFFER_SIZES="64 1024 2048 447 999 3000"
BUFFER_SIZES="1024"
