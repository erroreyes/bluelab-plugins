/* Copyright (C) 2022 Nicolas Dittlo <deadlab.plugins@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this software; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
//
//  SASViewerRender4.h
//  BL-Waves
//
//  Created by applematuer on 10/13/18.
//
//

#ifndef __BL_SASViewer__SASViewerRender4__
#define __BL_SASViewer__SASViewerRender4__

#ifdef IGRAPHICS_NANOVG

#include <vector>
#include <deque>
using namespace std;

#include <GraphControl12.h>
#include <LinesRender2.h>
#include <SASViewerPluginInterface.h>
//#include <SASViewerProcess3.h>
#include <SASViewerProcess4.h>

// Sides
//#define MAX_ANGLE_0 70.0

// Set to 90 for debugging
#define MAX_CAM_ANGLE_0 90.0

// Above
#define MAX_CAM_ANGLE_1 90.0 //70.0

// Below

// Almost horizontal (a little above)
#define MIN_CAM_ANGLE_1 15.0

class Axis3D;
class SASViewerRender4 : public GraphCustomControl
{
public:
    SASViewerRender4(SASViewerPluginInterface *plug,
                    GraphControl12 *graphControl,
                    BL_FLOAT sampleRate, int bufferSize);
    
    virtual ~SASViewerRender4();
    
    void SetGraph(GraphControl12 *graphControl);
    
    void Clear();
    
    virtual void AddData(SASViewerProcess4::Mode mode,
                          const WDL_TypedBuf<BL_FLOAT> &data);
    virtual void AddPoints(SASViewerProcess4::Mode mode,
                           const vector<LinesRender2::Point> &points);

    virtual void SetLineMode(SASViewerProcess4::Mode mode,
                             LinesRender2::Mode lineMode);
    
    // Control
    virtual void OnMouseDown(float x, float y, const IMouseMod &mod) override;
    virtual void OnMouseUp(float x, float y, const IMouseMod &mod) override;
    virtual void OnMouseDrag(float x, float y, float dX, float dY,
                             const IMouseMod &mod) override;
    virtual void OnMouseDblClick(float x, float y, const IMouseMod &mod) override;
    virtual void OnMouseWheel(float x, float y,
                              const IMouseMod &mod, float d) override;
    
    // Parameters
    virtual void SetSpeed(BL_FLOAT speed);
    virtual void SetDensity(BL_FLOAT density);
    virtual void SetScale(BL_FLOAT scale);
    
    // For parameter sent from plug (save state and automation)
    void SetCamAngle0(BL_FLOAT angle);
    void SetCamAngle1(BL_FLOAT angle);
    void SetCamFov(BL_FLOAT angle);
    
    //
    void SetMode(SASViewerProcess4::Mode mode);
    
    // Used for rendering tracked partials
    int GetNumSlices();
    int GetSpeed();
    
    void SetAdditionalLines(SASViewerProcess4::Mode mode,
                            const vector<LinesRender2::Line> &lines,
                            BL_FLOAT lineWidth);
    void ClearAdditionalLines();
    void ShowAdditionalLines(SASViewerProcess4::Mode mode, bool flag);
    
    void SetAdditionalPoints(SASViewerProcess4::Mode mode,
                             const vector<LinesRender2::Line> &lines,
                             BL_FLOAT lineWidth);
    void ClearAdditionalPoints();
    void ShowAdditionalPoints(SASViewerProcess4::Mode mode, bool flag);
    
    // For debugging
    void DBG_SetNumSlices(int numSlices);
    
protected:
    void DataToPoints(vector<LinesRender2::Point> *points,
                      const WDL_TypedBuf<BL_FLOAT> &data);
    
    //
    SASViewerPluginInterface *mPlug;
    
    GraphControl12 *mGraph;
    
    LinesRender2 *mLinesRenders[SASViewerProcess4::NUM_MODES];
    
    Axis3D *mFreqsAxis;
    
    // Selection
    bool mMouseIsDown;
    float mPrevDrag[2];
    
    // Used to detect pure mouse up, without drag
    bool mPrevMouseDrag;
    
    // Rotation
    BL_FLOAT mCamAngle0;
    BL_FLOAT mCamAngle1;
    
    BL_FLOAT mScale;
    
    BL_FLOAT mSampleRate;
    int mBufferSize;
    
    //
    unsigned long long int mAddNum;
    
    SASViewerProcess4::Mode mCurrentMode;
};

#endif // IGRAPHICS_NANOVG

#endif /* defined(__BL_SASViewer__SASViewerRender4__) */
