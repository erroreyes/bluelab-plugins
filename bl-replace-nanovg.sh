#!/bin/sh

# Set custom NanoVG inside iPlug2
if [ ! -d "nanovg" ]; then
    git clone https://github.com/deadlab-plugins/nanovg.git
    cd nanovg
    git pull origin ndbl-branch
    git checkout ndbl-branch
    cd ..
    mv iPlug2/Dependencies/IGraphics/NanoVG iPlug2/Dependencies/IGraphics/NanoVG-old
    cp -R nanovg iPlug2/Dependencies/IGraphics/NanoVG
fi
