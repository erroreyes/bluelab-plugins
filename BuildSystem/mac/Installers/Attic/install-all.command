#! /bin/sh

# install the local installers

source "./plugins-list.sh"
source "./plug-path.sh"

for PLUG in $PLUG_LIST
do
    DMG=./Installers/BL-$PLUG-mac.dmg

    sudo hdiutil attach $DMG

    PLUG_STRIP=$(echo $PLUG | head -n1 | cut -d "-" -f1)
    
    sudo installer -store -pkg "/Volumes/BL-$PLUG_STRIP/BL-$PLUG_STRIP Installer.pkg"

    sudo hdiutil detach /Volumes/BL-$PLUG_STRIP
done
