#! /bin/sh

# rename the local installers and copy them to BuildSystem/Installers

source "./plugins-list.sh"
source "./plug-path.sh"
	     
for PLUG in $PLUG_LIST
do
    PLUG_PATH=$PLUGS_PATH/$PLUG

    echo $PLUG_PATH

    PLUG_NAME_NO_VERSION=$(echo $PLUG | awk -F'-' '{print $1}')
    
    INSTALLER_SRC_PATH="$PLUG_PATH/installer/BL-$PLUG_NAME_NO_VERSION-mac.dmg"
    INSTALLER_DST_PATH="./Installers/Trial/BL-$PLUG[TRIAL]-mac.dmg"

    cp $INSTALLER_SRC_PATH $INSTALLER_DST_PATH
done
