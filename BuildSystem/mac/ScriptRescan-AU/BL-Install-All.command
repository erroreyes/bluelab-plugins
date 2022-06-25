# Install all the plugins contained in a directory
# while disabling temporarily "SecAssessment system policy security"
#
# Double-click on Install-All.command to install all the plugins
# in just one time
#
# A Terminal window will open, then type your administrator password
#
# The default plugin installation path will be used
#

#! /bin/sh

# spctl
SPCTL_STATUS_STR=$(spctl --status)
SPCTL_STATUS=0

if [[ $SPCTL_STATUS_STR == *"enabled"* ]]; then
  SPCTL_STATUS=1
fi

if [ $SPCTL_STATUS ]; then
   sudo spctl --master-disable
fi

# directory containing BL-Install-All.command"
cd  `dirname $0`

PLUG_LIST='*.dmg'

PLUG_LIST_STRIP=""

for PLUG in $PLUG_LIST
do
  PLUG_STRIP=$(echo $PLUG | head -n1 | cut -d "-" -f2 -f3)

  PLUG_LIST_STRIP=$PLUG_LIST_STRIP" "$PLUG_STRIP
done

PLUG_LIST=$PLUG_LIST_STRIP

for PLUG in $PLUG_LIST
do
    DMG=./BL-$PLUG-mac.dmg

    sudo hdiutil attach $DMG

    PLUG_STRIP=$(echo $PLUG | head -n1 | cut -d "-" -f1)
    
    sudo installer -store -pkg "/Volumes/BL-$PLUG_STRIP/BL-$PLUG_STRIP Installer.pkg" -target /

    sudo hdiutil detach /Volumes/BL-$PLUG_STRIP
done

# spctl
if [ $SPCTL_STATUS ]; then
   sudo spctl --master-enable
fi
