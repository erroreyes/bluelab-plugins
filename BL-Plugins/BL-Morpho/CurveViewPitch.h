#ifndef CURVE_VIEW_PITCH_H
#define CURVE_VIEW_PITCH_H

#include <CurveView.h>

class CurveViewPitch : public CurveView
{
public:
    CurveViewPitch(int maxNumData = CURVE_VIEW_DEFAULT_NUM_DATA);
    virtual ~CurveViewPitch();

protected:
    void DrawCurve(NVGcontext *vg, int width, int height) override;
};

#endif
