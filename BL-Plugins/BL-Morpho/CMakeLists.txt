cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
cmake_policy(SET CMP0091 NEW)

#########
# This is a build file for iPlug2 on Linux.
# It may work on Windows and MacOS, but that isn't the initial goal.
#
# To setup the build (change CMAKE_BUILD_TYPE as desired):
#   cmake -S . -B build-linux -DCMAKE_BUILD_TYPE=Debug
# To build the VST2 version:
#   cmake --build build-linux --target IPlugControls-vst2
# To build the VST3 version:
#   cmake --build build-linux --target IPlugControls-vst3

project(BL-Morpho VERSION 1.0.0 LANGUAGES C CXX)

set(IPLUG2_DIR ${CMAKE_SOURCE_DIR}/../..)
include(${IPLUG2_DIR}/iPlug2.cmake)
find_package(iPlug2 REQUIRED COMPONENTS APP VST2 VST3 AU)

#bluelab
include(${IPLUG2_DIR}/BL-Dependencies/bl_all_libs.cmake)
  
set(dir "${CMAKE_SOURCE_DIR}")
set(SRC_FILES
  "${dir}/config.h"
  "${dir}/bl_config.h"
  "${dir}/Morpho.h"
  "${dir}/Morpho.cpp"
  "${dir}/Morpho_defs.h"
  "${dir}/WaterfallSource.h"
  "${dir}/WaterfallSource.cpp"
  "${dir}/SoSource.h"
  "${dir}/SoSource.cpp"
  "${dir}/SoLiveSource.h"
  "${dir}/SoLiveSource.cpp"
  "${dir}/SoFileSource.h"
  "${dir}/SoFileSource.cpp"
  "${dir}/SoSourceImpl.h"
  "${dir}/SoSourceImpl.cpp"
  "${dir}/SoSourceManager.h"
  "${dir}/SoSourceManager.cpp"
  "${dir}/SoSourcesView.h"
  "${dir}/SoSourcesView.cpp"
  "${dir}/SySource.h"
  "${dir}/SySource.cpp"
  "${dir}/SyMixSource.h"
  "${dir}/SyMixSource.cpp"
  "${dir}/SyLiveSource.h"
  "${dir}/SyLiveSource.cpp"
  "${dir}/SyFileSource.h"
  "${dir}/SyFileSource.cpp"
  "${dir}/SySourceImpl.h"
  "${dir}/SySourceImpl.cpp"
  "${dir}/SySourceManager.h"
  "${dir}/SySourceManager.cpp"
  "${dir}/SySourcesView.h"
  "${dir}/SySourcesView.cpp"
  "${dir}/MorphoObj.h"
  "${dir}/MorphoObj.cpp"
  "${dir}/GhostTrack2.h"
  "${dir}/GhostTrack2.cpp"
  "${dir}/GhostCustomDrawer2.h"
  "${dir}/GhostCustomDrawer2.cpp"
  "${dir}/GhostCustomControl2.h"
  "${dir}/GhostCustomControl2.cpp"
  "${dir}/GhostSamplesToSpectro2.h"
  "${dir}/GhostSamplesToSpectro2.cpp"
  "${dir}/BorderCustomDrawer.h"
  "${dir}/BorderCustomDrawer.cpp"
  "${dir}/MorphoSoPipeline.h"
  "${dir}/MorphoSoPipeline.cpp"
  "${dir}/MorphoSyPipeline.h"
  "${dir}/MorphoSyPipeline.cpp"
  "${dir}/MorphoFrameAnalyzer.h"
  "${dir}/MorphoFrameAnalyzer.cpp"
  "${dir}/MorphoFrameAnalyzerFftObj.h"
  "${dir}/MorphoFrameAnalyzerFftObj.cpp"
  "${dir}/MorphoFrameAna2.h"
  "${dir}/MorphoFrameAna2.cpp"
  "${dir}/MorphoFrameSynthetizer.h"
  "${dir}/MorphoFrameSynthetizer.cpp"
  "${dir}/MorphoFrameSynthetizerFftObj.h"
  "${dir}/MorphoFrameSynthetizerFftObj.cpp"
  "${dir}/MorphoFrameSynth2.h"
  "${dir}/MorphoFrameSynth2.cpp"
  "${dir}/MorphoWaterfallRender.h"
  "${dir}/MorphoWaterfallRender.cpp"
  "${dir}/MorphoWaterfallView.h"
  "${dir}/MorphoWaterfallView.cpp"
  "${dir}/MorphoWaterfallGUI.h"
  "${dir}/MorphoWaterfallGUI.cpp"
  "${dir}/PartialTracker8.h"
  "${dir}/PartialTracker8.cpp"
  "${dir}/PartialFilter2.h"
  "${dir}/PartialFilterAMFM2.h"
  "${dir}/PartialFilterAMFM2.cpp"
  "${dir}/PartialsToFreq8.h"
  "${dir}/PartialsToFreq8.cpp"
  "${dir}/Partial2.h"
  "${dir}/Partial2.cpp"
  "${dir}/MorphoFrame7.cpp"
  "${dir}/MorphoFrame7.h"
  "${dir}/CurveView.h"
  "${dir}/CurveView.cpp"
  "${dir}/CurveViewAmp.h"
  "${dir}/CurveViewAmp.cpp"
  "${dir}/CurveViewPitch.h"
  "${dir}/CurveViewPitch.cpp"
  "${dir}/CurveViewPartials.h"
  "${dir}/CurveViewPartials.cpp"
  "${dir}/ViewTitle.h"
  "${dir}/ViewTitle.cpp"
  "${dir}/MorphoMixer.h"
  "${dir}/MorphoMixer.cpp"
)
source_group(TREE ${dir} FILES ${SRC_FILES})
set(RES_FILES
  "${dir}/resources/fonts/Roboto-Regular.ttf"

  "${dir}/resources/fonts/font-regular.ttf"
  "${dir}/resources/fonts/font-light.ttf"
  "${dir}/resources/fonts/font-bold.ttf"

  "${dir}/resources/fonts/OpenSans-ExtraBold.ttf"
  "${dir}/resources/fonts/Roboto-Bold.ttf"

  "${dir}/resources/fonts/OpenSans-Bold.ttf"
  
  "${dir}/resources/dummy.txt"
  
  "${dir}/resources/img/plugname.png"
  "${dir}/resources/img/help-button.png"
  "${dir}/resources/img/textfield.png"
  "${dir}/resources/img/checkbox.png"
  "${dir}/resources/img/knob-small.svg"

  # Common
  "${dir}/resources/img/button-sources.png"
  "${dir}/resources/img/button-synthesis.png"

  # Source panel
  "${dir}/resources/img/background_so.png"

  "${dir}/resources/img/graph-spectro-so.png"
  "${dir}/resources/img/graph-waterfall-so.png"

  "${dir}/resources/img/button-live-source.png"
  "${dir}/resources/img/button-file-source.png"
  "${dir}/resources/img/button-apply.png"

  "${dir}/resources/img/button-play.png"

  "${dir}/resources/img/button-seldir-both.png"
  "${dir}/resources/img/button-seldir-horiz.png"
  "${dir}/resources/img/button-seldir-vert.png"

  # Synthesis panel
  "${dir}/resources/img/background_sy.png"

  "${dir}/resources/img/graph-waterfall-sy.png"

  "${dir}/resources/img/button-mute.png"
  "${dir}/resources/img/button-solo.png"

  "${dir}/resources/img/label-source-name.png"
  "${dir}/resources/img/label-source-icons.png"
  
  "${dir}/resources/img/pad-track-sy.png"
  "${dir}/resources/img/pad-handle0.png"
  "${dir}/resources/img/pad-handle1.png"
  "${dir}/resources/img/pad-handle2.png"
  "${dir}/resources/img/pad-handle3.png"
  "${dir}/resources/img/pad-handle4.png"
  "${dir}/resources/img/pad-handle5.png"
)
source_group(TREE "${dir}/resources" PREFIX Resources FILES ${RES_FILES})

