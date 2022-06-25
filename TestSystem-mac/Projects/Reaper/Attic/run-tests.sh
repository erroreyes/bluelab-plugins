#!/bin/bash

# use ':' as separator, otherwise the following loop will fail
AIR_TESTS="BL-Air:00 BL-Air:01 BL-Air:02 BL-Air:03 BL-Air:04 BL-Air:05 BL-Air:06 BL-Air:07"

AUTOGAIN_TESTS="BL-AutoGain:00 BL-AutoGain:01 BL-AutoGain:02 BL-AutoGain:03 BL-AutoGain:04 BL-AutoGain:05 BL-AutoGain:06 BL-AutoGain:07"

CHROMA_TESTS="BL-Chroma:00 BL-Chroma:01 BL-Chroma:02 BL-Chroma:03 BL-Chroma:04 BL-Chroma:05 BL-Chroma:06 BL-Chroma:07"

DENOISER_TESTS="BL-Denoiser:00 BL-Denoiser:01 BL-Denoiser:02 BL-Denoiser:03 BL-Denoiser:04 BL-Denoiser:05 BL-Denoiser:06 BL-Denoiser:07"

EQHACK_TESTS="BL-EQHack:00 BL-EQHack:01 BL-EQHack:02 BL-EQHack:03 BL-EQHack:04 BL-EQHack:05 BL-EQHack:06 BL-EQHack:07"

GAIN12_TESTS="BL-Gain12:00 BL-Gain12:01 BL-Gain12:02 BL-Gain12:03 BL-Gain12:04 BL-Gain12:05 BL-Gain12:06 BL-Gain12:07"

GAIN24_TESTS="BL-Gain24:00 BL-Gain24:01 BL-Gain24:02 BL-Gain24:03 BL-Gain24:04 BL-Gain24:05 BL-Gain24:06 BL-Gain24:07"

GAIN60_TESTS="BL-Gain60:00 BL-Gain60:01 BL-Gain60:02 BL-Gain60:03 BL-Gain60:04 BL-Gain60:05 BL-Gain60:06 BL-Gain60:07"


TESTS="$AIR_TESTS $AUTOGAIN_TESTS $CHROMA_TESTS $DENOISER_TESTS $EQHACK_TESTS $GAIN12_TESTS $GAIN24_TESTS $GAIN60_TESTS"


BUFFER_SIZES="64 1024 2048 447 999 3000"

for currentTest in $TESTS
do
    TEST_NAME="$(cut -d':' -f1 <<<$currentTest)"
    TEST_NUMBER="$(cut -d':' -f2 <<<$currentTest)"
    
    for bufSize in $BUFFER_SIZES
    do
       sudo /Applications/SikuliX/runsikulix -r Reaper-TEST.sikuli --args $TEST_NAME $TEST_NUMBER $bufSize
    done
done

#sudo /Applications/SikuliX/runsikulix -r Reaper-TEST.sikuli --args "BL-Gain12" "00" 512
