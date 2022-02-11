//
//  Graph.cpp
//  Transient
//
//  Created by Apple m'a Tuer on 03/09/17.
//
//

#include <math.h>

#include <stdio.h>

#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#endif

#include "nanovg.h"

// Warning: Niko hack in NanoVg to support FBO even on GL2
#define NANOVG_GL2_IMPLEMENTATION

#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

#include "../../WDL/IPlug/IPlugBase.h"

// Plugin resource file
#include "resource.h"

#include <BLUtils.h>
#include "GLContext2.h"
#include "GraphControl2.h"

#define FONT_SIZE 14.0

#ifdef WIN32
#define SWAP_COLOR(__RGBA__)
#endif

#ifdef __APPLE__
// Convert from RGBA to ABGR
#define SWAP_COLOR(__RGBA__) \
{ int tmp[4] = { __RGBA__[0], __RGBA__[1], __RGBA__[2], __RGBA__[3] }; \
__RGBA__[0] = tmp[2]; \
__RGBA__[1] = tmp[1]; \
__RGBA__[2] = tmp[0]; \
__RGBA__[3] = tmp[3]; }
#endif


GraphControl2::GraphControl2(IPlugBase *pPlug, IRECT pR,
	int numCurves, int numCurveValues,
	const char *fontPath)
	: IControl(pPlug, pR),
	mFontInitialized(false),
	mAutoAdjustParamSmoother(1.0, 0.9),
	mNumCurves(numCurves),
	mNumCurveValues(numCurveValues),
	mHAxis(NULL),
	mVAxis(NULL)
{
	for (int i = 0; i < mNumCurves; i++)
	{
		GraphCurve *curve = new GraphCurve(numCurveValues);

		mCurves.push_back(curve);
	}

	mAutoAdjustFlag = false;
	mAutoAdjustFactor = 1.0;

	mYScaleFactor = 1.0;

	mXdBScale = false;
	mMinXdB = 0.1;
	mMaxXdB = 1.0;
    
    mPixels = (unsigned char *)malloc(this->mRECT.W()*this->mRECT.H()*4*sizeof(unsigned char));
    
    SetClearColor(0, 0, 0, 255);
    
    mDirty = true;
    
    // GL and nanovg
    mGLInitialized = false;
    mFontPath.Set(fontPath);
    
    InitGL();
    InitNanoVg();
    
#if PROFILE_GRAPH
    mDebugCount = 0;
#endif
}

GraphControl2::~GraphControl2()
{
    free(mPixels);
    
    for (int i = 0; i < mNumCurves; i++)
        delete mCurves[i];
    
    if (mHAxis != NULL)
        delete mHAxis;
    
    if (mVAxis != NULL)
        delete mVAxis;
    
    ExitNanoVg();
    ExitGL();
}

bool
GraphControl2::IsDirty()
{
    return mDirty;
}

bool
GraphControl2::Draw(IGraphics *pGraphics)
{
#if PROFILE_GRAPH
    mDebugTimer.Start();
#endif
    
    IPlugBase::IMutexLock lock(mPlug);
    
    DrawGL();
    
    // This is the important part where you bind the cairo data to LICE
    LICE_WrapperBitmap wrapperBitmap((LICE_pixel *)mPixels, this->mRECT.W(), this->mRECT.H(),
                                     this->mRECT.W(), false);
    
    // And we render
    IBitmap result(&wrapperBitmap, wrapperBitmap.getWidth(), wrapperBitmap.getHeight());
    
    mDirty = false;
    
#if PROFILE_GRAPH
    mDebugTimer.Stop();
    
    if (mDebugCount++ % 100 == 0)
    {
        long t = mDebugTimer.Get();
        mDebugTimer.Reset();
        
        char message[1024];
        sprintf(message, "Draw(x): %ld\n", t);
        Debug::DumpMessage("TransientProfile.txt", message);
    }
#endif
    
    return pGraphics->DrawBitmap(&result, &this->mRECT);
}

void
GraphControl2::AddHAxis(char *data[][2], int numData, int axisColor[4], int axisLabelColor[4])
{
    mHAxis = new GraphAxis();
    mHAxis->mOffset = 0.0;
    
    //int sAxisColor[4] = { axisColor[0], axisColor[1], axisColor[2], axisColor[3] };
    //SWAP_COLOR(sAxisColor);
    
    AddAxis(mHAxis, data, numData, axisColor, axisLabelColor);
}