set(MANUAL_FILE
  "${dir}/manual/BL-Morpho_manual-EN.pdf"
)
source_group(TREE ${dir} PREFIX Manual FILES ${MANUAL_FILE})

# While not required, creating a base interface for includes and settings seems like a good idea.
add_library(_base INTERFACE)
# iplug_target_add() is a shorthand function for adding sources and include dirs,
# linking libraries, adding resources, setting compile options, etc.
iplug_target_add(_base INTERFACE
  INCLUDE ${dir} ${dir}/resources
  LINK iPlug2_Synth iPlug2_NANOVG iPlug2_GL2)

# For whatever reason setting CXX_STANDARD doesn't seem to work properly, so set it explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(_base INTERFACE "$<$<COMPILE_LANGUAGE:CXX>:/std:c++14>")
else()
  target_compile_options(_base INTERFACE "$<$<COMPILE_LANGUAGE:CXX>:-std=c++14>")
endif()

add_executable(App WIN32 MACOSX_BUNDLE ${SRC_FILES} ${RES_FILES} ${MANUAL_FILE})
iplug_target_add(App PUBLIC LINK _base RESOURCE ${RES_FILES} ${MANUAL_FILE})

#bluelab
bl_add_all_libs(App)

# You MUST call iplug_configure_target(<target_name> <app|vst2|vst3|...>) for things to build correctly.
iplug_configure_target(App app)

add_library(VST2 MODULE ${SRC_FILES} ${RES_FILES} ${MANUAL_FILE})
iplug_target_add(VST2 PUBLIC LINK _base RESOURCE ${RES_FILES} ${MANUAL_FILE})
#bluelab
bl_add_all_libs(VST2)
iplug_configure_target(VST2 vst2)

add_library(VST3 MODULE ${SRC_FILES} ${RES_FILES} ${MANUAL_FILE})
iplug_target_add(VST3 PUBLIC LINK _base RESOURCE ${RES_FILES} ${MANUAL_FILE})
#bluelab
bl_add_all_libs(VST3)
iplug_configure_target(VST3 vst3)

if (APPLE)
  add_library(AUv2 MODULE ${SRC_FILES} ${RES_FILES} ${MANUAL_FILE})
  iplug_target_add(AUv2 PUBLIC LINK _base RESOURCE ${RES_FILES} ${MANUAL_FILE})
  iplug_configure_target(AUv2 au2)
endif()

