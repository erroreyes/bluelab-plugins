#!/bin/sh

for i in $(seq 0 100)
do
    export BL_TRACK_NUM=$i
    ./build-linux/BL-RebalanceStereo-app/BL-RebalanceStereo

    sleep 20
done
