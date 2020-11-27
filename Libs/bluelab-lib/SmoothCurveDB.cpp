//
//  AvgHistogram.cpp
//  EQHack
//
//  Created by Apple m'a Tuer on 10/09/17.
//
//

#include <SmoothAvgHistogramDB.h>
#include <GraphCurve5.h>

#include <BLUtils.h>
#include <BLDebug.h>

#include "SmoothCurveDB.h"

SmoothCurveDB::SmoothCurveDB(GraphCurve5 *curve,
                             BL_FLOAT smoothFactor,
                             int size, BL_FLOAT defaultValue,
                             BL_FLOAT minDB, BL_FLOAT maxDB)
{
    mMinDB = minDB;
    mMaxDB = maxDB;
    
    mHistogram = new SmoothAvgHistogramDB(size, smoothFactor,
                                          defaultValue, minDB, maxDB);
    
    mCurve = curve;
}

SmoothCurveDB::~SmoothCurveDB()
{
    delete mHistogram;
}

void
SmoothCurveDB::Reset()
{
    mHistogram->Reset();
}

void
SmoothCurveDB::AddValues(const WDL_TypedBuf<BL_FLOAT> &values)
{
    // Add the values
    int histoNumValues = mHistogram->GetNumValues();
    
    WDL_TypedBuf<BL_FLOAT> values0 = values;
    if (values0.GetSize() > histoNumValues)
    {
        // Decimate
        BL_FLOAT decimFactor = ((BL_FLOAT)histoNumValues)/values0.GetSize();
        
        WDL_TypedBuf<BL_FLOAT> decimValues;
        BLUtils::DecimateSamples(&decimValues, values0, decimFactor);
        
        values0 = decimValues;
    }
    
    mHistogram->AddValues(values0);

    // Process values and update curve
    WDL_TypedBuf<BL_FLOAT> avgValues;
    mHistogram->GetValues(&avgValues);
    
    mCurve->ClearValues();
    
#if 0 // Old version
    for (int i = 0; i < avgValues.GetSize(); i++)
    {
        BL_FLOAT t = ((BL_FLOAT)i)/(avgValues.GetSize() - 1);
        BL_FLOAT val = avgValues.Get()[i];
        mCurve->SetValue(t, val);
    }
#endif
    
#if 1 // New version: avoid the risk to have undefined values
    mCurve->SetValues4(avgValues);
#endif
}