void
GraphControl2::AddVAxis(char *data[][2], int numData, int axisColor[4], int axisLabelColor[4], BL_FLOAT offset)
{
    mVAxis = new GraphAxis();
    
    mVAxis->mOffset = offset;
    
    //int sAxisColor[4] = { axisColor[0], axisColor[1], axisColor[2], axisColor[3] };
    //SWAP_COLOR(sAxisColor);
    
    AddAxis(mVAxis, data, numData, axisColor, axisLabelColor);
}

void
GraphControl2::SetXdBScale(bool flag, BL_FLOAT minXdB, BL_FLOAT maxXdB)
{
    mXdBScale = flag;
    
    mMinXdB = minXdB;
    mMaxXdB = maxXdB;
}

void
GraphControl2::SetAutoAdjust(bool flag, BL_FLOAT smoothCoeff)
{
    mAutoAdjustFlag = flag;
    
    mAutoAdjustParamSmoother.SetSmoothCoeff(smoothCoeff);
    
    mDirty = true;
}

void
GraphControl2::SetYScaleFactor(BL_FLOAT factor)
{
    mYScaleFactor = factor;
    
    mDirty = true;
}

void
GraphControl2::SetClearColor(int r, int g, int b, int a)
{
    //int clearColor[4] = { 0, 0, 0, 255 };
    //SWAP_COLOR(clearColor);
    //SET_COLOR_FROM_INT(mClearColor, clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    
    SET_COLOR_FROM_INT(mClearColor, r, g, b, a);
    
    mDirty = true;
}

void
GraphControl2::SetCurveColor(int curveNum, int r, int g, int b)
{
    if (curveNum >= mNumCurves)
        return;
    
    //int sCurveColor[4] = { r, g, b, 255 };
    //SWAP_COLOR(sCurveColor);
    
    SET_COLOR_FROM_INT(mCurves[curveNum]->mColor, r, g, b, 255);
    
    mDirty = true;
}

void
GraphControl2::SetCurveAlpha(int curveNum, BL_FLOAT alpha)
{
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->mAlpha = alpha;
    
    mDirty = true;
}

void
GraphControl2::SetCurveLineWidth(int curveNum, BL_FLOAT lineWidth)
{
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->mLineWidth = lineWidth;
    
    mDirty = true;
}

void
GraphControl2::SetCurveSmooth(int curveNum, bool flag)
{
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->mDoSmooth = flag;
    
    mDirty = true;
}

void
GraphControl2::SetCurveFill(int curveNum, bool flag)
{
    if (curveNum >= mNumCurves)
        return;

    mCurves[curveNum]->mCurveFill = flag;
    
    mDirty = true;
}

void
GraphControl2::SetCurveFillAlpha(int curveNum, BL_FLOAT alpha)
{
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->mFillAlpha = alpha;
    
    mDirty = true;
}

void
GraphControl2::SetCurveSingleValue(int curveNum, bool flag)
{
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->mSingleValue = flag;
    
    mDirty = true;
}

void
GraphControl2::AddAxis(GraphAxis *axis, char *data[][2], int numData, int axisColor[4], int axisLabelColor[4])
{
    // Copy color
    for (int i = 0; i < 4; i++)
    {
        int sAxisColor[4] = { axisColor[0], axisColor[1], axisColor[2], axisColor[3] };
        SWAP_COLOR(sAxisColor);
        
        axis->mColor[i] = sAxisColor[i];
        
        int sLabelColor[4] = { axisLabelColor[0], axisLabelColor[1],
                               axisLabelColor[2], axisLabelColor[3] };
        SWAP_COLOR(sLabelColor);
        
        axis->mLabelColor[i] = sLabelColor[i];
    }
    
    // Copy data
    for (int i = 0; i < numData; i++)
    {
        char *cData[2] = { data[i][0], data[i][1] };
        
        BL_FLOAT t = atof(cData[0]);
        
        string text(cData[1]);
        
        // Error here, if we add an Y axis, we must not use mMinXdB
        GraphAxisData aData;
        aData.mT = (t - mMinXdB)/(mMaxXdB - mMinXdB);
        aData.mText = text;
        
        axis->mValues.push_back(aData);
    }
}

