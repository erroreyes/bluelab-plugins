#! /bin/sh

# build in debug mode (don't build installers)

source "./plugins-list.sh"
source "./plug-path.sh"

for plug in $PLUG_LIST
do
    PLUG_PATH=$PLUGS_PATH/$plug

    cd $PLUG_PATH

    echo $PLUG_PATH

    source ./make-all-debug-mac.command

    sleep 1 #avoid XCode crashes
done
