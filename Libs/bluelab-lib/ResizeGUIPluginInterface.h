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

class ResizeGUIPluginInterface
{
public:
    ResizeGUIPluginInterface(Plugin *plug) { mPlug = plug; }
    virtual ~ResizeGUIPluginInterface() {}
    
    Plugin *GetPlug() { return mPlug; }
         
    virtual void PreResizeGUI() = 0;
    virtual void PostResizeGUI() = 0;
    
protected:
    Plugin *mPlug;
};

#endif /* ResizeGUIPluginInterface_h */
