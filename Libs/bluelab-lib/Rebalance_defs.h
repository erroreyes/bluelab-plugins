#ifndef REBALANCE_DEFS
#define REBALANCE_DEFS

//
//  Rebalance_defs.h
//  BL-Rebalance
//
//  Created by applematuer on 5/17/20.
//
//

// Soft masks
#define USE_SOFTMASK_CHECKBOX 0 //1
#define SOFT_MASK_HISTO_SIZE 8

// Do not predict other, but keep the remaining
#define OTHER_IS_REST 0 //1 //0

// FIX: when using 192000Hz, with buffer size 64,
// the spectrogram was scaled horizontally progressively
//
// This was due to accumulated "rounding" error on the input buffer size
//
// GOOD: must be kept to 1
#define FIX_ADJUST_IN_RESAMPLING 1

// GOOD: must be kepts to 1 too
#define FIX_ADJUST_OUT_RESAMPLING 1

// Listen to what the network is listening
#define DEBUG_LISTEN_BUFFER 0

#define FREQ_RES 1

#define KEEP_SYNTHESIS_ENERGY 0

// When set to 0, we are accurate for frequencies
#define VARIABLE_HANNING 0

//#endif

// Reshape correctly to convert from 4 spectrogram colf of 356 freqs
// to a square of 32x32
#define RESHAPE_32_X_32 0 //1

#define DBG_PPM_COLOR_COEFF 64.0 //1.0

// Models
//#define MODEL_4X "rebalance-4x"
//#define MODEL_8X "rebalance-8x"
//#define MODEL_16X "rebalance-16x"
//#define MODEL_32X "rebalance-32x"

#define MODEL_NAME "rebalance"

#define USE_DEBUG_RADIO_BUTTONS 0 //1

#define NUM_STEM_SOURCES 4

#define REBALANCE_TARGET_SAMPLE_RATE 11025.0
#define REBALANCE_BUFFER_SIZE 2048
#define REBALANCE_TARGET_BUFFER_SIZE 512
#define REBALANCE_OVERLAPPING 4
#define REBALANCE_NUM_SPECTRO_COLS 32

enum RebalanceMode
{
    SOFT = 0,
    HARD
};

#endif
