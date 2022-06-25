#include <GraphSwapColor.h>

#include "CurveViewAmp.h"

CurveViewAmp::CurveViewAmp(int maxNumData)
: CurveView("amplitude", 0.0, maxNumData) {}

CurveViewAmp::~CurveViewAmp() {}

void
CurveViewAmp::DrawCurve(NVGcontext *vg, int width, int height)
{
    if (mData.empty())
        return;
    
    nvgSave(vg);
    nvgReset(vg);
    
    // Color
    int sColor[4] = { mCurveColor.R, mCurveColor.G,
                      mCurveColor.B, mCurveColor.A };
    SWAP_COLOR(sColor);
    nvgStrokeColor(vg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));

    // Width
    nvgStrokeWidth(vg, mCurveLineWidth);

    // Bevel
    nvgLineJoin(vg, NVG_BEVEL);

    // Draw the curve
    nvgBeginPath(vg);

    // First point
    BL_FLOAT ty0 = 1.0 - mData[0];
    if (ty0 > 1.0)
        ty0 = 1.0;
    if (ty0 < 0.0)
        ty0 = 0.0;
    BL_FLOAT y0 = ty0*(mHeight - CURVE_Y_OFFSET*2);
        
    nvgMoveTo(vg, mX, mY + y0 + CURVE_Y_OFFSET);
    
    for (int i = 1; i < mData.size(); i++)
    {
        BL_FLOAT tx = ((BL_FLOAT)i)/(mData.size() - 1);
        BL_FLOAT ty = 1.0 - mData[i];
        if (ty > 1.0)
            ty = 1.0;
        if (ty < 0.0)
            ty = 0.0;
        
        BL_FLOAT y = ty*(mHeight - CURVE_Y_OFFSET*2);

        nvgLineTo(vg, mX + tx*mWidth,
                  mY + y + CURVE_Y_OFFSET);
    }

    nvgStroke(vg);
    
    nvgRestore(vg);
}
