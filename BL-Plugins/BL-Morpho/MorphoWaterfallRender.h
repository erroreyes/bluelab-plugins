#ifndef MORPHO_WATERFALL_RENDER_H
#define MORPHO_WATERFALL_RENDER_H

#ifdef IGRAPHICS_NANOVG

#include <vector>
#include <deque>
using namespace std;

#include <GraphControl12.h>
#include <LinesRender2.h>
#include <MorphoWaterfallView.h>

#include <Morpho_defs.h>

// TODO
//#include <SASViewerPluginInterface.h>

// Sides
//#define MAX_ANGLE_0 70.0

// Set to 90 for debugging
#define MAX_CAM_ANGLE_0 90.0

// Above
#define MAX_CAM_ANGLE_1 90.0 //70.0

// Below

// Almost horizontal (a little above)
#define MIN_CAM_ANGLE_1 15.0

// SASViewerRender5 => MorphoWaterfallRender

class Axis3D;
class MorphoWaterfallRender : public GraphCustomControl
{
public:
    MorphoWaterfallRender(GraphControl12 *graphControl);
    
    virtual ~MorphoWaterfallRender();
    
    void SetGraph(GraphControl12 *graphControl);

    void SetView3DListener(View3DPluginInterface *view3DListener);
    
    void Clear();
    
    virtual void AddData(MorphoWaterfallView::DisplayMode mode,
                         const WDL_TypedBuf<BL_FLOAT> &data);
    virtual void AddPoints(MorphoWaterfallView::DisplayMode mode,
                           const vector<LinesRender2::Point> &points);

    virtual void SetLineMode(MorphoWaterfallView::DisplayMode mode,
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
    virtual void SetSpeedMod(int speedMod);
    virtual void SetDensity(BL_FLOAT density);
    virtual void SetScale(BL_FLOAT scale);
    
    // For parameter sent from plug (save state and automation)
    void SetCamAngle0(BL_FLOAT angle);
    void SetCamAngle1(BL_FLOAT angle);
    void SetCamFov(BL_FLOAT angle);
    
    //
    void SetMode(MorphoWaterfallView::DisplayMode mode);
    
    // Used for rendering tracked partials
    int GetNumSlices();
    int GetSpeed();
    
    void SetAdditionalLines(MorphoWaterfallView::DisplayMode mode,
                            const vector<LinesRender2::Line> &lines,
                            BL_FLOAT lineWidth);
    void ClearAdditionalLines();
    void ShowAdditionalLines(MorphoWaterfallView::DisplayMode mode, bool flag);
    
    void SetAdditionalPoints(MorphoWaterfallView::DisplayMode mode,
                             const vector<LinesRender2::Line> &lines,
                             BL_FLOAT lineWidth, bool optimSameColor);
    void ClearAdditionalPoints();
    void ShowAdditionalPoints(MorphoWaterfallView::DisplayMode mode, bool flag);
    
    // For debugging
    void DBG_SetNumSlices(int numSlices);
    
protected:
    void DataToPoints(vector<LinesRender2::Point> *points,
                      const WDL_TypedBuf<BL_FLOAT> &data);
    
    //
    View3DPluginInterface *mView3DListener;
    
    GraphControl12 *mGraph;
    
    LinesRender2 *mLinesRenders[MorphoWaterfallView::NUM_MODES];
    
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
    
    //
    unsigned long long int mAddNum;
    
    MorphoWaterfallView::DisplayMode mCurrentMode;

private:
    vector<LinesRender2::Point> mTmpBuf0;
};

#endif // IGRAPHICS_NANOVG

#endif
