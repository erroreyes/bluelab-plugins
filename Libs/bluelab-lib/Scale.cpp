//
//  Scale.cpp
//
//  Created by Pan on 08/04/18.
//
//

#include <cmath>

#include <BLTypes.h>
#include <BLUtils.h>
#include <MelScale.h>

#include "Scale.h"

#define LOG_SCALE_FACTOR 0.25
#define LOG_SCALE2_FACTOR 3.5

Scale::Scale()
{
    mMelScale = new MelScale();
}

Scale::~Scale()
{
    delete mMelScale;
}

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
    else if ((scaleType == MEL) || (scaleType == MEL_FILTER))
    {
        x = NormalizedToMel(x, minValue, maxValue);
    }
    else if ((scaleType == MEL_INV) || (scaleType == MEL_FILTER_INV))
    {
        x = NormalizedToMelInv(x, minValue, maxValue);
    }
    else if (scaleType == DB_INV)
    {
        x = NormalizedToDBInv(x, minValue, maxValue);
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
    else if (scaleType == MEL_FILTER)
    {
        DataToMelFilter(values, minValue, maxValue);
    }
    else if (scaleType == MEL_FILTER_INV)
    {
        DataToMelFilterInv(values, minValue, maxValue);
    }
}
// TMP HACK
//template void Scale::ApplyScale(Type scaleType, WDL_TypedBuf<float> *values,
//                                float minValue, float maxValue);
//template void Scale::ApplyScale(Type scaleType, WDL_TypedBuf<double> *values,
//                                double minValue, double maxValue);
template void Scale::ApplyScale(Type scaleType, WDL_TypedBuf<BL_FLOAT> *values,
                                BL_FLOAT minValue, BL_FLOAT maxValue);


template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToDB(FLOAT_TYPE x, FLOAT_TYPE mindB, FLOAT_TYPE maxdB)
{
    if (std::fabs(x) < BL_EPS)
        x = mindB;
    else
        x = BLUtils::AmpToDB(x);
    
    x = (x - mindB)/(maxdB - mindB);
    
    // Avoid negative values, for very low x dB
    if (x < 0.0)
        x = 0.0;
    
    return x;
}
template float Scale::NormalizedToDB(float y, float mindB, float maxdB);
template double Scale::NormalizedToDB(double y, double mindB, double maxdB);

template <typename FLOAT_TYPE>
FLOAT_TYPE
Scale::NormalizedToDBInv(FLOAT_TYPE x, FLOAT_TYPE mindB, FLOAT_TYPE maxdB)
{
    FLOAT_TYPE minAmp = BLUtils::DBToAmp(mindB);
    FLOAT_TYPE maxAmp = BLUtils::DBToAmp(maxdB);
    
    x = mindB + x*(maxdB - mindB);
    
    x = BLUtils::DBToAmp(x);
    
    x = (x - minAmp)/(maxAmp - minAmp);
    
    // Avoid negative values, for very low x dB
    if (x < 0.0)
        x = 0.0;
    
    return x;
}
template float Scale::NormalizedToDBInv(float y, float mindB, float maxdB);
template double Scale::NormalizedToDBInv(double y, double mindB, double maxdB);

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
FLOAT_TYPE
Scale::NormalizedToMelInv(FLOAT_TYPE x,
                          FLOAT_TYPE minFreq,
                          FLOAT_TYPE maxFreq)
{
    FLOAT_TYPE minMel = MelScale::HzToMel(minFreq);
    FLOAT_TYPE maxMel = MelScale::HzToMel(maxFreq);
    
    x = x*(maxMel - minMel) + minMel;
    
    x = MelScale::MelToHz(x);
    
    x = (x - minFreq)/(maxFreq - minFreq);
    
    return x;
}
template float Scale::NormalizedToMelInv(float value,
                                         float minFreq, float maxFreq);
template double Scale::NormalizedToMelInv(double value,
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

template <typename FLOAT_TYPE>
void
Scale::DataToMelFilter(WDL_TypedBuf<FLOAT_TYPE> *values,
                       FLOAT_TYPE minFreq, FLOAT_TYPE maxFreq)
{
    int numFilters = values->GetSize();
    WDL_TypedBuf<FLOAT_TYPE> result;
    mMelScale->HzToMelFilter(&result, *values, (FLOAT_TYPE)(maxFreq*2.0), numFilters);
    
    *values = result;
}
// TMP HACK
//template void Scale::DataToMelFilter(WDL_TypedBuf<float> *values,
//                                     float minFreq, float maxFreq);
//template void Scale::DataToMelFilter(WDL_TypedBuf<double> *values,
//                                     double minFreq, double maxFreq);
template void Scale::DataToMelFilter(WDL_TypedBuf<BL_FLOAT> *values,
                                     BL_FLOAT minFreq, BL_FLOAT maxFreq);

template <typename FLOAT_TYPE>
void
Scale::DataToMelFilterInv(WDL_TypedBuf<FLOAT_TYPE> *values,
                          FLOAT_TYPE minFreq, FLOAT_TYPE maxFreq)
{
    int numFilters = values->GetSize();
    WDL_TypedBuf<FLOAT_TYPE> result;
    mMelScale->MelToHzFilter(&result, *values, (FLOAT_TYPE)(maxFreq*2.0), numFilters);
    
    *values = result;
}
// TMP HACK
//template void Scale::DataToMelFilterInv(WDL_TypedBuf<float> *values,
//                                        float minFreq, float maxFreq);
//template void Scale::DataToMelFilterInv(WDL_TypedBuf<double> *values,
//                                        double minFreq, double maxFreq);
template void Scale::DataToMelFilterInv(WDL_TypedBuf<BL_FLOAT> *values,
                                        BL_FLOAT minFreq, BL_FLOAT maxFreq);
