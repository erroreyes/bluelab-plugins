#ifndef GHOST_TRIGGER_CONTROL_H
#define GHOST_TRIGGER_CONTROL_H

class GhostTriggerControl : public ITriggerControl
{
public:
    GhostTriggerControl(IPlugin* pPlug, int paramIdx)
    : ITriggerControl(pPlug, paramIdx) {}
    
    virtual ~GhostTriggerControl() {}
    
    virtual void OnGUIIdle();
};

#endif
