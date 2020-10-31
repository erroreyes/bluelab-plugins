//
//  IGUIResizeButtonControl.hpp
//  BL-GhostViewer-macOS
//
//  Created by applematuer on 10/29/20.
//
//

#ifndef IGUIResizeButtonControl_h
#define IGUIResizeButtonControl_h

#include <IRolloverButtonControl.h>
#include <IPlugConstants.h>

using namespace iplug;
using namespace iplug::igraphics;

// Must create a specific class
// (otherwise, if used parameter change, that should not be
// the right thread, and it crashes)
//
// Version 2 => with rollover
//
// From IGUIResizeButtonControl (iPlug1)
class IGUIResizeButtonControl : public IRolloverButtonControl
{
public:
    IGUIResizeButtonControl(Plugin *plug,
                            float x, float y,
                            const IBitmap &bitmap,
                            int paramIdx,
                            int resizeWidth, int resizeHeight,
                            EBlend blend = EBlend::Default)
    : IRolloverButtonControl(x, y, bitmap, paramIdx, false, blend)
    {
        mPlug = plug;
        
        mResizeWidth = resizeWidth;
        mResizeHeight = resizeHeight;
        
        mIsMouseClicking = false;
    }
    
    IGUIResizeButtonControl(Plugin *plug,
                            float x, float y,
                            const IBitmap &bitmap,
                            int resizeWidth, int resizeHeight,
                            EBlend blend = EBlend::Default)
    : IRolloverButtonControl(x, y, bitmap, kNoParameter, blend)
    {
        mPlug = plug;
        
        mResizeWidth = resizeWidth;
        mResizeHeight = resizeHeight;
    }
    
    virtual ~IGUIResizeButtonControl() {}
    
    virtual void OnMouseDown(float x, float y, const IMouseMod &mMod) override;
    
    bool IsMouseClicking()
    {
        return mIsMouseClicking;
    }
    
    // Disable double click
    // It was not consistent to use double click on a series of gui resize buttons
    // And made a bug: no button hilighted, and if we reclicked, no graph anymore
    void OnMouseDblClick(float x, float y, const IMouseMod &mod) override {}
    
protected:
    Plugin *mPlug;
    
    int mResizeWidth;
    int mResizeHeight;
    
    bool mIsMouseClicking;
};

#endif /* IGUIResizeButtonControl_h */
