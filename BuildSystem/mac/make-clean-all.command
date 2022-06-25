#! /bin/sh

# remove debug, release builds, and local installers

rm -rf /Users/applematuer/Documents/Dev/plugin-development/Latest-wdl-ol/wdl-ol/PCH

source "./plugins-list.sh"
source "./plug-path.sh"

for plug in $PLUG_LIST
do
    PLUG_PATH=$PLUGS_PATH/$plug

    cd $PLUG_PATH

    echo $PLUG_PATH

    source ./make-clean-install.command
    source ./make-clean-all-mac.command

    sleep 1
done
