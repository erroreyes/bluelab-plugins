#! /bin/sh

source "./plugins-list.sh"

for PLUG in $PLUG_LIST
do
    PLUG_NAME_NO_VERSION=$(echo $PLUG | awk -F'-' '{print $1}')

    PLUG_AAX=/Library/Application\ Support/Avid/Audio/Plug-Ins/BL-$PLUG_NAME_NO_VERSION.aaxplugin

    echo "PLUG: " BL-$PLUG_NAME_NO_VERSION.aaxplugin
    
    /Applications/PACEAntiPiracy/Eden/Fusion/Current/bin/wraptool verify --in "$PLUG_AAX"
done
