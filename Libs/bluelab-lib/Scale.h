//
//  Scale.h
//  Created by Pan on 08/04/18.
//
//

#ifndef __BL_Spectrogram__ColorMap__
#define __BL_Spectrogram__ColorMap__

#include <BLTypes.h>

#include "IPlug_include_in_plug_hdr.h"

class MelScale;
class Scale
{
public:
    // Note: log10, or log-anything is the same since we use normalized values
    enum Type
    {
        LINEAR,
        DB,
        LOG,
        LOG_FACTOR,
        MEL, // Quick Mel
        MEL_FILTER, // Mel with real filters,
        MEL_INV,
        MEL_FILTER_INV,
        DB_INV
    };
    
    Scale();
    virtual ~Scale();
    
    // Generic
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE ApplyScale(Type scaleType,
                                 FLOAT_TYPE x,
                                 FLOAT_TYPE minValue = -1.0,
                                 FLOAT_TYPE maxValue = -1.0);
    
    template <typename FLOAT_TYPE>
    void ApplyScale(Type scaleType,
                    WDL_TypedBuf<FLOAT_TYPE> *values,
                    FLOAT_TYPE minValue = -1.0,
                    FLOAT_TYPE maxValue = -1.0);
    
protected:
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToDB(FLOAT_TYPE y, FLOAT_TYPE mindB, FLOAT_TYPE maxdB);
    
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToDBInv(FLOAT_TYPE y, FLOAT_TYPE mindB, FLOAT_TYPE maxdB);
    
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToLog(FLOAT_TYPE x, FLOAT_TYPE minValue,
                                      FLOAT_TYPE maxValue);
    
#if 0 // Legacy test
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToLogCoeff(FLOAT_TYPE x,
                                           FLOAT_TYPE minValue, FLOAT_TYPE maxValue);
#endif
    
    // Apply to axis for example
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToLogScale(FLOAT_TYPE value);
    
    // Apply to spectrogram for example
    template <typename FLOAT_TYPE>
    static void DataToLogScale(WDL_TypedBuf<FLOAT_TYPE> *values);
    
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToMel(FLOAT_TYPE x,
                                      FLOAT_TYPE minFreq,
                                      FLOAT_TYPE maxFreq);
    
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToMelInv(FLOAT_TYPE x,
                                         FLOAT_TYPE minFreq,
                                         FLOAT_TYPE maxFreq);
    
    template <typename FLOAT_TYPE>
    static void DataToMel(WDL_TypedBuf<FLOAT_TYPE> *values,
                          FLOAT_TYPE minFreq, FLOAT_TYPE maxFreq);
    
    template <typename FLOAT_TYPE>
    void DataToMelFilter(WDL_TypedBuf<FLOAT_TYPE> *values,
                         FLOAT_TYPE minFreq, FLOAT_TYPE maxFreq);
    
    template <typename FLOAT_TYPE>
    void DataToMelFilterInv(WDL_TypedBuf<FLOAT_TYPE> *values,
                            FLOAT_TYPE minFreq, FLOAT_TYPE maxFreq);
    
    //
    // Must keep the object, for precomputed filter bank
    MelScale *mMelScale;
};

#endif
