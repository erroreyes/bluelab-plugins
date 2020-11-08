//
//  GraphCurve.h
//  EQHack
//
//  Created by Apple m'a Tuer on 10/09/17.
//
//

#ifndef GraphAxis2_h
#define GraphAxis2_h

#include <string>
using namespace std;

#include <BLTypes.h>

#include "IPlug_include_in_plug_hdr.h"

class GraphAxis2
{
public:
    GraphAxis2();
    
    virtual ~GraphAxis2();
    
    void InitHAxis(bool xDbScale,
                   int axisColor[4], int axisLabelColor[4],
                   BL_GUI_FLOAT offsetY = 0.0,
                   int axisOverlayColor[4] = NULL,
                   BL_GUI_FLOAT fontSizeCoeff = 1.0,
                   int axisLinesOverlayColor[4] = NULL);
    
    void InitVAxis(int axisColor[4], int axisLabelColor[4],
                   bool dbFlag, BL_GUI_FLOAT minY, BL_GUI_FLOAT maxY,
                   BL_GUI_FLOAT offset = 0.0, BL_GUI_FLOAT offsetX = 0.0,
                   int axisOverlayColor[4] = NULL,
                   BL_GUI_FLOAT fontSizeCoeff = 1.0,
                   bool alignTextRight = false,
                   int axisLinesOverlayColor[4] = NULL,
                   bool alignRight = true);
    
    void SetData(char *data[][2], int numData);
    
protected:
    void InitAxis(int axisColor[4],
                  int axisLabelColor[4],
                  BL_GUI_FLOAT minVal, BL_GUI_FLOAT maxVal,
                  int axisLabelOverlayColor[4],
                  int axisLinesOverlayColor[4]);
    
    typedef struct
    {
        BL_GUI_FLOAT mT;
        string mText;
    } GraphAxisData;

    friend class GraphControl12;
    
    //
    vector<GraphAxisData> mValues;
    
    int mColor[4];
    int mLabelColor[4];
    
    // Hack
    BL_GUI_FLOAT mOffset;
    
    // To be able to display the axis on the right
    BL_GUI_FLOAT mOffsetX;
    
    BL_GUI_FLOAT mOffsetY;
    
    // Overlay axis labels ?
    bool mOverlay;
    int mLabelOverlayColor[4];
    
    // Overlay axis lines ?
    bool mLinesOverlay;
    int mLinesOverlayColor[4];
    
    BL_GUI_FLOAT mFontSizeCoeff;
    
    //
    bool mXdBScale;
    
    bool mAlignTextRight;
    
    bool mAlignRight;
};

#endif
