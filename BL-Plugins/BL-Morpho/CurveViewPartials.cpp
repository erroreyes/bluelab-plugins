#include <BLUtilsMath.h>

#include <GraphSwapColor.h>

#include "CurveViewPartials.h"

#define CURVE_Y_OFFSET 10

CurveViewPartials::CurveViewPartials(int maxNumData)
: CurveView("partials", 0.0, maxNumData)
{
    // Blue
    //mPartialsCurveColor = IColor(255, 0, 44, 97);
    mPartialsCurveColor = IColor(255, 255, 255, 255);
    
    mPartialsCurveLineWidth = 1.0;
}

CurveViewPartials::~CurveViewPartials() {}

void
CurveViewPartials::DrawCurve(NVGcontext *vg, int width, int height)
{
    DrawBottomLine(vg, width, height);
    DrawPartials(vg, width, height);
    // Looks drafty...
    //DrawPartialsCurve(vg, width, height);
}

void
CurveViewPartials::DrawPartials(NVGcontext *vg, int width, int height)
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
    //nvgLineJoin(vg, NVG_BEVEL);

    BL_FLOAT bottomY = (mHeight - CURVE_Y_OFFSET*2);
        
    // Draw the curve
    for (int i = 0; i < mData.size(); i++)
    {
        BL_FLOAT tx = ((BL_FLOAT)i)/(mData.size() - 1);
        //BL_FLOAT x = mCurveLineWidth + tx*(mWidth - 2.0*mCurveLineWidth);
        BL_FLOAT x = mCurveLineWidth + tx*(mWidth - mCurveLineWidth);
        
        BL_FLOAT ty = 1.0 - mData[i];
        if (ty > 1.0)
            ty = 1.0;
        if (ty < 0.0)
            ty = 0.0;
        
        BL_FLOAT y = ty*(mHeight - CURVE_Y_OFFSET*2);
 
        nvgBeginPath(vg);
        
        nvgMoveTo(vg, mX + x,
                  mY + bottomY + CURVE_Y_OFFSET);
        
        nvgLineTo(vg, mX + x,
                  mY + y + CURVE_Y_OFFSET);

        nvgStroke(vg);
    }
    
    nvgRestore(vg);
}

void
CurveViewPartials::DrawPartialsCurve(NVGcontext *vg, int width, int height)
{    
    if (mData.empty())
        return;
    
    nvgSave(vg);
    nvgReset(vg);
    
    // Color
    int sColor[4] = { mPartialsCurveColor.R, mPartialsCurveColor.G,
                      mPartialsCurveColor.B, mPartialsCurveColor.A };
    SWAP_COLOR(sColor);
    nvgStrokeColor(vg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));

    // Width
    nvgStrokeWidth(vg, mPartialsCurveLineWidth);

    // Bevel
    nvgLineJoin(vg, NVG_BEVEL);

    // Draw the curve
    nvgBeginPath(vg);
    
    BL_FLOAT ty0 = 1.0 - mData[0];
    if (ty0 > 1.0)
        ty0 = 1.0;
    if (ty0 < 0.0)
        ty0 = 0.0;
    BL_FLOAT y0 = ty0*(mHeight - CURVE_Y_OFFSET*2);
    
    nvgMoveTo(vg, mX, mY + y0 + CURVE_Y_OFFSET);
    
    for (int i = 1; i < mData.size(); i++)
    {
        if ((i < mData.size() - 1) && (mData[i] < BL_EPS))
            continue;
        
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

void
CurveViewPartials::DrawBottomLine(NVGcontext *vg, int width, int height)
{
    nvgSave(vg);
    nvgReset(vg);
    
    nvgStrokeWidth(vg, mBorderWidth);

    nvgStrokeColor(vg, nvgRGBA(mBorderColor.R,
                               mBorderColor.G,
                               mBorderColor.B,
                               mBorderColor.A));

    nvgBeginPath(vg);

    nvgMoveTo(vg,  mX + mBorderWidth*0.5,
              mY + mHeight - (mBorderWidth*0.5 + CURVE_Y_OFFSET));
    nvgLineTo(vg,  mX + mWidth - mBorderWidth*0.5,
              mY + mHeight - (mBorderWidth*0.5 + CURVE_Y_OFFSET));
    
    nvgClosePath(vg);
    
    nvgStroke(vg);

    nvgRestore(vg);
}
