//
//  SASViewerRender2.cpp
//  BL-Waves
//
//  Created by applematuer on 10/13/18.
//
//

#ifdef IGRAPHICS_NANOVG

#include <LinesRender.h>
#include <Axis3D.h>

#include <BLUtils.h>

// Include for flag for partials debug
#include <SASViewerProcess.h>

#include "SASViewerRender2.h"

// Axis drawing
#define AXIS_OFFSET_Z 0.06

// Artificially modify the coeff, to increase the spread on the grid
#define MEL_COEFF 4.0

SASViewerRender2::SASViewerRender2(SASViewerPluginInterface *plug,
                                 GraphControl12 *graphControl,
                                 BL_FLOAT sampleRate, int bufferSize)
{
    mPlug = plug;
    
    mSampleRate = sampleRate;
    mBufferSize = bufferSize;
    
    mLinesRender = new LinesRender2();
    mLinesRender->SetMode(LinesRender2::LINES_FREQ);
    
    SetGraph(graphControl);
    
    mMouseIsDown = false;
    mPrevDrag[0] = 0;
    mPrevDrag[1] = 0;
    
    mPrevMouseDrag = false;
    
    // Rotation of the camera
    mCamAngle0 = 0.0;
    mCamAngle1 = MIN_CAM_ANGLE_1;
    
    mLinesRender->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    mAddNum = 0;
}

SASViewerRender2::~SASViewerRender2()
{
    delete mLinesRender;
}

void
SASViewerRender2::SetGraph(GraphControl12 *graphControl)
{
    mGraph = graphControl;
    
    if (mGraph != NULL)
    {
        mGraph->AddCustomControl(this);
        mGraph->AddCustomDrawer(mLinesRender);
    }
}

void
SASViewerRender2::Clear()
{
    mLinesRender->ClearSlices();

    mGraph->SetDataChanged();
}

