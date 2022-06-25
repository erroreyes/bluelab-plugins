#!/bin/sh

for i in $(seq 0 100)
do
    export BL_TRACK_NUM=$i
    ./build-linux/BL-Rebalance-app/BL-Rebalance

    sleep 20
done
