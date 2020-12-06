#ifndef GHOST_COMMAND_REPLACE_H
#define GHOST_COMMAND_REPLACE_H

#include <GhostCommand.h>

#include "IPlug_include_in_plug_hdr.h"

class GhostCommandReplace : public Command
{
public:
    GhostCommandReplace(bool processHorizontal, bool processVertical);
    
    virtual ~GhostCommandReplace();
    
    void Apply(vector<WDL_TypedBuf<BL_FLOAT> > *magns,
               vector<WDL_TypedBuf<BL_FLOAT> > *phases);
    
protected:
    bool mProcessHorizontal;
    bool mProcessVertical;
};


#endif 