void
GraphControl2::ResetCurve(int curveNum, BL_FLOAT val)
{
    IPlugBase::IMutexLock lock(mPlug);
    
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->FillAllValues(val);
    
    mDirty = true;
}

void
GraphControl2::SetCurveYdBScale(int curveNum, bool flag, BL_FLOAT minYdB, BL_FLOAT maxYdB)
{
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->SetYdBScale(flag, minYdB, maxYdB);
    
    mDirty = true;
}

void
GraphControl2::SetCurveValues(int curveNum, const WDL_TypedBuf<BL_FLOAT> *values)
{
    // Must lock otherwise with DATA_STEP, curve will jerk
    IPlugBase::IMutexLock lock(mPlug);
    
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->SetValues(values);
    
    mDirty = true;
}

void
GraphControl2::SetCurveValue(int curveNum, BL_FLOAT t, BL_FLOAT val)
{
    // Must lock otherwise we may have curve will jerk (??)
    IPlugBase::IMutexLock lock(mPlug);
    
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->SetValue(t, val);
    
    mDirty = true;
}

void
GraphControl2::SetCurveSingleValue(int curveNum, BL_FLOAT val)
{
    // Must lock otherwise with DATA_STEP, curve will jerk
    IPlugBase::IMutexLock lock(mPlug);
    
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->SetValue(0.0, val);
    
    mDirty = true;
}


void
GraphControl2::PushCurveValue(int curveNum, BL_FLOAT val)
{
    IPlugBase::IMutexLock lock(mPlug);
    
    if (curveNum >= mNumCurves)
        return;
    
    mCurves[curveNum]->PushValue(val);
    mDirty = true;
}

void
GraphControl2::DrawAxis(bool lineLabelFlag)
{
    if (mHAxis != NULL)
        DrawAxis(mHAxis, true, lineLabelFlag);
    
    if (mVAxis != NULL)
        DrawAxis(mVAxis, false, lineLabelFlag);
}


