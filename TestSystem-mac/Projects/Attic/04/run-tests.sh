#!/bin/bash

source "./tests-list.sh"
#source "./tests-list-mini.sh"
#source "./tests-list-dbg.sh"

TESTS="$AIR_TESTS $AUTOGAIN_TESTS $CHROMA_TESTS $DENOISER_TESTS $EQHACK_TESTS $GAIN12_TESTS $GAIN24_TESTS $GAIN60_TESTS $GHOST_TESTS $GHOSTVIEWER_TESTS $IMPULSES_TESTS $LOFI_TESTS $PITCHSHIFT_TESTS $PRECEDENCE_TESTS $REBALANCE_TESTS $SAMPLEDELAY_TESTS $SATURATE_TESTS $SHAPER_TESTS $SINE_TESTS $SPATIALIZER_TESTS $SPECTRALDIFF_TESTS $STEREOPAN_TESTS $STEREOWIDTH_TESTS $TRANSIENT_TESTS $WAVES_TESTS"

for currentTest in $TESTS
do
    TEST_NAME="$(cut -d':' -f1 <<<$currentTest)"
    TEST_NUMBER="$(cut -d':' -f2 <<<$currentTest)"

    #TEST_BOUNCE_DELAY="$(cut -d':' -f3 <<<$currentTest)"
    
    for bufSize in $BUFFER_SIZES
    do
       echo "Running test: $TEST_NAME $TEST_NUMBER $bufSize..."
       
       sudo /Applications/SikuliX/runsikulix -r Reaper-TEST.sikuli --args $TEST_NAME $TEST_NUMBER $bufSize #$TEST_BOUNCE_DELAY
    done
done
