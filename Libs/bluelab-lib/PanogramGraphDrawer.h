//
//  PanogramGraphDrawer.h
//  BL-Pano
//
//  Created by applematuer on 8/9/19.
//
//

#ifndef __BL_Pano__PanogramGraphDrawer__
#define __BL_Pano__PanogramGraphDrawer__

#ifdef IGRAPHICS_NANOVG

#include <GraphControl11.h>

class PanogramGraphDrawer : public GraphCustomDrawer
{
public:
    PanogramGraphDrawer();
    
    virtual ~PanogramGraphDrawer();
    
    // Draw after everything
    void PostDraw(NVGcontext *vg, int width, int height);
};

#endif // IGRAPHICS_NANOVG

#endif /* defined(__BL_Pano__PanogramGraphDrawer__) */
