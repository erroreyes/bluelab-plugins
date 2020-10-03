//
//  DUETCustomControl.h
//  BL-Panogram
//
//  Created by applematuer on 10/21/19.
//
//

#ifndef __BL_Panogram__DUETCustomControl__
#define __BL_Panogram__DUETCustomControl__

#include <GraphControl11.h>
#include <DUETPlugInterface.h>

using namespace iplug;

class SpectrogramDisplayScroll;

class DUETCustomControl : public GraphCustomControl
{
public:
    DUETCustomControl(DUETPlugInterface *plug);
    
    virtual ~DUETCustomControl() {}
    
    void Reset();
    
    void OnMouseDown(float x, float y, const IMouseMod &mod);
    void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod &mod);
    
    bool OnKeyDown(float x, float y, int key, const IMouseMod &mod);
    
protected:
    DUETPlugInterface *mPlug;
};

#endif /* defined(__BL_Panogram__DUETCustomControl__) */
