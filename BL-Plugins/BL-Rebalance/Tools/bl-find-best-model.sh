#!/bin/sh

# Reaper command line options
# https://github.com/ReaTeam/Doc/blob/master/REAPER%20command%20line%20parameters.md

BASE_PATH=/home/niko/Documents/BlueLabAudio-Debug/BL-FindBestModel
PLUG_MODEL_RESOURCE=/home/niko/.vst/BL-Rebalance.vst2/resources/rebalance1.weights
#PLUG_MODEL_RESOURCE=/home/niko/.vst/BL-Rebalance.vst2/resources/rebalance0.weights

REAPER_APP=/home/niko/Documents/Apps/reaper_linux_x86_64/REAPER/reaper
REAPER_PROJECT_DIR=/home/niko/Documents/Dev/plugs-dev/Projects/Rebalance-Reaper
REAPER_FILE=$REAPER_PROJECT_DIR/TOOL-Find-Best-Model-VST2.RPP
REAPER_RENDER_FILE=$REAPER_PROJECT_DIR/Bounces/bl-find-best-model-render.wav

SPECTRAL_DIFF_APP=/home/niko/Documents/Dev/plugs-dev/bluelab/BL-Plugins/BL-SpectralDiff/build-linux/BL-SpectralDiff-app/BL-SpectralDiff

REFERENCE_WAVE_FILE=$BASE_PATH/reference/bl-find-best-model-reference.wav

SCORE_FILE=$BASE_PATH/score.txt

WEIGHT_FILES=$(find $BASE_PATH/models -name 'rebalance_*.weights' | sort -t _ -k 2 -g)

I=0
BEST_SCORE=100.0
BEST_I=0
BEST_WEIGHT_FILE=""

rm $SCORE_FILE

for WEIGHT_FILE in $WEIGHT_FILES
do
    echo "Processing file: $WEIGHT_FILE"

    cp $WEIGHT_FILE $PLUG_MODEL_RESOURCE

    $REAPER_APP -nosplash -renderproject $REAPER_FILE

    SHORT_WEIGHT_FILE=$(basename $WEIGHT_FILE)

    RENDER_WAVE_FILE=$BASE_PATH/renders/$SHORT_WEIGHT_FILE"_"$((I)).wav
    
    mv $REAPER_RENDER_FILE $RENDER_WAVE_FILE

    SCORE=$($SPECTRAL_DIFF_APP --brief-display $REFERENCE_WAVE_FILE $RENDER_WAVE_FILE)

    echo "$SCORE " >> $SCORE_FILE

    if [ $(echo "$SCORE < $BEST_SCORE" | bc -l) -eq 1 ]
    then
	BEST_SCORE=$SCORE
	BEST_I=$I
	BEST_WEIGHT_FILE=$SHORT_WEIGHT_FILE 
    fi

    I=$((I+1))

    sleep 5
done

echo "Best: $BEST_I - $BEST_WEIGHT_FILE - $BEST_SCORE %"
