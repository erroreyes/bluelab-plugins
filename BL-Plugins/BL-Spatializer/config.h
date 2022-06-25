#define PLUG_NAME "BL-Spatializer"
#define PLUG_MFR "BlueLab"
#define PLUG_VERSION_HEX 0x00060304
#define PLUG_VERSION_STR "6.3.4"
// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'z0de'
#define PLUG_MFR_ID 'BlLa'
#define PLUG_URL_STR "http://www.bluelab-plugs.com"
#define PLUG_EMAIL_STR "contact@bluelab-plugs.com"
#define PLUG_COPYRIGHT_STR "Copyright 2021 BlueLab | Audio Plugins"
#define PLUG_CLASS_NAME Spatializer

#define BUNDLE_NAME "BL-Spatializer"
#define BUNDLE_MFR "BlueLab"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "BL-Spatializer"

#define PLUG_CHANNEL_IO "1-2 2-2"

#define PLUG_LATENCY 1024
#define PLUG_TYPE 0
#define PLUG_DOES_MIDI_IN 0
#define PLUG_DOES_MIDI_OUT 0
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 454
#define PLUG_HEIGHT 456
// Origin IPlug2: 60:
// Origin Spatializer: 25
// For Windows + VSynch
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY Spatializer_Entry
#define AUV2_ENTRY_STR "Spatializer_Entry"
#define AUV2_FACTORY Spatializer_Factory
#define AUV2_VIEW_CLASS Spatializer_View
#define AUV2_VIEW_CLASS_STR "Spatializer_View"

#define AAX_TYPE_IDS 'IEF1', 'IEF2'
#define AAX_TYPE_IDS_AUDIOSUITE 'IEA1', 'IEA2'
#define AAX_PLUG_MFR_STR "BlLa"
#define AAX_PLUG_NAME_STR "Spatializer\nSpatializer"
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

#define MANUAL_FN "BL-Spatializer_manual-EN.pdf"
// For WIN32
#define MANUAL_FULLPATH_FN "manual\BL-Spatializer_manual-EN.pdf"

#define DUMMY_RESOURCE_FN "dummy.txt"

// Image resource locations for this plug.
#define BACKGROUND_FN "background.png"
#define PLUGNAME_FN "plugname.png"
#define HELP_BUTTON_FN "help-button.png"
#define TEXTFIELD_FN "textfield.png"

#define KNOB_SMALL_FN "knob-small.svg"

// Specific
#define HEAD_AZIMUTH_FN "head-azimuth.png"
#define HEAD_ELEVATION_FN "head-elevation.png"

#define SPATIALIZER_HANDLE_FN "spatializer-handle.png"

#define XYPAD_TRACK_FN "xypad-track.png"
#define XYPAD_HANDLE_FN "xypad-handle.png"
#define XYPAD_BORDER_SIZE 2.0

