//
//  SASViewerRender2.h
//  BL-Waves
//
//  Created by applematuer on 10/13/18.
//
//

#ifndef __BL_SASViewer__SASViewerRender2__
#define __BL_SASViewer__SASViewerRender2__

#ifdef IGRAPHICS_NANOVG

#include <vector>
#include <deque>
using namespace std;

#include <GraphControl12.h>
#include <LinesRender2.h>
#include <SASViewerPluginInterface.h>

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
class SASViewerRender2 : public GraphCustomControl
{
public:
    SASViewerRender2(SASViewerPluginInterface *plug,
                    GraphControl12 *graphControl,
                    BL_FLOAT sampleRate, int bufferSize);
    
    virtual ~SASViewerRender2();
    
    void SetGraph(GraphControl12 *graphControl);
    
    void Clear();
    
    virtual void AddMagns(const WDL_TypedBuf<BL_FLOAT> &magns);
    
    // NEW
    virtual void AddPoints(const vector<LinesRender2::Point> &points);

    virtual void SetLineMode(LinesRender2::Mode mode);
    
    // Control
    virtual void OnMouseDown(float x, float y, const IMouseMod &mod);
    virtual void OnMouseUp(float x, float y, const IMouseMod &mod);
    virtual void OnMouseDrag(float x, float y, float dX, float dY,
                             const IMouseMod &mod);
    virtual void/*bool*/ OnMouseDblClick(float x, float y, const IMouseMod &mod);
    virtual void OnMouseWheel(float x, float y,
                              const IMouseMod &mod, BL_FLOAT d);
    
    // Parameters
    virtual void SetSpeed(BL_FLOAT speed);
    virtual void SetDensity(BL_FLOAT density);
    virtual void SetScale(BL_FLOAT scale);
    
    // For parameter sent from plug (save state and automation)
    void SetCamAngle0(BL_FLOAT angle);
    void SetCamAngle1(BL_FLOAT angle);
    void SetCamFov(BL_FLOAT angle);
    
    // Used for rendering tracked partials
    int GetNumSlices();
    int GetSpeed();
    
    void SetAdditionalLines(const vector<vector<LinesRender2::Point> > &lines,
                            unsigned char color[4], BL_FLOAT lineWidth);
    
    void ClearAdditionalLines();
    
    void ShowAdditionalLines(bool flag);
    
protected:
    void MagnsToPoints(vector<LinesRender2::Point> *points,
                       const WDL_TypedBuf<BL_FLOAT> &magns);
    
    void CreateFreqsAxis();
    
    BL_FLOAT FreqToMelNorm(BL_FLOAT freq);
    
    SASViewerPluginInterface *mPlug;
    
    GraphControl12 *mGraph;
    
    
    LinesRender2 *mLinesRenderWaves;
    LinesRender2 *mLinesRenderPartials;
    
    Axis3D *mFreqsAxis;
    
    // Selection
    bool mMouseIsDown;
    int mPrevDrag[2];
    
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
};

#endif // IGRAPHICS_NANOVG

#endif /* defined(__BL_SASViewer__SASViewerRender2__) */
