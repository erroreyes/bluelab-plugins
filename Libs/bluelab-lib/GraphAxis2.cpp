//
//  GraphCurve.cpp
//  EQHack
//
//  Created by Apple m'a Tuer on 10/09/17.
//
//

#include <BLUtils.h>
#include <GraphSwapColor.h>

#include "GraphAxis2.h"

GraphAxis2::GraphAxis2()
{
    for (int i = 0; i < 4; i++)
        mColor[i] = 0;
    
    for (int i = 0; i < 4; i++)
        mLabelColor[i] = 0;
    
    mOffset = 0.0;
    mOffsetX = 0.0;
    mOffsetY = 0.0;
    
    mOverlay = false;
    for (int i = 0; i < 4; i++)
        mLabelOverlayColor[i] = 0;
    
    mLinesOverlay = false;;
    for (int i = 0; i < 4; i++)
        mLinesOverlayColor[i] = 0;
    
    mFontSizeCoeff = 1.0;
    
    mXdBScale = false;;
    
    mAlignTextRight = false;
    
    mAlignRight = false;
}

GraphAxis2::~GraphAxis2() {}

void
GraphAxis2::InitHAxis(bool xDbScale,
                      int axisColor[4], int axisLabelColor[4],
                      BL_GUI_FLOAT offsetY,
                      int axisOverlayColor[4],
                      BL_GUI_FLOAT fontSizeCoeff,
                      int axisLinesOverlayColor[4])
{
    mOffset = 0.0;
    
    // Warning, offset Y is normalized value
    mOffsetY = offsetY;
    mOverlay = false;
    mLinesOverlay = false;
    
    mFontSizeCoeff = fontSizeCoeff;
    
    mXdBScale = xDbScale;
    
    mAlignTextRight = false;
    mAlignRight = true;
    
    BL_FLOAT minVal = 0.0;
    BL_FLOAT maxVal = 1.0;
    
    InitAxis(axisColor, axisLabelColor,
             minVal, maxVal,
             axisOverlayColor, axisLinesOverlayColor);
}

void
GraphAxis2::InitVAxis(int axisColor[4], int axisLabelColor[4],
                      bool dbFlag, BL_GUI_FLOAT minY, BL_GUI_FLOAT maxY,
                      BL_GUI_FLOAT offset, BL_GUI_FLOAT offsetX,
                      int axisOverlayColor[4],
                      BL_GUI_FLOAT fontSizeCoeff, bool alignTextRight,
                      int axisLinesOverlayColor[4],
                      bool alignRight)
{
    mOverlay = false;
    mLinesOverlay = false;
    mOffset = offset;
    mOffsetX = offsetX;
    mOffsetY = 0.0;
    
    mFontSizeCoeff = fontSizeCoeff;
    
    mXdBScale = false;
    
    mAlignTextRight = alignTextRight;
    mAlignRight = alignRight;
    
    InitAxis(axisColor, axisLabelColor, minY, maxY,
             axisOverlayColor, axisLinesOverlayColor);
}

void
GraphAxis2::SetData(char *data[][2], int numData)
{
    mValues.clear();
    
    BL_FLOAT minVal = 0.0;
    BL_FLOAT maxVal = 1.0;
    
    // Copy data
    for (int i = 0; i < numData; i++)
    {
        char *cData[2] = { data[i][0], data[i][1] };
        
        BL_GUI_FLOAT t = atof(cData[0]);
        
        string text(cData[1]);
        
        // Error here, if we add an Y axis, we must not use mMinXdB
        GraphAxisData aData;
        aData.mT = (t - minVal)/(maxVal - minVal);
        aData.mText = text;
        
        mValues.push_back(aData);
    }
}

void
GraphAxis2::InitAxis(int axisColor[4],
                     int axisLabelColor[4],
                     BL_GUI_FLOAT minVal, BL_GUI_FLOAT maxVal,
                     int axisLabelOverlayColor[4],
                     int axisLinesOverlayColor[4])
{
    // Color
    int sAxisColor[4] = { axisColor[0], axisColor[1],
                          axisColor[2], axisColor[3] };
    SWAP_COLOR(sAxisColor);

    int sLabelColor[4] = { axisLabelColor[0], axisLabelColor[1],
                           axisLabelColor[2], axisLabelColor[3] };
    SWAP_COLOR(sLabelColor);
    
    int sOverColor[4] = { axisLabelOverlayColor[0], axisLabelOverlayColor[1],
                          axisLabelOverlayColor[2], axisLabelOverlayColor[3] };
    SWAP_COLOR(sOverColor);
    
    int sLineOverColor[4];
    if (axisLinesOverlayColor != NULL)
    {
        for (int i = 0; i < 4; i++)
        {
            sLineOverColor[i] = axisLinesOverlayColor[i];
        }
    }
    SWAP_COLOR(sLineOverColor);
    
    // Copy color
    for (int i = 0; i < 4; i++)
    {
        mColor[i] = sAxisColor[i];
        mLabelColor[i] = sLabelColor[i];
        
        mLabelOverlayColor[i] = 0;
        
        if (axisLabelOverlayColor != NULL)
        {
            mOverlay = true;
            mLabelOverlayColor[i] = sOverColor[i];
        }
        
        mLinesOverlayColor[i] = 0;
        
        if (axisLinesOverlayColor != NULL)
        {
            mLinesOverlay = true;
            
            mLinesOverlayColor[i] = sLineOverColor[i];
        }
    }
}