void
GraphControl2::DrawAxis(GraphAxis *axis, bool horizontal, bool lineLabelFlag)
{
    nvgSave(mVg);
    nvgStrokeWidth(mVg, 1.0);
    
    int axisColor[4] = { axis->mColor[0], axis->mColor[1], axis->mColor[2], axis->mColor[3] };
    SWAP_COLOR(axisColor);
    
    nvgStrokeColor(mVg, nvgRGBA(axisColor[0], axisColor[1], axisColor[2], axisColor[3]));
    
    int width = this->mRECT.W();
    int height = this->mRECT.H();
    
    for (int i = 0; i < axis->mValues.size(); i++)
    {
        const GraphAxisData &data = axis->mValues[i];
        
        BL_FLOAT t = data.mT;
        const char *text = data.mText.c_str();

        if (horizontal)
        {
            BL_FLOAT textOffset = FONT_SIZE*0.2;
            
            t = BLUtils::NormalizedXTodB(t, mMinXdB, mMaxXdB);
            
            BL_FLOAT x = t*width;
        
            if ((i > 0) && (i < axis->mValues.size() - 1))
                // First and last: don't draw axis line
            {
                if (lineLabelFlag)
                {
                    // Draw a vertical line
                    nvgBeginPath(mVg);

                    BL_FLOAT y0 = 0.0;
                    BL_FLOAT y1 = height;
        
                    nvgMoveTo(mVg, x, y0);
                    nvgLineTo(mVg, x, y1);
    
                    nvgStroke(mVg);
                }
                else
                    DrawText(x, textOffset, FONT_SIZE, text, axis->mLabelColor, NVG_ALIGN_CENTER);
            }
     
            if (!lineLabelFlag)
            {
                if (i == 0)
                    // First text: aligne left
                    DrawText(x + textOffset, textOffset, FONT_SIZE, text, axis->mLabelColor, NVG_ALIGN_LEFT);
        
                if (i == axis->mValues.size() - 1)
                    // Last text: aligne right
                    DrawText(x - textOffset, textOffset, FONT_SIZE, text, axis->mLabelColor, NVG_ALIGN_RIGHT);
            }
        }
        else
            // Vertical
        {
            BL_FLOAT textOffset = FONT_SIZE*0.2;
            
            // Retreive the Y db scale
            BL_FLOAT minYdB = -40.0;
            BL_FLOAT maxYdB = 40.0;
            
            if (!mCurves.empty())
                // Get from the first curve
            {
                minYdB = mCurves[0]->mMinYdB;
                maxYdB = mCurves[0]->mMaxYdB;
            }
            
            t = (t - minYdB)/(maxYdB - minYdB);
            
            BL_FLOAT y = t*height;
            
            // Hack
            // See Transient, with it 2 vertical axis
            y += axis->mOffset;
            
            if ((i > 0) && (i < axis->mValues.size() - 1))
                // First and last: don't draw axis line
            {
                if (lineLabelFlag)
                {
                    // HERE DEBUG HERE !!!!
                    
                    // Draw a horizontal line
                    nvgBeginPath(mVg);
                
                    BL_FLOAT x0 = 0.0;
                    BL_FLOAT x1 = width;
                
                    nvgMoveTo(mVg, x0, y);
                    nvgLineTo(mVg, x1, y);
                
                    nvgStroke(mVg);

                }
                else
                    DrawText(textOffset, y, FONT_SIZE, text, axis->mLabelColor, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
            }
            
            if (!lineLabelFlag)
            {
                if (i == 0)
                    // First text: aligne "top"
                    DrawText(textOffset, y + FONT_SIZE*0.75, FONT_SIZE, text, axis->mLabelColor, NVG_ALIGN_LEFT);
            
                if (i == axis->mValues.size() - 1)
                    // Last text: aligne "bottom"
                    DrawText(textOffset, y - FONT_SIZE*1.5, FONT_SIZE, text, axis->mLabelColor, NVG_ALIGN_LEFT);
            }
        }
    }
    
    nvgRestore(mVg);
}

void
GraphControl2::DrawCurves()
{
    for (int i = 0; i < mNumCurves; i++)
    {
        if (!mCurves[i]->mSingleValue)
        {
            if (mCurves[i]->mCurveFill)
            {
                DrawFillCurve(&mCurves[i]->mValues,
                               mCurves[i]->mColor, mCurves[i]->mFillAlpha,
                               mCurves[i]->mAlpha, mCurves[i]->mLineWidth, mCurves[i]);
            }
            else
            {
                DrawLineCurve(&mCurves[i]->mValues,
                              mCurves[i]->mColor, mCurves[i]->mAlpha, mCurves[i]->mLineWidth, mCurves[i]);
            }
        }
        else
        {
            if (!mCurves[i]->mValues.GetSize() == 0)
            {
                if (mCurves[i]->mCurveFill)
                {
                    DrawFillCurveSV(mCurves[i]->mValues.Get()[0],
                                    mCurves[i]->mColor, mCurves[i]->mFillAlpha, mCurves[i]);
                }
            
                DrawLineCurveSV(mCurves[i]->mValues.Get()[0],
                                mCurves[i]->mColor, mCurves[i]->mAlpha,
                                mCurves[i]->mLineWidth, mCurves[i]);
            }
        }
    }
}

void
GraphControl2::DrawLineCurve(const WDL_TypedBuf<BL_FLOAT> *points, CurveColor color,
                            BL_FLOAT alpha, BL_FLOAT lineWidth,
                            GraphCurve *curve)
{
    int width = this->mRECT.W();
    int height = this->mRECT.H();
    
    nvgSave(mVg);
    nvgStrokeWidth(mVg, lineWidth);
    
    int sColor[4] = { (int)(color[0]*255), (int)(color[1]*255),
                      (int)(color[2]*255), (int)(alpha*255) };
    SWAP_COLOR(sColor);
    
    nvgStrokeColor(mVg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));
    nvgBeginPath(mVg);
    
    for (int i = 0; i < points->GetSize(); i ++)
    {
        // Normalize
        BL_FLOAT x = ((BL_FLOAT)i)/points->GetSize();
        
        if (mXdBScale)
            x = BLUtils::NormalizedXTodB(x, mMinXdB, mMaxXdB);
        
        // Scale for the interface
        x = x * width;
        
        BL_FLOAT val = points->Get()[i];
        
        if (curve->mYdBScale)
            val = BLUtils::NormalizedYTodB(val, curve->mMinYdB, curve->mMaxYdB);
        
        BL_FLOAT y = val * mAutoAdjustFactor * mYScaleFactor * height;
        
        if (i == 0)
            nvgMoveTo(mVg, x, y);
        
        nvgLineTo(mVg, x, y);
    }
    
    nvgStroke(mVg);
    nvgRestore(mVg);
    
    mDirty = true;
}

