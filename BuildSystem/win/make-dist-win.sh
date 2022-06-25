#!/bin/bash

source "./plugins-list.sh"

PLUGS_PATH=/c/Documents/Dev/plugs-dev/bluelab/BL-Plugins

BUILD_SYS_DIR=$(pwd)

for PLUG in $PLUGS
do
    PLUG_PATH=$PLUGS_PATH/$PLUG

    echo $PLUG

    cd $PLUG_PATH/scripts

    ./makedist-win.bat 0
    
    #sleep 30

    cd $BUILD_SYS_DIR
done
