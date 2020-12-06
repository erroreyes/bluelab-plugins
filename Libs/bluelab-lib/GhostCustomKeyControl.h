#ifndef GHOST_CUSTOM_KEY_CONTROL_H
#define GHOST_CUSTOM_KEY_CONTROL_H

class GhostCustomKeyControl : public KeyCustomControl
{
public:
    GhostCustomKeyControl(Ghost *plug);
    
    virtual ~GhostCustomKeyControl() {}
    
    virtual bool OnKeyDown(int x, int y, int key, IMouseMod* pMod);
    
protected:
    Ghost *mPlug;
};


#endif
