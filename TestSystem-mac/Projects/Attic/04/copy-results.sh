#!/bin/bash

PROJECTS=$(ls ./Projects)

for proj in $PROJECTS
do
    #cp ./Projects/$proj/Result/Bounces/*.wav ./Results
    cp ./Projects/$proj/Result/Screenshots/*.png ./Results
    cp ./Projects/$proj/Result/Spectrograms/*.png ./Results
done
    

