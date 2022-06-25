#!/bin/sh

#export OMP_NUM_THREADS=4

/Users/applematuer/Documents/Apps/clang+llvm-4.0.1-x86_64-apple-macosx10.9.0/bin/clang -fopenmp rebalance.c main.c /Users/applematuer/Documents/Dev/plugin-development/Libs/darknet-master/src/*.c -I/Users/applematuer/Documents/Dev/plugin-development/Libs/darknet-master/include -I/Users/applematuer/Documents/Dev/plugin-development/Libs/darknet-master/src -O3 -o build/bl-darknet-omp
