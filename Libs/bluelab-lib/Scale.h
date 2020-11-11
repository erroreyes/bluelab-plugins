//
//  Scale.h
//  Created by Pan on 08/04/18.
//
//

#ifndef __BL_Spectrogram__ColorMap__
#define __BL_Spectrogram__ColorMap__

#include <BLTypes.h>

#include "IPlug_include_in_plug_hdr.h"

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
        LOG_FACTOR2
    };
    
    // Generic
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE ApplyScale(Type scaleType,
                                 FLOAT_TYPE x,
                                 FLOAT_TYPE minValue = -1.0,
                                 FLOAT_TYPE maxValue = -1.0);
    
    template <typename FLOAT_TYPE>
    static void ApplyScale(Type scaleType, WDL_TypedBuf<FLOAT_TYPE> *values);
    
protected:
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToDB(FLOAT_TYPE y, FLOAT_TYPE mindB, FLOAT_TYPE maxdB);
    
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToLog(FLOAT_TYPE x, FLOAT_TYPE minValue,
                                      FLOAT_TYPE maxValue);
    
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToLogCoeff(FLOAT_TYPE x,
                                           FLOAT_TYPE minValue, FLOAT_TYPE maxValue);
    
    // Apply to axis for example
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToLogScale2(FLOAT_TYPE value);
    
    // Apply to spectrogram for example
    template <typename FLOAT_TYPE>
    static void LogScale2(WDL_TypedBuf<FLOAT_TYPE> *values);
};

#endif
