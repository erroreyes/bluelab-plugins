#!/bin/bash

PROJECTS=$(ls ./Projects)

for proj in $PROJECTS
do
    cp ./Projects/$proj/Result/Bounces/*.wav ./Results/Bounces
    cp ./Projects/$proj/Result/Screenshots/*.png ./Results/Screenshots
    cp ./Projects/$proj/Result/Spectrograms/*.png ./Results/Spectrograms
done
    

