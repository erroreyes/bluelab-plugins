//
//  DNNModelMc.h
//  BL-Rebalance
//
//  Created by applematuer on 5/24/20.
//
//

#ifndef BL_Rebalance_DNNModelMc_h
#define BL_Rebalance_DNNModelMc_h

#include <vector>
using namespace std;

#include "IPlug_include_in_plug_hdr.h"

using namespace iplug::igraphics;

class DNNModelMc
{
public:
    DNNModelMc() {}
    
    virtual ~DNNModelMc() {};

    virtual bool Load(const char *modelFileName,
                      const char *resourcePath) = 0;
                
    // For WIN32
    virtual bool LoadWin(IGraphics *pGraphics,
                         int modelRcId, int weightsRcId) = 0;
    
    // Returns several masks at once
    virtual void Predict(const WDL_TypedBuf<BL_FLOAT> &input,
                         vector<WDL_TypedBuf<BL_FLOAT> > *masks) = 0;
};

#endif
