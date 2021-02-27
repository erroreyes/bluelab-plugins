//
//  ParamSmoother2.h
//  UST
//
//  Created by applematuer on 1/4/20.
//
//

#ifndef UST_ParamSmoother2_h
#define UST_ParamSmoother2_h

#include <cmath>

#include <BLTypes.h>
#include <BLUtils.h>

// 140ms => coeff 0.999 at 44100Hz
#define DEFAULT_SMOOTHING_TIME_MS 140.0

// See: http://www.musicdsp.org/en/latest/Filters/257-1-pole-lpf-for-smooth-parameter-changes.html
//
// Advanced version of simple smooth usually used by me
// - the result won't vary if we change the sample rate
// - we specify the smooth with smooth time (ms)
//

// ParamSmoother2: from ParamSmoother2
class ParamSmoother2
{
public:
    ParamSmoother2(BL_FLOAT samplingRate,
                   BL_FLOAT value,
                   BL_FLOAT smoothingTimeMs = DEFAULT_SMOOTHING_TIME_MS)
    {
        mSmoothingTimeMs = smoothingTimeMs;
        mSampleRate = samplingRate;

        mZ = value;
        mTargetValue = value;
        
        Reset(samplingRate);
    }
    
    virtual ~ParamSmoother2() {}
    
    inline void Reset(BL_FLOAT samplingRate)
    {
        mSampleRate = samplingRate;
        
        mA = std::exp(-(BL_FLOAT)M_TWO_PI/(mSmoothingTimeMs * 0.001 * samplingRate));
        mB = 1.0 - mA;
        //mZ = 0.0;
        mZ = mTargetValue;
    }
    
    inline void SetTargetValue(BL_FLOAT val)
    {
        mTargetValue = val;
    }

    inline BL_FLOAT Process()
    {
        mZ = (mTargetValue * mB) + (mZ * mA);

        FIX_FLT_DENORMAL(mZ);
        
        return mZ;
    }
    
protected:
    BL_FLOAT mSmoothingTimeMs;
    BL_FLOAT mSampleRate;
    
    BL_FLOAT mA;
    BL_FLOAT mB;
    BL_FLOAT mZ;

    BL_FLOAT mTargetValue;
};

#endif