void
SASViewerRender2::AddMagns(const WDL_TypedBuf<BL_FLOAT> &magns)
{
    if (magns.GetSize() == 0)
        return;
    
    vector<LinesRender2::Point> points;
    MagnsToPoints(&points, magns);
    
    mLinesRender->AddSlice(points);
    
    mAddNum++;
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::AddPoints(const vector<LinesRender2::Point> &points)
{
    mLinesRender->AddSlice(points);
    
    mAddNum++;
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetLineMode(LinesRender2::Mode mode)
{
    mLinesRender->SetMode(mode);
}

void
SASViewerRender2::OnMouseDown(float x, float y, const IMouseMod &mod)
{
    mMouseIsDown = true;
    
    mPrevDrag[0] = x;
    mPrevDrag[1] = y;
    
    mPrevMouseDrag = false;
}

void
SASViewerRender2::OnMouseUp(float x, float y, const IMouseMod &mod)
{
    if (!mMouseIsDown)
        return;
    
    mMouseIsDown = false;
}

void
SASViewerRender2::OnMouseDrag(float x, float y, float dX, float dY,
                              const IMouseMod &mod)
{
    if (mod.A)
    // Alt-drag => zoom
    {
        if (!mPrevMouseDrag)
        {
            mPrevMouseDrag = true;
            
            mPrevDrag[0] = y;
            
            return;
        }
        
#define DRAG_WHEEL_COEFF 0.2
        
        BL_FLOAT dY = y - mPrevDrag[0];
        mPrevDrag[0] = y;
        
        dY *= -1.0;
        dY *= DRAG_WHEEL_COEFF;
        
        OnMouseWheel(mPrevDrag[0], mPrevDrag[1], mod, dY);
        
        return;
    }
    
    mPrevMouseDrag = true;
    
    // Move camera
    float dragX = x - mPrevDrag[0];
    float dragY = y - mPrevDrag[1];
    
    mPrevDrag[0] = x;
    mPrevDrag[1] = y;
    
    mCamAngle0 += dragX;
    mCamAngle1 += dragY;
    
    // Bounds
    if (mCamAngle0 < -MAX_CAM_ANGLE_0)
        mCamAngle0 = -MAX_CAM_ANGLE_0;
    if (mCamAngle0 > MAX_CAM_ANGLE_0)
        mCamAngle0 = MAX_CAM_ANGLE_0;
    
    if (mCamAngle1 < MIN_CAM_ANGLE_1)
        mCamAngle1 = MIN_CAM_ANGLE_1;
    if (mCamAngle1 > MAX_CAM_ANGLE_1)
        mCamAngle1 = MAX_CAM_ANGLE_1;
    
    mLinesRender->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    // Camera changed
    //
    // Without that, the camera point of view is not modified if
    // the sound is not playing
    //mGraph->SetDirty(true);
    mGraph->SetDataChanged();
    
    mPlug->SetCameraAngles(mCamAngle0, mCamAngle1);
}

void
SASViewerRender2::OnMouseDblClick(float x, float y, const IMouseMod &mod)
{
    // Reset the view
    mCamAngle0 = 0.0;
    mCamAngle1 = MIN_CAM_ANGLE_1;
    
    mLinesRender->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    // Reset the fov
    mLinesRender->ResetZoom();

    mGraph->SetDataChanged();
    
    mPlug->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    BL_FLOAT fov = mLinesRender->GetCameraFov();
    mPlug->SetCameraFov(fov);
}

void
SASViewerRender2::OnMouseWheel(float x, float y,
                              const IMouseMod &mod, BL_FLOAT d)
{
#define WHEEL_ZOOM_STEP 0.025
    
    BL_FLOAT zoomChange = 1.0 + d*WHEEL_ZOOM_STEP;
    
    mLinesRender->ZoomChanged(zoomChange);
    
    mGraph->SetDataChanged();
    
    BL_FLOAT angle = mLinesRender->GetCameraFov();
    mPlug->SetCameraFov(angle);
}

void
SASViewerRender2::SetSpeed(BL_FLOAT speed)
{
    mLinesRender->SetSpeed(speed);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetDensity(BL_FLOAT density)
{
    mLinesRender->SetDensity(density);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetScale(BL_FLOAT scale)
{
    mLinesRender->SetScale(scale);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetCamAngle0(BL_FLOAT angle)
{
    mCamAngle0 = angle;
    
    mLinesRender->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetCamAngle1(BL_FLOAT angle)
{
    mCamAngle1 = angle;
    
    mLinesRender->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetCamFov(BL_FLOAT angle)
{
    mLinesRender->SetCameraFov(angle);
    
    mGraph->SetDataChanged();
}

int
SASViewerRender2::GetNumSlices()
{
    int numSlices = (int)mLinesRender->GetNumSlices();
    
    return numSlices;
}

int
SASViewerRender2::GetSpeed()
{
    int speed = mLinesRender->GetSpeed();
    
    return speed;
}

void
SASViewerRender2::SetAdditionalLines(const vector<LinesRender2::Line> &lines,
                                     BL_FLOAT lineWidth)
{
    mLinesRender->SetAdditionalLines(lines, lineWidth);
}

void
SASViewerRender2::ClearAdditionalLines()
{
    mLinesRender->ClearAdditionalLines();
}

void
SASViewerRender2::ShowAdditionalLines(bool flag)
{
    mLinesRender->ShowAdditionalLines(flag);
}

void
SASViewerRender2::DBG_SetNumSlices(int numSlices)
{
    mLinesRender->SetNumSlices(numSlices);
    mLinesRender->DBG_ForceDensityNumSlices();
}

void
SASViewerRender2::MagnsToPoints(vector<LinesRender2::Point> *points,
                               const WDL_TypedBuf<BL_FLOAT> &magns)
{
    if (magns.GetSize() == 0)
        return;
    
    // Convert to points
    points->resize(magns.GetSize());
    
    // Optim
    BL_FLOAT xCoeff = 0.0;
    if (magns.GetSize() > 0)
    {
        if (magns.GetSize() <= 1)
            xCoeff = 1.0/magns.GetSize();
        else
            xCoeff = 1.0/(magns.GetSize() - 1);
    }
    
    // Loop
    for (int i = 0; i < magns.GetSize(); i++)
    {
        BL_FLOAT magn = magns.Get()[i];
        
        LinesRender2::Point &p = (*points)[i];
        
        p.mX = xCoeff*i - 0.5;
        p.mY = magn;
        
        // Fill with dummy Z (to avoid undefined value)
        p.mZ = 0.0;
        
        // Fill with dummy color (to avoid undefined value)
        p.mR = 0;
        p.mG = 0;
        p.mB = 0;
        p.mA = 0;
    }
}

BL_FLOAT
SASViewerRender2::FreqToMelNorm(BL_FLOAT freq)
{
    // Convert to Mel
    BL_FLOAT hzPerBin = mSampleRate/(mBufferSize/2.0);
    hzPerBin *= MEL_COEFF;
    
    // Hack: something is not really correct here...
    freq *= 2.0;
    
    BL_FLOAT result = BLUtils::FreqToMelNorm((BL_FLOAT)(freq*MEL_COEFF), hzPerBin, mBufferSize);
    
    return result;
}

#endif // IGRAPHICS_NANOVG
