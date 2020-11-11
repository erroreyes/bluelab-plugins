//
//  Scale.h
//  Created by Pan on 08/04/18.
//
//

#ifndef __BL_Spectrogram__ColorMap__
#define __BL_Spectrogram__ColorMap__

#include <BLTypes.h>

#define LOG_SCALE_COEFF 0.25

class Scale
{
public:
    // Note: log10, or log-anything is the same since we use normalized values
    enum Type
    {
        LINEAR,
        DB,
        LOG,
        LOG_COEFF
    };
    
    // Generic
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE ApplyScale(Type scaleType,
                                 FLOAT_TYPE x, FLOAT_TYPE minValue, FLOAT_TYPE maxValue);
    
protected:
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToDB(FLOAT_TYPE y, FLOAT_TYPE mindB, FLOAT_TYPE maxdB);
    
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToLog(FLOAT_TYPE x, FLOAT_TYPE minValue, FLOAT_TYPE maxValue);
    
    template <typename FLOAT_TYPE>
    static FLOAT_TYPE NormalizedToLogCoeff(FLOAT_TYPE x,
                                           FLOAT_TYPE minValue, FLOAT_TYPE maxValue);
};

#endif
