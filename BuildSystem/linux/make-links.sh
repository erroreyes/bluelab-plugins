#!/bin/bash

source "./plugins-list.sh"

PLUGS_PATH=../../BL-Plugins

for PLUG in $PLUGS
do
    PLUG_PATH=$PLUGS_PATH/$PLUG

    echo $PLUG_PATH

    ln -s $PLUG_PATH ../../iPlug2/Examples
    
    #sleep 1
done
