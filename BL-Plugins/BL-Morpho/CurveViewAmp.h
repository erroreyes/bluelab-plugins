#ifndef CURVE_VIEW_AMP_H
#define CURVE_VIEW_AMP_H

#include <CurveView.h>

class CurveViewAmp : public CurveView
{
public:
    CurveViewAmp(int maxNumData = CURVE_VIEW_DEFAULT_NUM_DATA);
    virtual ~CurveViewAmp();

protected:
    void DrawCurve(NVGcontext *vg, int width, int height) override;
};

#endif
