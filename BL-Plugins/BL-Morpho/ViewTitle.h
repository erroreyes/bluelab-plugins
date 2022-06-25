#ifndef VIEW_TITLE_H
#define VIEW_TITLE_H

#ifdef IGRAPHICS_NANOVG

#include <GraphControl12.h>

#define TITLE_SIZE 256

class ViewTitle : public GraphCustomDrawer
{
 public:
    ViewTitle();
    virtual ~ViewTitle();

    void SetPos(float x, float y);

    void SetTitle(const char *title);
    
    void PostDraw(NVGcontext *vg, int width, int height) override;

protected:    
    //
    char mTitle[TITLE_SIZE];
    
    float mX;
    float mY;
    
    // Style
    IColor mTitleColor;
    IColor mTitleColorDark;
};

#endif // IGRAPHICS_NANOVG

#endif
