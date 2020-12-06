#ifndef GHOST_COMMAND_GAIN_H
#define GHOST_COMMAND_GAIN_H

#include <GhostCommand.h>

#include "IPlug_include_in_plug_hdr.h"

class GhostCommandGain : public Command
{
public:
    GhostCommandGain(BL_FLOAT factor);
    
    virtual ~GhostCommandGain();
    
    void Apply(vector<WDL_TypedBuf<BL_FLOAT> > *magns,
               vector<WDL_TypedBuf<BL_FLOAT> > *phases);
    
protected:
    BL_FLOAT mFactor;
};

#endif
