//
//  GraphFreqAxis2.cpp
//  BL-InfrasonicViewer
//
//  Created by applematuer on 11/22/19.
//
//

#include <GraphAxis2.h>
#include <GUIHelper12.h>
#include <BLUtils.h>
#include <Scale.h>

#include "GraphFreqAxis2.h"


GraphFreqAxis2::GraphFreqAxis2()
{
    mGraphAxis = NULL;
    
    mBufferSize = 2048;
    mSampleRate = 44100.0;
}

GraphFreqAxis2::~GraphFreqAxis2() {}

void
GraphFreqAxis2::Init(GraphAxis2 *graphAxis, GUIHelper12 *guiHelper,
                     bool horizontal,
                     int bufferSize, BL_FLOAT sampleRate,
                     int graphWidth)
{
    mGraphAxis = graphAxis;
    
    mBufferSize = bufferSize;
    mSampleRate = sampleRate;
    
    //
    IColor axisIColor;
    guiHelper->GetGraphAxisColor(&axisIColor);
    int axisColor[4] = { axisIColor.R, axisIColor.G, axisIColor.B, axisIColor.A };
    
    IColor axisLabelIColor;
    guiHelper->GetGraphAxisLabelColor(&axisLabelIColor);
    int axisLabelColor[4] = { axisLabelIColor.R, axisLabelIColor.G,
                              axisLabelIColor.B, axisLabelIColor.A };
    
    IColor axisLabelOverlayIColor;
    guiHelper->GetGraphAxisLabelOverlayColor(&axisLabelOverlayIColor);
    int axisLabelOverlayColor[4] = { axisLabelOverlayIColor.R,
                                     axisLabelOverlayIColor.G,
                                     axisLabelOverlayIColor.B,
                                     axisLabelOverlayIColor.A };
    
    //
    if (horizontal)
    {
        mGraphAxis->InitHAxis(Scale::LOG_COEFF,
                              0.0, sampleRate*0.5,
                              axisColor, axisLabelColor,
                              0.0,
                              axisLabelOverlayColor);
    }
    else
    {
        mGraphAxis->InitVAxis(Scale::LOG_COEFF, 0.0, sampleRate*0.5,
                              axisColor, axisLabelColor,
                              0.0, graphWidth - 40.0,
                              axisLabelOverlayColor);
    }
    
    //
    Update();
}

void
GraphFreqAxis2::Reset(int bufferSize, BL_FLOAT sampleRate)
{
    mBufferSize = bufferSize;
    mSampleRate = sampleRate;
    
    mGraphAxis->SetMinMaxValues(0.0, sampleRate*0.5);
    
    Update();
}

void
GraphFreqAxis2::Update()
{
    // Just in case
    if (mGraphAxis == NULL)
        return;
    
#define NUM_AXIS_DATA 12
    char *AXIS_DATA [NUM_AXIS_DATA][2];
    for (int i = 0; i < NUM_AXIS_DATA; i++)
    {
        AXIS_DATA[i][0] = (char *)malloc(255);
        AXIS_DATA[i][1] = (char *)malloc(255);
    }
    
    sprintf(AXIS_DATA[0][1], "");
    sprintf(AXIS_DATA[1][1], "50Hz");
    sprintf(AXIS_DATA[2][1], "100Hz");
    sprintf(AXIS_DATA[3][1], "500Hz");
    sprintf(AXIS_DATA[4][1], "1KHz");
    sprintf(AXIS_DATA[5][1], "2KHz");
    sprintf(AXIS_DATA[6][1], "5KHz");
    sprintf(AXIS_DATA[7][1], "10KHz");
    sprintf(AXIS_DATA[8][1], "20KHz");
    sprintf(AXIS_DATA[9][1], "40KHz");
    sprintf(AXIS_DATA[10][1], "80KHz");
    sprintf(AXIS_DATA[11][1], "");
    
    BL_FLOAT freqs[NUM_AXIS_DATA] =
                    { 25.0, 50.0, 100.0, 500.0, 1000.0, 2000.0, 5000.0,
                      10000.0, 20000.0, 40000.0, 80000.0, 176400.0 };

    BL_FLOAT minHzValue;
    BL_FLOAT maxHzValue;
    BLUtils::GetMinMaxFreqAxisValues(&minHzValue, &maxHzValue,
                                     mBufferSize, mSampleRate);
    
    // Avoid a shift
    minHzValue = 0.0;
    
    // Normalize
    //int start = 0;
    //int end = NUM_AXIS_DATA - 1;
    for (int i = 0; i < NUM_AXIS_DATA; i++)
    {
        //freqs[i] = (freqs[i] - minHzValue)/(maxHzValue - minHzValue);
        
        //freqs[i] = BLUtils::LogScaleNormInv(freqs[i],
        //                                    (BL_FLOAT)1.0,
        //                                    (BL_FLOAT)Y_LOG_SCALE_FACTOR);
        
        sprintf(AXIS_DATA[i][0], "%g", freqs[i]);
        
        // We are over the sample rate, make empty label
        //if ((freqs[i] < 0.0) || (freqs[i] > 1.0))
        if ((freqs[i] < minHzValue) || (freqs[i] > maxHzValue))
            sprintf(AXIS_DATA[i][1], "");
        
        //if (freqs[i] < 0.0)
        //    start++;
        //if (freqs[i] > 1.0)
        //    end--;
    }
    
    mGraphAxis->SetData(AXIS_DATA, NUM_AXIS_DATA);
    // Adjust the number of values,
    // because the graph automatically fixes the bounds alignement
    //mGraphAxis->SetData(&AXIS_DATA[start], end - start + 1);
    
    for (int i = 0; i < NUM_AXIS_DATA; i++)
    {
        free(AXIS_DATA[i][0]);
        free(AXIS_DATA[i][1]);
    }
}

