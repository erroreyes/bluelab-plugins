//
//  Configs.h
//  Impulses
//
//  Created by Pan on 20/01/18.
//
//

#ifndef Impulses_Configs2_h
#define Impulses_Configs2_h

// Configs2: start fort value before 0s

// Examples about reverb time:
//
// Ref: "Le Livre des techniques du son" - Denis Mercier, Collectif Dunod
//
// RT60
// - studio: 0.3s
// - empty room: 4s
// - big hall: 12s
// - cathedral: ~10s (ND. Paris: 9s)
// - reverberation room: 40s
// - world record (Scotland, 2014): 112s

// HACK: when cropping 2 pixels of graph on the right,
// the last label would be too close to the border
//#define HACK_SHIFT_X_SCALE 0.003 // 2 pixels
#define HACK_SHIFT_X_SCALE 0.006

// Config 50ms

// 1 dirac per second
#define DIRAC_FREQUENCY_50MS 1.0

// Horizontal axis: time
#define MIN_X_AXIS_VALUE_50MS 0.0
#define MAX_X_AXIS_VALUE_50MS 0.06 + HACK_SHIFT_X_SCALE*0.05

// Axis data
#define NUM_AXIS_DATA 7

#define SHIFT_TIME_50MS 0.01

static char *AXIS_DATA_50MS [NUM_AXIS_DATA][2] =
{
    { "0.0", "" },
    { "0.01", "0 ms" },
    { "0.02", "10 ms" },
    { "0.03", "20 ms" },
    { "0.04", "30 ms" },
    { "0.05", "40 ms" },
    { "0.06", "50 ms" }
};

// Config 100ms

// 1 dirac per second
#define DIRAC_FREQUENCY_100MS 1.0

// Horizontal axis: time
#define MIN_X_AXIS_VALUE_100MS 0.0
#define MAX_X_AXIS_VALUE_100MS 0.12 + HACK_SHIFT_X_SCALE*0.1

#define SHIFT_TIME_100MS 0.02

static char *AXIS_DATA_100MS [NUM_AXIS_DATA][2] =
{
    { "0.0", "" },
    { "0.02", "0 ms" },
    { "0.04", "20 ms" },
    { "0.06", "40 ms" },
    { "0.08", "60 ms" },
    { "0.1", "80 ms" },
    { "0.12", "100 ms" }
};

// Config 1s

// 1 dirac every 2second
#define DIRAC_FREQUENCY_1S 1.0/2.0

// Horizontal axis: time
#define MIN_X_AXIS_VALUE_1S 0.0
#define MAX_X_AXIS_VALUE_1S 1.2 + + HACK_SHIFT_X_SCALE*1.0

#define SHIFT_TIME_1S 0.2

static char *AXIS_DATA_1S [NUM_AXIS_DATA][2] =
{
    { "0.0", "" },
    { "0.2", "0 ms" },
    { "0.4", "200 ms" },
    { "0.6", "400 ms" },
    { "0.8", "600 ms" },
    { "1.0", "800 ms" },
    { "1.2", "1000 ms" }
};

// Config 5s

// 1 dirac every 6 second
#define DIRAC_FREQUENCY_5S 1.0/6.0

// Horizontal axis: time
#define MIN_X_AXIS_VALUE_5S 0.0
#define MAX_X_AXIS_VALUE_5S 6.0 + + HACK_SHIFT_X_SCALE*5.0

#define SHIFT_TIME_5S 1.0

static char *AXIS_DATA_5S [NUM_AXIS_DATA][2] =
{
    { "0.0", "" },
    { "1.0", "0 s" },
    { "2.0", "1 s" },
    { "3.0", "2 s" },
    { "4.0", "3 s" },
    { "5.0", "4 s" },
    { "6.0", "5 s" }
};

// Config 10s

// 1 dirac every 11s
#define DIRAC_FREQUENCY_10S 1.0/11.0

// Horizontal axis: time
#define MIN_X_AXIS_VALUE_10S 0.0
#define MAX_X_AXIS_VALUE_10S 12.0 + HACK_SHIFT_X_SCALE*10.0

#define SHIFT_TIME_10S 2.0

static char *AXIS_DATA_10S [NUM_AXIS_DATA][2] =
{
    { "0.0", "" },
    { "2.0", "0 s" },
    { "4.0", "2 s" },
    { "6.0", "4 s" },
    { "8.0", "6 s" },
    { "10.0", "8 s" },
    { "12.0", "10 s" }
};

#endif
