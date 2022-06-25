#! /bin/sh

BASEDIR=$(dirname $0)

cd $BASEDIR/..

PLUGIN_NAME=`echo | grep BUNDLE_NAME config.h`
PLUGIN_NAME=${PLUGIN_NAME//\#define BUNDLE_NAME }
PLUGIN_NAME=${PLUGIN_NAME//\"}

AAX=`echo | grep AAX_PATH ../../iPlug2/common-mac.xcconfig`
AAX=${AAX//\AAX_PATH = }/$PLUGIN_NAME.aaxplugin

echo "Plug: " $PLUGIN_NAME.aaxplugin
    
/Applications/PACEAntiPiracy/Eden/Fusion/Current/bin/wraptool verify --in "$AAX"
