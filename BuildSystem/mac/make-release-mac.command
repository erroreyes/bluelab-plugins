#! /bin/sh

# build in release mode (don't build installers)

source "./plugins-list.sh"
source "./plug-path.sh"

for plug in $PLUG_LIST
do
    PLUG_PATH=$PLUGS_PATH/$plug

    cd $PLUG_PATH

    echo $PLUG_PATH

    source ./make-all-mac.command

    sleep 1 #avoid XCode crashes
done
