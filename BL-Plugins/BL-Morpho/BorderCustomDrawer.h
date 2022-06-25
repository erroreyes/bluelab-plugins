#ifndef BORDER_CUSTOM_DRAWER_H
#define BORDER_CUSTOM_DRAWER_H

#include <GraphControl12.h>

class BorderCustomDrawer : public GraphCustomDrawer
{
 public:
    BorderCustomDrawer(float borderWidth, IColor borderColor);
    
    virtual ~BorderCustomDrawer();

    void PostDraw(NVGcontext *vg, int width, int height) override;

    bool IsOwnedByGraph() override { return true; }

 protected:
    float mBorderWidth;

    IColor mBorderColor;
};

// A CustomDrawer that adds a border around a graph

#endif
