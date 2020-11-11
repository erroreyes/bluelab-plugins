//
//  Scale.cpp
//
//  Created by Pan on 08/04/18.
//
//

#include <cmath>

#include <BLUtils.h>
#include <MelScale.h>

#include "Scale.h"

#define LOG_SCALE_FACTOR 0.25
#define LOG_SCALE2_FACTOR 3.5

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
#if 0
    else if (scaleType == LOG_FACTOR)
    {
        x = NormalizedToLogFactor(x, minValue, maxValue);
    }
#endif
    else if (scaleType == LOG_FACTOR)
    {
        x = NormalizedToLogScale(x);
    }
    else if (scaleType == MEL)
    {
        x = NormalizedToMel(x, minValue, maxValue);
    }
    
    return x;
}
template float Scale::ApplyScale(Type scaleType, float y, float mindB, float maxdB);
template double Scale::ApplyScale(Type scaleType, double y, double mindB, double maxdB);

template <typename FLOAT_TYPE>
void
Scale::ApplyScale(Type scaleType,
                  WDL_TypedBuf<FLOAT_TYPE> *values,
                  FLOAT_TYPE minValue, FLOAT_TYPE maxValue)
{
    if (scaleType == LOG_FACTOR)
    {
        DataToLogScale(values);
    }
    else if (scaleType == MEL)
    {
        DataToMel(values, minValue, maxValue);
    }
}
template void Scale::ApplyScale(Type scaleType, WDL_TypedBuf<float> *values,
                                float minValue, float maxValue);
template void Scale::ApplyScale(Type scaleType, WDL_TypedBuf<double> *values,
                                double minValue, double maxValue);


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

#if 0 // Legacy test
template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToLogCoeff(FLOAT_TYPE x, FLOAT_TYPE minValue, FLOAT_TYPE maxValue)
{
    // This trick may be improved!
    x *= LOG_SCALE_FACTOR;
    minValue *= LOG_SCALE_FACTOR;
    maxValue *= LOG_SCALE_FACTOR;
    
    x = NormalizedToLog(x, minValue, maxValue);
    
    return x;
}
template float Scale::NormalizedToLogCoeff(float x, float minValue, float maxValue);
template double Scale::NormalizedToLogCoeff(double x, double minValue, double maxValue);
#endif

template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToLogScale(FLOAT_TYPE value)
{
    FLOAT_TYPE t0 =
        std::log((FLOAT_TYPE)1.0 + value*(std::exp(LOG_SCALE2_FACTOR) - 1.0))/LOG_SCALE2_FACTOR;
                        
    return t0;
}
template float Scale::NormalizedToLogScale(float value);
template double Scale::NormalizedToLogScale(double value);

template <typename FLOAT_TYPE>
void
Scale::DataToLogScale(WDL_TypedBuf<FLOAT_TYPE> *values)
{
    WDL_TypedBuf<FLOAT_TYPE> origValues = *values;
    
    int valuesSize = values->GetSize();
    FLOAT_TYPE *valuesData = values->Get();
    FLOAT_TYPE *origValuesData = origValues.Get();
    
    for (int i = 0; i < valuesSize; i++)
    {
        FLOAT_TYPE t0 = ((FLOAT_TYPE)i)/valuesSize;
        
        // "Inverse" process for data
        t0 *= LOG_SCALE2_FACTOR;
        FLOAT_TYPE t = (std::exp(t0) - 1.0)/(std::exp(LOG_SCALE2_FACTOR) - 1.0);
        
        int dstIdx = (int)(t*valuesSize);
        
        if (dstIdx < 0)
            // Should not happen
            dstIdx = 0;
        
        if (dstIdx > valuesSize - 1)
            // We never know...
            dstIdx = valuesSize - 1;
        
        FLOAT_TYPE dstVal = origValuesData[dstIdx];
        valuesData[i] = dstVal;
    }
}
template void Scale::DataToLogScale(WDL_TypedBuf<float> *values);
template void Scale::DataToLogScale(WDL_TypedBuf<double> *values);

template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToMel(FLOAT_TYPE x,
                       FLOAT_TYPE minFreq,
                       FLOAT_TYPE maxFreq)
{
    x = x*(maxFreq - minFreq) + minFreq;
    
    x = MelScale::HzToMel(x);
    
    FLOAT_TYPE lMin = MelScale::HzToMel(minFreq);
    FLOAT_TYPE lMax = MelScale::HzToMel(maxFreq);
    
    x = (x - lMin)/(lMax - lMin);
    
    return x;
}
template float Scale::NormalizedToMel(float value,
                                      float minFreq, float maxFreq);
template double Scale::NormalizedToMel(double value,
                                       double minFreq, double maxFreq);

template <typename FLOAT_TYPE>
void
Scale::DataToMel(WDL_TypedBuf<FLOAT_TYPE> *values,
                 FLOAT_TYPE minFreq, FLOAT_TYPE maxFreq)
{
    WDL_TypedBuf<FLOAT_TYPE> origValues = *values;
    
    int valuesSize = values->GetSize();
    FLOAT_TYPE *valuesData = values->Get();
    FLOAT_TYPE *origValuesData = origValues.Get();
    
    FLOAT_TYPE minMel = MelScale::HzToMel(minFreq);
    FLOAT_TYPE maxMel = MelScale::HzToMel(maxFreq);
    
    for (int i = 0; i < valuesSize; i++)
    {
        FLOAT_TYPE t0 = ((FLOAT_TYPE)i)/valuesSize;

        // "Inverse" process for data
        FLOAT_TYPE mel = minMel + t0*(maxMel - minMel);
        FLOAT_TYPE freq = MelScale::MelToHz(mel);
        FLOAT_TYPE t = (freq - minFreq)/(maxFreq - minFreq);

        int dstIdx = (int)(t*valuesSize);
        
        if (dstIdx < 0)
            // Should not happen
            dstIdx = 0;
        
        if (dstIdx > valuesSize - 1)
            // We never know...
            dstIdx = valuesSize - 1;
        
        FLOAT_TYPE dstVal = origValuesData[dstIdx];
        valuesData[i] = dstVal;
    }
}
template void Scale::DataToMel(WDL_TypedBuf<float> *values,
                               float minFreq, float maxFreq);
template void Scale::DataToMel(WDL_TypedBuf<double> *values,
                               double minFreq, double maxFreq);
