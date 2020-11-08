//
//  WavesPluginInterface.h
//  UST-macOS
//
//  Created by applematuer on 9/26/20.
//
//

#ifndef ResizeGUIPluginInterface_h
#define ResizeGUIPluginInterface_h

#include "IPlug_include_in_plug_hdr.h"

using namespace iplug;
using namespace iplug::igraphics;

class IGUIResizeButtonControl;
class GraphControl11;

class ResizeGUIPluginInterface
{
public:
    ResizeGUIPluginInterface(Plugin *plug) { mPlug = plug; }
    virtual ~ResizeGUIPluginInterface() {}
         
    virtual void PreResizeGUI(int newGUIWidth, int newGUIHeight) = 0;
    virtual void PostResizeGUI() = 0;
    
    virtual void GetNewGUISize(int guiSizeIdx,
                               int *newGUIWidth,
                               int *newGUIHeight) = 0;
    
    void ApplyGUIResize(int guiSizeIdx);
    
protected:
    void GUIResizeParamChange(int paramNum,
                              int params[], IGUIResizeButtonControl *buttons[],
                              int numParams);
    
    void GUIResizePreResizeGUI(IGUIResizeButtonControl *buttons[],
                               int numButtons);
    
    void GUIResizeComputeOffsets(int defaultGUIWidth, int defaultGUIHeight,
                                 int newGUIWidth, int newGUIHeight,
                                 int *offsetX, int *offsetY);
    
    //
    Plugin *mPlug;
};

#endif /* ResizeGUIPluginInterface_h */
