//
//  BLVumeter2SidesControl.hpp
//  UST-macOS
//
//  Created by applematuer on 9/30/20.
//
//

#ifndef BLVumeter2SidesControl_h
#define BLVumeterControl_h

#include <IGraphics.h>
#include <IControl.h>
#include <IGraphicsStructs.h>
#include <IPlugConstants.h>

using namespace iplug;
using namespace iplug::igraphics;

// Vector vumeter
class BLVumeter2SidesControl : public IControl
{
public:
    BLVumeter2SidesControl(IRECT &rect,
                           const IColor &color,
                           int paramIdx = kNoParameter);
    
    virtual ~BLVumeter2SidesControl();
    
    void Draw(IGraphics& g) override;
    
protected:
    IColor mColor;
};

#endif /* BLVumeterControl_hpp */
