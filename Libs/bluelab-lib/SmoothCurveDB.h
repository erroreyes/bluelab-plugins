//
//  SmoothCurveDB.h
//
//  Created by Apple m'a Tuer on 10/09/17.
//
//

#ifndef SmoothCurveDB_h
#define SmoothCurveDB_h

#include <BLTypes.h>

#include "IPlug_include_in_plug_hdr.h"

class SmoothAvgHistogramDB;
class GraphCurve5;
class SmoothCurveDB
{
public:
    SmoothCurveDB(GraphCurve5 *curve,
                  BL_FLOAT smoothFactor,
                  int size, BL_FLOAT defaultValue,
                  BL_FLOAT minDB, BL_FLOAT maxDB);
    
    virtual ~SmoothCurveDB();
    
    void Reset();
    
    void AddValues(const WDL_TypedBuf<BL_FLOAT> &values);
    
protected:
    SmoothAvgHistogramDB *mHistogram;
    
    GraphCurve5 *mCurve;
};

#endif
