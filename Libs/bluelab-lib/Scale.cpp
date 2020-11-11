//
//  Scale.cpp
//
//  Created by Pan on 08/04/18.
//
//

#include <cmath>

#include <BLUtils.h>

#include "Scale.h"

template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::ApplyScale(Type scaleType,
                  FLOAT_TYPE x, FLOAT_TYPE minValue, FLOAT_TYPE maxValue)
{
    if (scaleType == DB)
    {
        x = NormalizedToDB(x, minValue, maxValue);
    }
    else if (scaleType == LOG)
    {
        x = NormalizedToLog(x, minValue, maxValue);
    }
    else if (scaleType == LOG_COEFF)
    {
        x = NormalizedToLogCoeff(x, minValue, maxValue);
    }
    
    return x;
}
template float Scale::ApplyScale(Type scaleType, float y, float mindB, float maxdB);
template double Scale::ApplyScale(Type scaleType, double y, double mindB, double maxdB);

template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToDB(FLOAT_TYPE x, FLOAT_TYPE mindB, FLOAT_TYPE maxdB)
{
    if (std::fabs(x) < BL_EPS)
        x = mindB;
    else
        x = BLUtils::AmpToDB(x);
    
    x = (x - mindB)/(maxdB - mindB);
    
    return x;
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

template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToLogCoeff(FLOAT_TYPE x, FLOAT_TYPE minValue, FLOAT_TYPE maxValue)
{
    // This trick may be improved!
    x *= LOG_SCALE_COEFF;
    minValue *= LOG_SCALE_COEFF;
    maxValue *= LOG_SCALE_COEFF;
    
    x = NormalizedToLog(x, minValue, maxValue);
    
    return x;
}
template float Scale::NormalizedToLogCoeff(float x, float minValue, float maxValue);
template double Scale::NormalizedToLogCoeff(double x, double minValue, double maxValue);
