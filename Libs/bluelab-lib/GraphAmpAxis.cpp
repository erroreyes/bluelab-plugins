//
//  GraphAmpAxis.cpp
//  BL-InfrasonicViewer
//
//  Created by applematuer on 11/22/19.
//
//

#include <GraphAxis2.h>
#include <GUIHelper12.h>
#include <BLUtils.h>

#include "GraphAmpAxis.h"


GraphAmpAxis::GraphAmpAxis()
{
    mGraphAxis = NULL;
    
    mMinDB = -60.0;
    mMaxDB = 0.0;
}

GraphAmpAxis::~GraphAmpAxis() {}

void
GraphAmpAxis::Init(GraphAxis2 *graphAxis,
                   GUIHelper12 *guiHelper,
                   BL_FLOAT minDB, BL_FLOAT maxDB,
                   int graphWidth)
{
    mMinDB = minDB;
    mMaxDB = maxDB;
    
    mGraphAxis = graphAxis;
    
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
    
    mGraphAxis->InitVAxis(axisColor, axisLabelColor,
                          true,
                          0.0, 1.0,
                          0.0,
                          //graphWidth - 40.0, // on the right
                          4.0, //40.0, // on the left
                          axisLabelOverlayColor);
    
    //
    Update();
}

void
GraphAmpAxis::Reset(BL_FLOAT minDB, BL_FLOAT maxDB)
{
    mMinDB = minDB;
    mMaxDB = maxDB;
    
    Update();
}

void
GraphAmpAxis::Update()
{
    // Just in case
    if (mGraphAxis == NULL)
        return;
    
#define NUM_AXIS_DATA 10
    char *AXIS_DATA [NUM_AXIS_DATA][2];
    for (int i = 0; i < NUM_AXIS_DATA; i++)
    {
        AXIS_DATA[i][0] = (char *)malloc(255);
        AXIS_DATA[i][1] = (char *)malloc(255);
    }
    
    sprintf(AXIS_DATA[0][1], "-160dB");
    sprintf(AXIS_DATA[1][1], "-140dB");
    sprintf(AXIS_DATA[2][1], "-120dB");
    sprintf(AXIS_DATA[3][1], "-100dB");
    sprintf(AXIS_DATA[4][1], "-80dB");
    sprintf(AXIS_DATA[5][1], "-60dB");
    sprintf(AXIS_DATA[6][1], "-40dB");
    sprintf(AXIS_DATA[7][1], "-20dB");
    sprintf(AXIS_DATA[8][1], "0dB");
    sprintf(AXIS_DATA[9][1], "20dB");
    
    BL_FLOAT amps[NUM_AXIS_DATA] =
                    { -160.0, -140.0, -120.0, -100.0, -80.0,
                      -60.0, -40.0, -20.0, 0.0, 20.0 };

    // Normalize
    int start = 0;
    int end = NUM_AXIS_DATA - 1;
    for (int i = 0; i < NUM_AXIS_DATA; i++)
    {
        // Normalized value
        amps[i] = (amps[i] - mMinDB)/(mMaxDB - mMinDB);
        
        //freqs[i] = BLUtils::LogScaleNormInv(freqs[i],
        //                                    (BL_FLOAT)1.0,
        //                                    (BL_FLOAT)Y_LOG_SCALE_FACTOR);
        
        sprintf(AXIS_DATA[i][0], "%g", amps[i]);
        
        // We are outside the range, don't display label
        if ((amps[i] < 0.0) || (amps[i] > 1.0))
        //if ((amps[i] <= 0.0) || (amps[i] >= 1.0))
            sprintf(AXIS_DATA[i][1], "");
        
        //if (amps[i] < 0.0)
        //    start++;
        
        //if (amps[i] > 1.0)
        //    end--;
    }
    
    //mGraphAxis->SetData(AXIS_DATA, NUM_AXIS_DATA);
    
    // Adjust the number of values,
    // because the graph automatically fixes the bounds alignement
    mGraphAxis->SetData(&AXIS_DATA[start], end - start + 1);
    
    for (int i = 0; i < NUM_AXIS_DATA; i++)
    {
        free(AXIS_DATA[i][0]);
        free(AXIS_DATA[i][1]);
    }
}

