#define PLUG_NAME "BL-StereoWidth"
#define PLUG_MFR "BlueLab"
#define PLUG_VERSION_HEX 0x00060304
#define PLUG_VERSION_STR "6.3.4"
// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
#define PLUG_UNIQUE_ID 'rkjs'
#define PLUG_MFR_ID 'BlLa'
#define PLUG_URL_STR "http://www.bluelab-plugs.com"
#define PLUG_EMAIL_STR "contact@bluelab-plugs.com"
#define PLUG_COPYRIGHT_STR "Copyright 2021 BlueLab | Audio Plugins"
#define PLUG_CLASS_NAME StereoWidth

#define BUNDLE_NAME "BL-StereoWidth"
#define BUNDLE_MFR "BlueLab"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "BL-StereoWidth"

#define PLUG_CHANNEL_IO "1-2 2-2"

#define PLUG_LATENCY 0
#define PLUG_TYPE 0
#define PLUG_DOES_MIDI_IN 0
#define PLUG_DOES_MIDI_OUT 0
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 456
#define PLUG_HEIGHT 564
// Origin IPlug2: 60:
// Origin Rebalance: 25
// For Windows + VSynch
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY StereoWidth_Entry
#define AUV2_ENTRY_STR "StereoWidth_Entry"
#define AUV2_FACTORY StereoWidth_Factory
#define AUV2_VIEW_CLASS StereoWidth_View
#define AUV2_VIEW_CLASS_STR "StereoWidth_View"

#define AAX_TYPE_IDS 'IEF1', 'IEF2'
#define AAX_TYPE_IDS_AUDIOSUITE 'IEA1', 'IEA2'
#define AAX_PLUG_MFR_STR "BlLa"
#define AAX_PLUG_NAME_STR "StereoWidth\nStereoWidth"
/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "StereoWidth", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect"
instrument determined by PLUG _IS _INST
*/
#define AAX_PLUG_CATEGORY_STR "SoundField"
#define AAX_DOES_AUDIOSUITE 1

#define VST3_SUBCATEGORY "Fx|Spatial"

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

#define MANUAL_FN "BL-StereoWidth_manual-EN.pdf"
// For WIN32
#define MANUAL_FULLPATH_FN "manual\BL-StereoWidth_manual-EN.pdf"

#define DUMMY_RESOURCE_FN "dummy.txt"

// Image resource locations for this plug.
#define BACKGROUND_FN "background.png"
#define PLUGNAME_FN "plugname.png"
#define HELP_BUTTON_FN "help-button.png"
#define TEXTFIELD_FN "textfield.png"
#define CHECKBOX_FN "checkbox.png"

#define KNOB_FN "knob.svg"
#define KNOB_SMALL_FN "knob-small.svg"

#define GRAPH_FN "graph.png"

// Specific
#define CORRELATION_METER_FN "correlation-meter.png"
#define WIDTH_METER_FN "width-meter.png"

#define VECTORSCOPE_MODE_0_TOGGLE_BUTTON_FN "button-number-1.png"
#define VECTORSCOPE_MODE_1_TOGGLE_BUTTON_FN "button-number-2.png"
#define VECTORSCOPE_MODE_2_TOGGLE_BUTTON_FN "button-number-3.png"
#define VECTORSCOPE_MODE_3_TOGGLE_BUTTON_FN "button-number-4.png"
