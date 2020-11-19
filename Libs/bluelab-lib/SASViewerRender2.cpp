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
#include <BLDebug.h>

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
    
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
    {
        mLinesRenders[i] = new LinesRender2();
        mLinesRenders[i]->SetMode(LinesRender2::LINES_FREQ);
    }
    
    mCurrentMode = SASViewerProcess2::TRACKING;
    
    SetGraph(graphControl);
    
    mMouseIsDown = false;
    mPrevDrag[0] = 0;
    mPrevDrag[1] = 0;
    
    mPrevMouseDrag = false;
    
    // Rotation of the camera
    mCamAngle0 = 0.0;
    mCamAngle1 = MIN_CAM_ANGLE_1;
    
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
    {
        mLinesRenders[i]->SetCameraAngles(mCamAngle0, mCamAngle1);
    }
    
    mAddNum = 0;
}

SASViewerRender2::~SASViewerRender2()
{
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        delete mLinesRenders[i];
}

void
SASViewerRender2::SetGraph(GraphControl12 *graphControl)
{
    mGraph = graphControl;
    
    if (mGraph != NULL)
    {
        mGraph->AddCustomControl(this);
        mGraph->AddCustomDrawer(mLinesRenders[(int)mCurrentMode]);
    }
}

void
SASViewerRender2::Clear()
{
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->ClearSlices();

    mGraph->SetDataChanged();
}

void
SASViewerRender2::AddMagns(const WDL_TypedBuf<BL_FLOAT> &magns,
                           SASViewerProcess2::Mode mode)
{
    if (magns.GetSize() == 0)
        return;
    
    vector<LinesRender2::Point> points;
    MagnsToPoints(&points, magns);
    
    mLinesRenders[(int)mode]->AddSlice(points);
    
    mAddNum++;
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::AddPoints(const vector<LinesRender2::Point> &points,
                            SASViewerProcess2::Mode mode)
{
    mLinesRenders[(int)mode]->AddSlice(points);
    
    mAddNum++;
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetLineMode(LinesRender2::Mode mode)
{
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->SetMode(mode);
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
    
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->SetCameraAngles(mCamAngle0, mCamAngle1);
    
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
    
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
            mLinesRenders[i]->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    // Reset the fov
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->ResetZoom();

    mGraph->SetDataChanged();
    
    mPlug->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    BL_FLOAT fov = mLinesRenders[0]->GetCameraFov();
    mPlug->SetCameraFov(fov);
}

void
SASViewerRender2::OnMouseWheel(float x, float y,
                              const IMouseMod &mod, BL_FLOAT d)
{
#define WHEEL_ZOOM_STEP 0.025
    
    BL_FLOAT zoomChange = 1.0 + d*WHEEL_ZOOM_STEP;
    
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->ZoomChanged(zoomChange);
    
    mGraph->SetDataChanged();
    
    BL_FLOAT angle = mLinesRenders[0]->GetCameraFov();
    mPlug->SetCameraFov(angle);
}

void
SASViewerRender2::SetSpeed(BL_FLOAT speed)
{
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->SetSpeed(speed);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetDensity(BL_FLOAT density)
{
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->SetDensity(density);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetScale(BL_FLOAT scale)
{
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->SetScale(scale);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetCamAngle0(BL_FLOAT angle)
{
    mCamAngle0 = angle;
    
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetCamAngle1(BL_FLOAT angle)
{
    mCamAngle1 = angle;
    
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->SetCameraAngles(mCamAngle0, mCamAngle1);
    
    mGraph->SetDataChanged();
}

void
SASViewerRender2::SetCamFov(BL_FLOAT angle)
{
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->SetCameraFov(angle);
    
    mGraph->SetDataChanged();
}

int
SASViewerRender2::GetNumSlices()
{
    int numSlices = (int)mLinesRenders[0]->GetNumSlices();
    
    return numSlices;
}

void
SASViewerRender2::SetMode(SASViewerProcess2::Mode mode)
{
    mGraph->RemoveCustomDrawer(mLinesRenders[mCurrentMode]);
    
    mCurrentMode = mode;
    
    mGraph->AddCustomDrawer(mLinesRenders[mCurrentMode]);
}

int
SASViewerRender2::GetSpeed()
{
    int speed = mLinesRenders[0]->GetSpeed();
    
    return speed;
}

void
SASViewerRender2::SetAdditionalLines(const vector<LinesRender2::Line> &lines,
                                     BL_FLOAT lineWidth,
                                     SASViewerProcess2::Mode mode)
{
    mLinesRenders[(int)mode]->SetAdditionalLines(lines, lineWidth);
}

void
SASViewerRender2::ClearAdditionalLines()
{
    for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
        mLinesRenders[i]->ClearAdditionalLines();
}

void
SASViewerRender2::ShowTrackingLines(bool flag, SASViewerProcess2::Mode mode)
{
    mLinesRenders[(int)mode]->ShowAdditionalLines(flag);
}

void
SASViewerRender2::DBG_SetNumSlices(int numSlices)
{
     for (int i = 0; i < (int)SASViewerProcess2::NUM_MODES; i++)
     {
         mLinesRenders[i]->SetNumSlices(numSlices);
         mLinesRenders[i]->DBG_ForceDensityNumSlices();
     }
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

#endif // IGRAPHICS_NANOVG
