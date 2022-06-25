#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: make-base.sh <dir>"
    exit
fi

DIR=$1

cd $DIR

#
cd Bounces
BOUNCES="$(ls)"

for bounce in $BOUNCES
do
    FILENAME="$(cut -d'.' -f1 <<<$bounce)"
    FILENAME2="$(cut -d'.' -f2 <<<$bounce)"
    mv $bounce $FILENAME.$FILENAME2-base.wav
done

cd ..

#
cd Screenshots
SNAPS="$(ls)"

for snap in $SNAPS
do
    FILENAME="$(cut -d'.' -f1 <<<$snap)"
    FILENAME2="$(cut -d'.' -f2 <<<$snap)"
    mv $snap $FILENAME.$FILENAME2-base.png
done

cd ..

#
cd Spectrograms
SPECTROS="$(ls)"

for spectro in $SPECTROS
do
    FILENAME="$(cut -d'.' -f1 <<<$spectro)"
    FILENAME2="$(cut -d'.' -f2 <<<$spectro)"
    mv $spectro $FILENAME.$FILENAME2-base.png
done

cd ..

cd ..
