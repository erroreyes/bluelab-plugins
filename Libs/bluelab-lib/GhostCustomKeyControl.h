#ifndef GHOST_CUSTOM_KEY_CONTROL_H
#define GHOST_CUSTOM_KEY_CONTROL_H

#include <GhostPluginInterface.h>

class GhostCustomKeyControl : public KeyCustomControl
{
public:
    GhostCustomKeyControl(GhostPluginInterface *plug);
    
    virtual ~GhostCustomKeyControl() {}
    
    virtual bool OnKeyDown(int x, int y, int key, IMouseMod* pMod);
    
protected:
    GhostPluginInterface *mPlug;
};


#endif