void
GraphControl2::DrawFillCurve(const WDL_TypedBuf<BL_FLOAT> *points, CurveColor color, BL_FLOAT fillAlpha,
                             BL_FLOAT strokeAlpha, BL_FLOAT lineWidth, GraphCurve *curve)
{
    // Offset used to draw the closing of the curve outside the viewport
    // Because we draw both stroke and fill at the same time
#define OFFSET lineWidth
    
    int width = this->mRECT.W();
    int height = this->mRECT.H();
    
    nvgSave(mVg);
    
    int sFillColor[4] = { (int)(color[0]*255), (int)(color[1]*255),
                      (int)(color[2]*255), (int)(fillAlpha*255) };
    SWAP_COLOR(sFillColor);
    
    int sStrokeColor[4] = { (int)(color[0]*255), (int)(color[1]*255),
        (int)(color[2]*255), (int)(strokeAlpha*255) };
    SWAP_COLOR(sStrokeColor);
    
    nvgBeginPath(mVg);
    
    BL_FLOAT x0 = 0.0;
    for (int i = 0; i < points->GetSize(); i ++)
    {        
        // Normalize
        BL_FLOAT x = ((BL_FLOAT)i)/points->GetSize();
        
        if (mXdBScale)
            x = BLUtils::NormalizedXTodB(x, mMinXdB, mMaxXdB);
        
        // Scale for the interface
        x = x * width;
        
        BL_FLOAT val = points->Get()[i];
        if (curve->mYdBScale)
            val = BLUtils::NormalizedYTodB(val, curve->mMinYdB, curve->mMaxYdB);
        
        BL_FLOAT y = val * mAutoAdjustFactor * mYScaleFactor * height;
        
        if (i == 0)
        {
            x0 = x;
            
            nvgMoveTo(mVg, x0 - OFFSET, 0 - OFFSET);
            nvgLineTo(mVg, x - OFFSET, y);
        }
        
        nvgLineTo(mVg, x, y);
        
        if (i >= points->GetSize() - 1)
            // Close
        {
            nvgLineTo(mVg, x + OFFSET, y);
            nvgLineTo(mVg, x + OFFSET, 0 - OFFSET);
            
            nvgClosePath(mVg);
        }
    }
    
    nvgFillColor(mVg, nvgRGBA(sFillColor[0], sFillColor[1], sFillColor[2], sFillColor[3]));
	nvgFill(mVg);
    
    nvgStrokeColor(mVg, nvgRGBA(sStrokeColor[0], sStrokeColor[1], sStrokeColor[2], sStrokeColor[3]));
    nvgStrokeWidth(mVg, lineWidth);
    nvgStroke(mVg);

    nvgRestore(mVg);
    
    mDirty = true;
}

void
GraphControl2::DrawLineCurveSV(BL_FLOAT val, CurveColor color, BL_FLOAT alpha, BL_FLOAT lineWidth,
                              GraphCurve *curve)
{
    int width = this->mRECT.W();
    int height = this->mRECT.H();
    
    nvgSave(mVg);
    nvgStrokeWidth(mVg, lineWidth);
    
    int sColor[4] = { (int)(color[0]*255), (int)(color[1]*255),
                      (int)(color[2]*255), (int)(alpha*255) };
    SWAP_COLOR(sColor);
    
    nvgStrokeColor(mVg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));
    
    nvgBeginPath(mVg);
    
    BL_FLOAT x0 = 0;
    BL_FLOAT x1 = width;
    
    if (curve->mYdBScale)
        val = BLUtils::NormalizedYTodB(val, curve->mMinYdB, curve->mMaxYdB);

    BL_FLOAT y = val * mAutoAdjustFactor * height;
    
    nvgMoveTo(mVg, x0, y);
    nvgLineTo(mVg, x1, y);
    
    nvgStroke(mVg);
    nvgRestore(mVg);
    
    mDirty = true;
}

