#!/bin/sh

SOURCE_NUM="0"

#cp ./backup/rebalance"$SOURCE_NUM"_final.weights .
#./dk-build/bl-darknet rebalance test cfg/source0.cfg cfg/rebalance.cfg rebalance"$SOURCE_NUM"_final.weights

#cp ./backup/rebalance"$SOURCE_NUM".backup .
#./dk-build/bl-darknet rebalance test cfg/source0.cfg cfg/rebalance.cfg rebalance"$SOURCE_NUM".backup

#cp ./backup/rebalance.backup .
./dk-build/bl-darknet rebalance test cfg/sources.cfg cfg/unet-darknet.cfg rebalance.backup
