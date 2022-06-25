#!/bin/sh

#./dk-build/bl-darknet rebalance train cfg/source0.cfg cfg/model.cfg 
#./dk-build/bl-darknet rebalance train cfg/t1.test.cfg cfg/model.cfg 

#./dk-build/bl-darknet-omp rebalance train cfg/source0.cfg cfg/rebalance.cfg

#./dk-build/bl-darknet rebalance train cfg/source0.cfg cfg/rebalance.cfg
./dk-build/bl-darknet rebalance train cfg/sources.cfg cfg/rebalance-test-simple.cfg


#say "finished"
