#!/bin/bash

source "./tests-list.sh"
#source "./tests-list-mini.sh"
#source "./tests-list-dbg.sh"

for currentTest in $TESTS
do
    TEST_NAME="$(cut -d':' -f1 <<<$currentTest)"
    TEST_NUMBER="$(cut -d':' -f2 <<<$currentTest)"
    
    for bufSize in $BUFFER_SIZES
    do
       echo "Running test: $TEST_NAME $TEST_NUMBER $bufSize..."
       
       sudo /Applications/SikuliX/runsikulix -r Reaper-TEST.sikuli --args $TEST_NAME $TEST_NUMBER $bufSize
    done
done
