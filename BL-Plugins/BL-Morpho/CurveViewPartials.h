#ifndef CURVE_VIEW_PARTIALS_H
#define CURVE_VIEW_PARTIALS_H

#include <CurveView.h>

class CurveViewPartials : public CurveView
{
public:
    CurveViewPartials(int maxNumData = CURVE_VIEW_DEFAULT_NUM_DATA);
    virtual ~CurveViewPartials();

protected:
    void DrawCurve(NVGcontext *vg, int width, int height) override;

    void DrawPartials(NVGcontext *vg, int width, int height);
    void DrawPartialsCurve(NVGcontext *vg, int width, int height);

    void DrawBottomLine(NVGcontext *vg, int width, int height);
        
    //
    IColor mPartialsCurveColor;

    float mPartialsCurveLineWidth;
};

#endif
