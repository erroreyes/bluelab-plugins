#!/bin/sh

# predict the from the last computed weights

cp ./backup/rebalance-test-simple.backup ./rebalance.backup
./dk-build/bl-darknet rebalance test cfg/sources.cfg cfg/rebalance-test-simple.cfg rebalance.backup