void
GraphControl2::DrawFillCurveSV(BL_FLOAT val, CurveColor color, BL_FLOAT alpha,
                              GraphCurve *curve)
{
    int width = this->mRECT.W();
    int height = this->mRECT.H();
    
    nvgSave(mVg);
    
    int sColor[4] = { (int)(color[0]*255), (int)(color[1]*255),
                      (int)(color[2]*255), (int)(alpha*255) };
    SWAP_COLOR(sColor);
    
    nvgStrokeColor(mVg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));
    
    nvgBeginPath(mVg);
    
    if (curve->mYdBScale)
        val = BLUtils::NormalizedYTodB(val, curve->mMinYdB, curve->mMaxYdB);
    
    BL_FLOAT x0 = 0;
    BL_FLOAT x1 = width;
    BL_FLOAT y0 = 0;
    BL_FLOAT y1 = val * mAutoAdjustFactor * height;
    
    nvgMoveTo(mVg, x0, y0);
    nvgLineTo(mVg, x0, y1);
    nvgLineTo(mVg, x1, y1);
    nvgLineTo(mVg, x1, y0);
    
    nvgClosePath(mVg);
    
    nvgFillColor(mVg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));
	nvgFill(mVg);
    
    nvgStroke(mVg);
    nvgRestore(mVg);
    
    mDirty = true;
}

void
GraphControl2::AutoAdjust()
{
    // First, compute the maximul value of all the curves
    BL_FLOAT max = -1e16;
    for (int i = 0; i < mNumCurves; i++)
    {
        GraphCurve *curve = mCurves[i];
        
        for (int j = curve->mValues.GetSize(); j >= 0; j--)
        {
            BL_FLOAT val = curve->mValues.Get()[j];
            
            if (val > max)
                max = val;
        }
    }
    
    // Compute the scale factor
    
    // keep a margin, do not scale to the maximum
    BL_FLOAT margin = 0.25;
    BL_FLOAT factor = 1.0;
    
    if (max > 0.0)
         factor = (1.0 - margin)/max;
    
    // Then smooth the factor
    mAutoAdjustParamSmoother.SetNewValue(factor);
    mAutoAdjustParamSmoother.Update();
    factor = mAutoAdjustParamSmoother.GetCurrentValue();
    
    mAutoAdjustFactor = factor;
}

BL_FLOAT
GraphControl2::MillisToPoints(long long int elapsed, int sampleRate, int numSamplesPoint)
{
    BL_FLOAT numSamples = (((BL_FLOAT)elapsed)/1000.0)*sampleRate;
    
    BL_FLOAT numPoints = numSamples/numSamplesPoint;
    
    return numPoints;
}

void
GraphControl2::InitFont(const char *fontPath)
{
#ifndef WIN32
    nvgCreateFont(mVg, "font", fontPath);

	mFontInitialized = true;
#else //  On windows, resources are not external files 
	
	// Load the resource in memory, then create the nvg font

	IGraphicsWin *graphics = (IGraphicsWin *)GetGUI();
	if (graphics == NULL)
		return;

	HINSTANCE instance = graphics->GetHInstance();
	if (instance == NULL)
		return;

	HRSRC fontResource = ::FindResource(instance, MAKEINTRESOURCE(FONT_ID), RT_RCDATA);

	HMODULE module = instance;

	unsigned int fontResourceSize = ::SizeofResource(module, fontResource);
	HGLOBAL fontResourceData = ::LoadResource(module, fontResource);
	void* pBinaryData = ::LockResource(fontResourceData);

	if (pBinaryData == NULL)
		return;

	unsigned char* data = (unsigned char*)malloc(fontResourceSize);
	int ndata = fontResourceSize;
	memcpy(data, fontResourceData, ndata);

	nvgCreateFontMem(mVg, "font", data, ndata, 1);

	mFontInitialized = true;
#endif
}

void
GraphControl2::DrawText(BL_FLOAT x, BL_FLOAT y, BL_FLOAT fontSize,
                        const char *text, int color[4],
                        int halign)
{
    nvgSave(mVg);
    
    nvgFontSize(mVg, fontSize);
	nvgFontFace(mVg, "font");
    nvgFontBlur(mVg, 0);
	nvgTextAlign(mVg, halign | NVG_ALIGN_BOTTOM);
    
    int sColor[4] = { color[0], color[1], color[2], color[3] };
    SWAP_COLOR(sColor);
    
    nvgFillColor(mVg, nvgRGBA(sColor[0], sColor[1], sColor[2], sColor[3]));
    
	nvgText(mVg, x, y, text, NULL);
    
    nvgRestore(mVg);
}

