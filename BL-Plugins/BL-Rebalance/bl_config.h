// Necessary to move this include from IPlug_include_in_plug_hdr.h to here
// This is necessary for Windows compilation, because there is another config.h file
// for externals, and Visual Studio includes the wrong one
#include "config.h"

// BlueLab
#define BL_GUI_TYPE_FLOAT 0
#define BL_TYPE_FLOAT 0
#define BL_FIX_FLT_DENORMAL_OBJ 0 //1

#ifndef WIN32
#define DARKNET_USE_BLAS_GEMM 1 // 0
#else
#ifdef _WIN64
// For x64, use OpenBLAS compiled with clang/Ninja
// (for better performances, and otherwise there would be unsolvable linking errors)
#define DARKNET_USE_BLAS_GEMM 1
#else
// For Win32, just don't use OpenBLAS
// (unsolvable linking errors)
#define DARKNET_USE_BLAS_GEMM 0
#endif

#endif
