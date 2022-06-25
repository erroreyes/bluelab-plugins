#! /bin/sh

# avoid stopping after any error
#set +e

source "./plugins-list.sh"

source "./plug-path.sh"

for plug in $PLUGS
do
    PLUG_PATH=$PLUGS_PATH/$plug

    cd $PLUG_PATH/scripts

    echo $PLUG_PATH

    source ./makedist-mac.sh

    sleep 1 #avoid XCode crashes
done
