#include "GhostCommandGain.h"

GhostCommandGain::GhostCommandGain(BL_FLOAT factor)
{
    mFactor = factor;
}

GhostCommandGain::~GhostCommandGain() {}

void
GhostCommandGain::Apply(vector<WDL_TypedBuf<BL_FLOAT> > *magns,
                   vector<WDL_TypedBuf<BL_FLOAT> > *phases)
{
    // Do not use phases
    
    WDL_TypedBuf<BL_FLOAT> selectedMagns;
    
    // Get the selected data, just for convenience
    //GetSelectedData(*data, &selectedData);
    GetSelectedDataY(*magns, &selectedMagns);
    
    // For the moment, do not use fade, just fill all with zeros
    Utils::MultValues(&selectedMagns, mFactor);
    
    // And replace in the result
    //ReplaceSelectedData(data, selectedData);
    ReplaceSelectedDataY(magns, selectedMagns);
}
