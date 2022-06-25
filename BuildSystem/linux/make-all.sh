#!/bin/bash

source "./plugins-list.sh"

BUILD_SYS_DIR=$(pwd)

#export CMAKE_BUILD_PARALLEL_LEVEL=3
export CMAKE_BUILD_PARALLEL_LEVEL=1
#export CMAKE_BUILD_PARALLEL_LEVEL=2

for PLUG in $PLUGS
do
    PLUG_PATH=$PLUGS_PATH/$PLUG

    echo $PLUG

    cd ../../iPlug2/Examples/$PLUG
    
    #make -k vst2 vst3 app BUILD_CONFIG=Debug
    #make -k vst2 vst3 app BUILD_CONFIG=Release
    make -k vst2 BUILD_CONFIG=Debug
    
    #sleep 30

    cd $BUILD_SYS_DIR
done
