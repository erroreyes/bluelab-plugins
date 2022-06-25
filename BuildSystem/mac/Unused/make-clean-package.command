#! /bin/sh

#remove the package generated, in the build directory

source "./plugins-list.sh"
source "./plug-path.sh"

for plug in $PLUG_LIST
do
    PLUG_PATH=$PLUGS_PATH/$plug

    cd $PLUG_PATH

    echo $PLUG_PATH
    
    source ./make-clean-package.command
done