// Unused
void
GraphControl2::SwapChannels(unsigned int *data, int numPixels)
{
    for (int i = 0; i < numPixels; i++)
    {
        unsigned int srcPixel = data[i];
        
        unsigned char *srcRGBA = (unsigned char *)&srcPixel;
        
        // Swap
        unsigned char dstRGBA[4] = { srcRGBA[3], srcRGBA[2], srcRGBA[1], srcRGBA[0] };
        
        unsigned int dstPixel = *(unsigned int *)dstRGBA;
        
        data[i] = dstPixel;
    }
}

void
GraphControl2::DrawGL()
{
    IPlugBase::IMutexLock lock(mPlug);
    
	// On Windows, we need lazy evaluation, because we need HInstance and IGraphics
	if (!mFontInitialized)
		InitFont(NULL);
    
    GLContext2 *context = GLContext2::Get();
    context->Bind();
    
    nvgluBindFramebuffer(mFbo);
    
    int width = this->mRECT.W();
    int height = this->mRECT.H();
    
    nvgImageSize(mVg, mFbo->image, &width, &height);
    
    glViewport(0, 0, width, height);
    
    int rectWidth = this->GetRECT()->W();
    int rectHeight = this->GetRECT()->H();
    
    // Set pixel ratio to 1.0
    // Otherwise, fonts will be blurry,
    // and lines too I guess...
    nvgBeginFrame(mVg, rectWidth, rectHeight, 1.0);
    
    int clearColor[4] = { (int)(mClearColor[0]*255), (int)(mClearColor[1]*255),
                          (int)(mClearColor[2]*255), (int)(mClearColor[3]*255) };
    SWAP_COLOR(clearColor);
    
	glClearColor(clearColor[0]/255.0, clearColor[1]/255.0,
                 clearColor[2]/255.0, clearColor[3]/255.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    if (mAutoAdjustFlag)
    {
        AutoAdjust();
    }
    
    DrawAxis(true);
    
    nvgSave(mVg);
    
    DrawCurves();
    
    nvgRestore(mVg);
    
    DrawAxis(false);
    
    nvgEndFrame(mVg);
    
#ifdef WIN32
    glReadPixels(0, 0, this->mRECT.W(), this->mRECT.H(), GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)mPixels);
#endif
    
#ifdef __APPLE__
    glReadPixels(0, 0, this->mRECT.W(), this->mRECT.H(), GL_ABGR_EXT, GL_UNSIGNED_BYTE, (GLubyte *)mPixels);
#endif
    
    nvgluBindFramebuffer(NULL);
    
    context->Unbind();
    
    // Causes performance problems
    // So we swap colors at the beginning and draw with swapped colors
#ifdef __APPLE__
    //SwapChannels((unsigned int *)mPixels, this->mRECT.W()*this->mRECT.H());
#endif
}

void
GraphControl2::InitGL()
{
    if (mGLInitialized)
        return;
    
    // "Init" GL
    GLContext2::Catch();
    
    GLContext2 *context = GLContext2::Get();
    context->Bind();

    mGLInitialized = true;
}

void
GraphControl2::ExitGL()
{
    GLContext2 *context = GLContext2::Get();
    //context->Bind();

    //nvgluBindFramebuffer(NULL);

    context->Unbind();
    GLContext2::Release();
}

void
GraphControl2::InitNanoVg()
{
    GLContext2 *context = GLContext2::Get();
    context->Bind();
    
	mVg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	if (mVg == NULL)
		return;
    
	mFbo = nvgluCreateFramebuffer(mVg, this->mRECT.W(), this->mRECT.H(), NVG_IMAGE_NEAREST);
	
    InitFont(mFontPath.Get());
}

void
GraphControl2::ExitNanoVg()
{
    GLContext2 *context = GLContext2::Get();
    //context->Unbind();
	context->Bind();
	
	nvgluBindFramebuffer(NULL); //
	
	nvgluDeleteFramebuffer(mFbo);
	
    nvgDeleteGL2(mVg);
}
