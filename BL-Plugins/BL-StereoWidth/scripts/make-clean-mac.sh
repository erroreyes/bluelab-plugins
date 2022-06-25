#! /bin/sh

BASEDIR=$(dirname $0)
cd $BASEDIR/..

#---------------------------------------------------------------------------------------------------------

PLUGIN_NAME=`echo | grep BUNDLE_NAME bl_config.h`
PLUGIN_NAME=${PLUGIN_NAME//\#define BUNDLE_NAME }
PLUGIN_NAME=${PLUGIN_NAME//\"}

# work out the paths to the binaries

VST2=`echo | grep VST_PATH ../../iPlug2/common-mac.xcconfig`
VST2=${VST2//\VST_PATH = }/$PLUGIN_NAME.vst

VST3=`echo | grep VST3_PATH ../../iPlug2/common-mac.xcconfig`
VST3=${VST3//\VST3_PATH = }/$PLUGIN_NAME.vst3

AU=`echo | grep AU_PATH ../../iPlug2/common-mac.xcconfig`
AU=${AU//\AU_PATH = }/$PLUGIN_NAME.component

APP=`echo | grep APP_PATH ../../iPlug2/common-mac.xcconfig`
APP=${APP//\APP_PATH = }/$PLUGIN_NAME.app

# Dev build folder
AAX=`echo | grep AAX_PATH ../../iPlug2/common-mac.xcconfig`
AAX=${AAX//\AAX_PATH = }/$PLUGIN_NAME.aaxplugin
#AAX_FINAL=$AAX

if [ -d $AU ] 
then
    sudo rm -f -R $AU
fi

if [ -d $VST2 ] 
then
    sudo rm -f -R $VST2
fi

if [ -d $VST3 ] 
then
    sudo rm -f -R $VST3
fi

if [ -d "${AAX}" ] 
then
    sudo rm -f -R "${AAX}"
fi

#if [ -d "${AAX_FINAL}" ] 
#then
#  sudo rm -f -R "${AAX_FINAL}"
#fi


# clean from xcode project. Change target to build individual formats 
xcodebuild -project ./projects/$PLUGIN_NAME-macOS.xcodeproj -xcconfig ./config/$PLUGIN_NAME-mac.xcconfig -target "All-BL" -configuration Debug clean 2> ./build-mac.log

xcodebuild -project ./projects/$PLUGIN_NAME-macOS.xcodeproj -xcconfig ./config/$PLUGIN_NAME-mac.xcconfig -target "All-BL" -configuration Release clean 2> ./build-mac.log


if [ -s build-mac.log ]
then
  echo "build failed due to following errors:"
  echo ""
  cat build-mac.log
  exit 1
else
 rm build-mac.log
fi

rm -rf build-mac
rm -rf installer/build-mac
rm -rf installer/*.dmg
rm -rf installer/*.zip

echo "done"
