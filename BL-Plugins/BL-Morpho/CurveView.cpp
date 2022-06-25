#include <GraphSwapColor.h>

#include <BLDebug.h>

#include "CurveView.h"

#define TITLE_OFFSET 10.0
#define TITLE_FONT_SIZE 15.0
#define TITLE_FONT "OpenSans-Bold"

CurveView::CurveView(const char *title,
                     BL_FLOAT defaultVal, int maxNumData)
{
    strcpy(mTitle, title);
    
    mDefaultVal = defaultVal;
    mMaxNumData = maxNumData;

    // Bounds
    mX = 0.0;
    mY = 0.0;
    mWidth = 64.0;
    mHeight = 64.0;

    // Style
    mBackgroundColor = IColor(64, 0, 0, 0);
    
    mBorderWidth = 1.0;
    mBorderColor = IColor(255, 255, 255, 255);

    mTitleColor = IColor(255, 255, 255, 255);

    mCurveColor = IColor(255, 255, 255, 255);

    mCurveLineWidth = 2.0;
    
    ResetData();

    mSpeedMod = 1;
    mAddNum = 0;
}

CurveView::~CurveView() {}

void
CurveView::SetBounds(float x, float y, float width, float height)
{
    mX = x;
    mY = y;
    mWidth = width;
    mHeight = height;
}

void
CurveView::ResetData()
{
    mData.resize(mMaxNumData);
    for (int i = 0; i < mData.size(); i++)
        mData[i] = mDefaultVal;
    mData.freeze();
}

void
CurveView::PushData(BL_FLOAT data)
{
    bool skipAdd = ((mAddNum++ % mSpeedMod) != 0);
    if (skipAdd)
        return;

    if (mData.size() == mMaxNumData)
    {
        mData.freeze();
        mData.push_pop(data);
    }
    else
        mData.push_back(data);
}

void
CurveView::AddData(BL_FLOAT data, BL_FLOAT t)
{
    if (mData.size() != mMaxNumData)
        return;

    if ((t < 0.0) || (t > 1.0))
        return;

    int idx = t*(mData.size() - 1);

    mData[idx] = data;
}

bool
CurveView::SpeedModCanAdd()
{
    bool skipAdd = ((mAddNum++ % mSpeedMod) != 0);
    if (skipAdd)
        return false;

    return true;
}

void
CurveView::SetSpeedMod(int speedMod)
{
    mSpeedMod = speedMod;
}

void
CurveView::PostDraw(NVGcontext *vg, int width, int height)
{
    DrawBackground(vg, width, height);
    
    DrawCurve(vg, width, height);
    
    DrawTitle(vg, width, height);
        
    // At the end
    DrawBorder(vg, width, height);
}

void
CurveView::DrawBackground(NVGcontext *vg, int width, int height)
{
    nvgSave(vg);
    nvgReset(vg);

    // Color
    int sColor[4] = { mBackgroundColor.R, mBackgroundColor.G,
                      mBackgroundColor.B, mBackgroundColor.A };
    SWAP_COLOR(sColor);
    
    nvgFillColor(vg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));

    nvgBeginPath(vg);
    
    nvgRect(vg, mX, mY, mWidth, mHeight);

    nvgFill(vg);
    
    nvgRestore(vg);
}

void
CurveView::DrawTitle(NVGcontext *vg, int width, int height)
{
    nvgSave(vg);
    nvgReset(vg);

    // Color
    int sColor[4] = { mTitleColor.R, mTitleColor.G,
                      mTitleColor.B, mTitleColor.A };
    SWAP_COLOR(sColor);
    
    nvgFillColor(vg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));

    // Text params
    nvgFontSize(vg, TITLE_FONT_SIZE);
	nvgFontFace(vg, TITLE_FONT);
    nvgFontBlur(vg, 0);
	nvgTextAlign(vg, NVG_ALIGN_LEFT |NVG_ALIGN_MIDDLE);

    // Draw text
    nvgText(vg, mX + TITLE_OFFSET, mY + TITLE_OFFSET, mTitle, NULL);
    
    nvgRestore(vg);
}
    
void
CurveView::DrawBorder(NVGcontext *vg, int width, int height)
{
    nvgSave(vg);
    nvgReset(vg);
    
    nvgStrokeWidth(vg, mBorderWidth);

    nvgStrokeColor(vg, nvgRGBA(mBorderColor.R,
                               mBorderColor.G,
                               mBorderColor.B,
                               mBorderColor.A));

    nvgBeginPath(vg);

    nvgMoveTo(vg,  mX + mBorderWidth*0.5,          mY + mBorderWidth*0.5);
    nvgLineTo(vg,  mX + mWidth - mBorderWidth*0.5, mY + mBorderWidth*0.5);
    nvgLineTo(vg,  mX + mWidth - mBorderWidth*0.5, mY + mHeight - mBorderWidth*0.5);
    nvgLineTo(vg,  mX + mBorderWidth*0.5,          mY + mHeight - mBorderWidth*0.5);
    
    nvgClosePath(vg);
    
    nvgStroke(vg);

    nvgRestore(vg);
}
