# bluelab-plugins
All BlueLab plugins and apps

All the necessary files to rebuild all the plugins, installers, modify the design and the documentation.

There are interesting plugins such as phantom fundamental effect, good quality stero widening, good quality pitch shifting, spectrogram edition, experimental volume rendering of sound, Deep Learning demixing, visualizers, partials tracking, experimental (not finished) synthetizer based on the SAS model (Strutured Additive Synthesis)...

## Building on Linux
- call bl-checkout-submodules.sh
- call bl-replace-nanovg.sh
- cd buildsystem/linux and call make-dist-linux.sh
(change plugins-list.sh to select only some plugins to build)
