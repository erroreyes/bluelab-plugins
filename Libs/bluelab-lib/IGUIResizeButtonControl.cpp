//
//  IGUIResizeButtonControl.cpp
//  BL-GhostViewer-macOS
//
//  Created by applematuer on 10/29/20.
//
//

#include <BLUtils.h>

#include "IGUIResizeButtonControl.h"

void
IGUIResizeButtonControl::OnMouseDown(float x, float y, const IMouseMod &mod)
{
    // Disable Alt + click on gui resize buttons
    // FIX: that made the graph disappear
    if (mod.A)
        ((IMouseMod &)mod).A = false;
    
    mIsMouseClicking = true;
    
    double prevValue = GetValue();
    
    IRolloverButtonControl::OnMouseDown(x, y, mod);
    
    // Check to not resize to identity if we re-clicked on the button already active
    if (std::fabs(GetValue() - prevValue) > BL_EPS)
    {
#if 0 // #bluelab: TODO: implemente this!
        mPlug->PreResizeGUI();
        mPlug->ResizeGUI(mResizeWidth, mResizeHeight);
#endif
    }
    
    mIsMouseClicking = false;
}
