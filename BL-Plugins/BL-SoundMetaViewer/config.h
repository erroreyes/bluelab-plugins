#define PLUG_NAME "BL-SoundMetaViewer"
#define PLUG_MFR "BlueLab"
#define PLUG_VERSION_HEX 0x00060101
#define PLUG_VERSION_STR "6.1.1"
// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
#define PLUG_UNIQUE_ID 'seyj'
#define PLUG_MFR_ID 'BlLa'
#define PLUG_URL_STR "http://www.bluelab-plugs.com"
#define PLUG_EMAIL_STR "contact@bluelab-plugs.com"
#define PLUG_COPYRIGHT_STR "Copyright 2021 BlueLab | Audio Plugins"
#define PLUG_CLASS_NAME SoundMetaViewer

#define BUNDLE_NAME "BL-SoundMetaViewer"
#define BUNDLE_MFR "BlueLab"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "BL-SoundMetaViewer"

#define PLUG_CHANNEL_IO "1-2 2-2"

#define PLUG_LATENCY 0
#define PLUG_TYPE 0
#define PLUG_DOES_MIDI_IN 0
#define PLUG_DOES_MIDI_OUT 0
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 1050
#define PLUG_HEIGHT 724
// Origin IPlug2: 60:
// Origin Rebalance: 25
// For Windows + VSynch
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY SoundMetaViewer_Entry
#define AUV2_ENTRY_STR "SoundMetaViewer_Entry"
#define AUV2_FACTORY SoundMetaViewer_Factory
#define AUV2_VIEW_CLASS SoundMetaViewer_View
#define AUV2_VIEW_CLASS_STR "SoundMetaViewer_View"

#define AAX_TYPE_IDS 'IEF1', 'IEF2'
#define AAX_TYPE_IDS_AUDIOSUITE 'IEA1', 'IEA2'
#define AAX_PLUG_MFR_STR "BlLa"
#define AAX_PLUG_NAME_STR "SoundMetaViewer\SoundMetaViewer"
/* AAX_PLUG_CATEGORY_STR can be "None", "EQ", "Dynamics", "SoundMetaViewer", "Reverb", "Delay", "Modulation", "Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
*/
#define AAX_PLUG_CATEGORY_STR "Analyzer"
#define AAX_DOES_AUDIOSUITE 1

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|SoundMetaViewer", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
#define VST3_SUBCATEGORY "Fx|Analyzer"

#define APP_NUM_CHANNELS 2
#define APP_N_VECTOR_WAIT 0
#define APP_MULT 1
#define APP_COPY_AUV3 0
#define APP_SIGNAL_VECTOR_SIZE 64

// Necessary!
#define ROBOTO_FN "Roboto-Regular.ttf"

#define FONT_REGULAR_FN "font-regular.ttf"
#define FONT_LIGHT_FN "font-light.ttf"
#define FONT_BOLD_FN "font-bold.ttf"

#define FONT_OPENSANS_EXTRA_BOLD_FN "OpenSans-ExtraBold.ttf"
#define FONT_ROBOTO_BOLD_FN "Roboto-Bold.ttf"

#define MANUAL_FN "BL-SoundMetaViewer_manual-EN.pdf"
// For WIN32
#define MANUAL_FULLPATH_FN "manual\BL-SoundMetaViewer_manual-EN.pdf"

#define DUMMY_RESOURCE_FN "dummy.txt"

// Image resource locations for this plug.
#define BACKGROUND_FN "background.png"
#define PLUGNAME_FN "plugname.png"
#define LOGO_FN "logo-anim.png"

#define HELP_BUTTON_FN "help-button.png"

#define TEXTFIELD_FN "textfield.png"
#define KNOB_FN "knob.png"
#define KNOB_SMALL_FN "knob-small.png"

#define ROLLOVER_BUTTON_FN "rollover-button.png"
#define RADIOBUTTON_FN "radiobutton.png"
#define CHECKBOX_FN "checkbox.png"

#define GRAPH_FN "graph.png"

// GUI resize
#define BUTTON_RESIZE_SMALL_FN "button-small-gui.png"
#define BUTTON_RESIZE_MEDIUM_FN "button-med-gui.png"
#define BUTTON_RESIZE_BIG_FN "button-big-gui.png"
#define BUTTON_RESIZE_PORTRAIT_FN "button-portrait-gui.png"

// Specific
// Play button
#define PLAY_BUTTON_FN "play-button.png"
