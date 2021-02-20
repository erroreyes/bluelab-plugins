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
     NORMALIZED,
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

    // Apply to Y

    // Apply value by value
    
    // Generic
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT ApplyScale(Type scaleType,
                                   BL_FLOAT x,
                                   BL_FLOAT minValue = -1.0,
                                   BL_FLOAT maxValue = -1.0);
    
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT ApplyScaleInv(Type scaleType,
                                      BL_FLOAT x,
                                      BL_FLOAT minValue = -1.0,
                                      BL_FLOAT maxValue = -1.0);

    // OPTIM: Apply for each value
    void ApplyScaleForEach(Type scaleType,
                           WDL_TypedBuf<BL_FLOAT> *values,
                           BL_FLOAT minValue = -1.0,
                           BL_FLOAT maxValue = -1.0);
    
    void ApplyScaleInvForEach(Type scaleType,
                              WDL_TypedBuf<BL_FLOAT> *values,
                              BL_FLOAT minValue = -1.0,
                              BL_FLOAT maxValue = -1.0);
    
    // Apply to X
    
    //template <typename FLOAT_TYPE>
    void ApplyScale(Type scaleType,
                    WDL_TypedBuf<BL_FLOAT> *values,
                    BL_FLOAT minValue = -1.0,
                    BL_FLOAT maxValue = -1.0);
    
protected:
    BL_FLOAT ValueToNormalized(BL_FLOAT y,
                               BL_FLOAT minValue,
                               BL_FLOAT maxValue);

    BL_FLOAT ValueToNormalizedInv(BL_FLOAT y,
                                  BL_FLOAT minValue,
                                  BL_FLOAT maxValue);
    
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToDB(BL_FLOAT y, BL_FLOAT mindB,
                                       BL_FLOAT maxdB);
    
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToDBInv(BL_FLOAT y, BL_FLOAT mindB,
                                          BL_FLOAT maxdB);
    
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToLog(BL_FLOAT x, BL_FLOAT minValue,
                                        BL_FLOAT maxValue);
    
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToLogInv(BL_FLOAT x, BL_FLOAT minValue,
                                           BL_FLOAT maxValue);
    
#if 0 // Legacy test
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToLogCoeff(BL_FLOAT x,
                                             BL_FLOAT minValue,
                                             BL_FLOAT maxValue);
#endif
    
    // Apply to axis for example
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToLogScale(BL_FLOAT value);
    
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToLogScaleInv(BL_FLOAT value);
    
    // Apply to spectrogram for example
    //template <typename FLOAT_TYPE>
    /*static*/ void DataToLogScale(WDL_TypedBuf<BL_FLOAT> *values);
    
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToMel(BL_FLOAT x,
                                        BL_FLOAT minFreq,
                                        BL_FLOAT maxFreq);
    
    //template <typename FLOAT_TYPE>
    /*static*/ BL_FLOAT NormalizedToMelInv(BL_FLOAT x,
                                           BL_FLOAT minFreq,
                                           BL_FLOAT maxFreq);
    
    //template <typename FLOAT_TYPE>
    /*static*/ void DataToMel(WDL_TypedBuf<BL_FLOAT> *values,
                              BL_FLOAT minFreq, BL_FLOAT maxFreq);
    
    //template <typename FLOAT_TYPE>
    void DataToMelFilter(WDL_TypedBuf<BL_FLOAT> *values,
                         BL_FLOAT minFreq, BL_FLOAT maxFreq);
    
    //template <typename FLOAT_TYPE>
    void DataToMelFilterInv(WDL_TypedBuf<BL_FLOAT> *values,
                            BL_FLOAT minFreq, BL_FLOAT maxFreq);

    // OPTIM: process in block
    //
    void ValueToNormalizedForEach(WDL_TypedBuf<BL_FLOAT> *values,
                                  BL_FLOAT minValue, BL_FLOAT maxValue);
    
    void ValueToNormalizedInvForEach(WDL_TypedBuf<BL_FLOAT> *values,
                                     BL_FLOAT minValue, BL_FLOAT maxValue);
    
    void NormalizedToDBForEach(WDL_TypedBuf<BL_FLOAT> *values,
                               BL_FLOAT mindB, BL_FLOAT maxdB);

    void NormalizedToDBInvForEach(WDL_TypedBuf<BL_FLOAT> *values,
                                  BL_FLOAT mindB, BL_FLOAT maxdB);    
 
    void NormalizedToLogForEach(WDL_TypedBuf<BL_FLOAT> *values,
                                BL_FLOAT minValue, BL_FLOAT maxValue);
 
    void NormalizedToLogInvForEach(WDL_TypedBuf<BL_FLOAT> *values,
                                   BL_FLOAT minValue, BL_FLOAT maxValue);
        
    void NormalizedToLogScaleForEach(WDL_TypedBuf<BL_FLOAT> *values);
    
    void NormalizedToLogScaleInvForEach(WDL_TypedBuf<BL_FLOAT> *values);
    
    void NormalizedToMelForEach(WDL_TypedBuf<BL_FLOAT> *values,
                                BL_FLOAT minFreq, BL_FLOAT maxFreq);
    
    void NormalizedToMelInvForEach(WDL_TypedBuf<BL_FLOAT> *values,
                                   BL_FLOAT minFreq, BL_FLOAT maxFreq);
    //
    // Must keep the object, for precomputed filter bank
    MelScale *mMelScale;

private:
    // Tmp buffers
    WDL_TypedBuf<BL_FLOAT> mTmpBuf0;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf1;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf2;
    WDL_TypedBuf<BL_FLOAT> mTmpBuf3;
};

#endif
