// Necessary to move this include from IPlug_include_in_plug_hdr.h to here
// This is necessary for Windows compilation, because there is another config.h file
// for externals, and Visual Studio includes the wrong one
#include "config.h"

// 19: ok for 10s*48000Hz*2
//#define FFT_MAXBITLEN 19
//#define NIKO_FFT_LEN_HACK 1

// BlueLab
#define BL_GUI_TYPE_FLOAT 0
#define BL_TYPE_FLOAT 0
#define BL_FIX_FLT_DENORMAL_OBJ 0 //1
