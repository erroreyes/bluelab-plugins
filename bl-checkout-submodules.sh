#!/bin/sh

# bluelabpxx@ssh.cluster026.hosting.ovh.net:bluelab.git

# git submodules init
# git submodules update

git pull --recurse-submodules

cd ..
cd iPlug2
git remote rm origin
git remote add origin https://github.com/deadlab-plugins/iPlug2.git 
git pull origin bluelab
git checkout bluelab
git pull origin bluelab

cd ../Libs
cd CrossoverFilter
git checkout master
git pull origin master

cd ..
cd darknet
git checkout bluelab
git pull origin bluelab

cd ..
cd Decimator
git checkout master
git pull origin master

cd ..
cd fastapprox
git checkout master
git pull origin master

cd ..
cd fast-math
git checkout master
git pull origin master

cd ..
cd flac
git checkout bluelab
git pull origin bluelab

cd ..
cd fmem
git checkout bluelab
git pull origin bluelab

cd ..
cd freeverb
git checkout master
git pull origin master

cd ..
cd glm
git checkout bluelab
git pull origin bluelab

cd ..
cd ini_parser
git checkout master
git pull origin master

cd ..
cd KalmanFilter
git checkout master
git pull origin master

cd ..
cd libdct
git checkout master
git pull origin master

cd ..
cd libebur128
git checkout bluelab
git pull origin bluelab

cd ..
cd libsamplerate/
git checkout master
git pull origin master

cd ..
cd libsndfile
git checkout master
git pull origin master

cd ..
cd OpenBLAS
git checkout bluelab
git pull origin bluelab

cd ..
cd PhaseVocoder-DSP
git checkout master
git pull origin master

cd ..
cd r8brain
git checkout master
git pull origin master

cd ..
cd RandomSequence
git checkout bluelab
git pull origin bluelab

cd ..
cd RTConvolve
git checkout bluelab
git pull origin bluelab

cd ..
cd smbPitchShift
git checkout master
git pull origin master

cd ..
cd sndfilter
git checkout bluelab
git pull origin bluelab

cd ..
cd ..
