#include <BorderCustomDrawer.h>

BorderCustomDrawer::BorderCustomDrawer(float borderWidth,
                                       IColor borderColor)
{
    mBorderWidth = borderWidth;

    mBorderColor = borderColor;
}
    
BorderCustomDrawer::~BorderCustomDrawer() {}

void
BorderCustomDrawer::PostDraw(NVGcontext *vg, int width, int height)
{
    nvgSave(vg);
    nvgReset(vg);
    
    nvgStrokeWidth(vg, mBorderWidth);

    nvgStrokeColor(vg, nvgRGBA(mBorderColor.R,
                               mBorderColor.G,
                               mBorderColor.B,
                               mBorderColor.A));

    nvgBeginPath(vg);

    nvgMoveTo(vg,  0.0 + mBorderWidth*0.5,   0.0 + mBorderWidth*0.5);
    nvgLineTo(vg,  width - mBorderWidth*0.5, 0.0 + mBorderWidth*0.5);
    nvgLineTo(vg,  width - mBorderWidth*0.5, height - mBorderWidth*0.5);
    nvgLineTo(vg,  0.0 + mBorderWidth*0.5, height - mBorderWidth*0.5);
    
    nvgClosePath(vg);
    
    nvgStroke(vg);

    nvgRestore(vg);
}
