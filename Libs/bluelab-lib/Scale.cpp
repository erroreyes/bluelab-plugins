//
//  Scale.cpp
//
//  Created by Pan on 08/04/18.
//
//

#include <cmath>

#include <BLUtils.h>

#include "Scale.h"

//
template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToDB(FLOAT_TYPE y, FLOAT_TYPE mindB, FLOAT_TYPE maxdB)
{
    if (std::fabs(y) < BL_EPS)
        y = mindB;
    else
        y = BLUtils::AmpToDB(y);
    
    y = (y - mindB)/(maxdB - mindB);
    
    return y;
}
template float Scale::NormalizedToDB(float y, float mindB, float maxdB);
template double Scale::NormalizedToDB(double y, double mindB, double maxdB);

template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToLog(FLOAT_TYPE x, FLOAT_TYPE minValue, FLOAT_TYPE maxValue)
{
    x = x*(maxValue - minValue) + minValue;
    
    x = std::log10(1.0 + x);
    
    FLOAT_TYPE lMin = std::log10(1.0 + minValue);
    FLOAT_TYPE lMax = std::log10(1.0 + maxValue);
    
    x = (x - lMin)/(lMax - lMin);
    
    return x;
}
template float Scale::NormalizedToLog(float x, float mindB, float maxdB);
template double Scale::NormalizedToLog(double x, double mindB, double maxdB);
