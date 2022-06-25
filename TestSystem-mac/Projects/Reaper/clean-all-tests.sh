#!/bin/bash

PROJECTS=$(ls ./Projects)

for proj in $PROJECTS
do
    PWD0=$PWD

    cd ./Projects
    cd $proj

    source ./clean-test.sh
    
    cd $PWD0
done
    

