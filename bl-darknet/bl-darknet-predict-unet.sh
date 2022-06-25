#!/bin/sh

# predict the from the last computed weights

cp ./backup/bl-unet.backup ./rebalance.backup
./dk-build/bl-darknet rebalance test cfg/sources.cfg cfg/bl-unet.cfg rebalance.backup
