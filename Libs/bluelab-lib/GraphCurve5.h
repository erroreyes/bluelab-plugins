//
//  GraphCurve.h
//  EQHack
//
//  Created by Apple m'a Tuer on 10/09/17.
//
//

#ifndef EQHack_GraphCurve5_h
#define EQHack_GraphCurve5_h

#include <BLTypes.h>
#include <ParamSmoother.h>

#include "IPlug_include_in_plug_hdr.h"

#define FILL_CURVE_ALPHA 0.125

#define CURVE_VALUE_UNDEFINED 1e16

typedef float CurveColor [4];

using namespace iplug::igraphics;

class GraphCurve5
{
public:
    GraphCurve5(int numValues);
    
    virtual ~GraphCurve5();
    
    void SetViewSize(BL_FLOAT width, BL_FLOAT height);
    
    void SetBounds(const BL_GUI_FLOAT bounds[4]);
    void SetYScaleFactor(BL_FLOAT factor);
    void SetAutoAdjustFactor(BL_FLOAT factor);
    
    void SetDescription(const char *description, int descrColor[4]);
    
    void ResetNumValues(int numValues);
    
    void ClearValues();
    
    void SetYScale(bool flag, BL_GUI_FLOAT minY = -120.0, BL_GUI_FLOAT maxY = 0.0);
    
    // Fill linearly
    void FillAllXValues(BL_GUI_FLOAT minX, BL_GUI_FLOAT maxX);
    
    void FillAllYValues(BL_GUI_FLOAT val);
    
    void SetYValues(const WDL_TypedBuf<BL_GUI_FLOAT> *yValues,
                    BL_GUI_FLOAT minX, BL_GUI_FLOAT maxX);
    
    void SetValue(BL_GUI_FLOAT t, BL_GUI_FLOAT x, BL_GUI_FLOAT y);
    
    // The last value will be automatically poped
    void PushValue(BL_GUI_FLOAT x, BL_GUI_FLOAT y);
    
    void NormalizeXValues(BL_GUI_FLOAT maxXValue);
    
    // Points
    void SetXScale(bool dBFlag,
                   BL_GUI_FLOAT minX = 0.0, BL_GUI_FLOAT maxX = 1.0);
    
    void SetPointValues(const WDL_TypedBuf<BL_GUI_FLOAT> &xValues,
                        const WDL_TypedBuf<BL_GUI_FLOAT> &yValues);
    
    // Bounds
    void SetLimitToBounds(bool flag);
    
    void SetWeightTargetColor(int color[4]);
    
    //
    void Reset(BL_GUI_FLOAT resetVal);
    
    // Not tested
    void Resize(int numValues);
    int GetNumValues();
    
    // Curves style
    void SetColor(int r, int g, int b);
    void SetAlpha(BL_GUI_FLOAT alpha);
    void SetLineWidth(BL_GUI_FLOAT lineWidth);
    
    // For UST
    void SetBevel(bool bevelFlag);
    
    void SetSmooth(bool flag);
    
    void SetFill(bool flag, BL_GUI_FLOAT originY = 0.0);
    
    void SetFillColor(int r, int g, int b);
    
    // Down
    // Fill under the curve
    void SetFillAlpha(BL_GUI_FLOAT alpha);
    
    // Up
    // Fill over the curve
    void SetFillAlphaUp(BL_GUI_FLOAT alpha);
    
    // Points
    void SetPointSize(BL_GUI_FLOAT pointSize);
    
    // For UST
    void SetPointOverlay(bool flag);
    
    void SetWeightMultAlpha(bool flag);
    
    void SetPointStyle(bool flag,
                       bool pointsAsLinesPolar, bool pointsAsLines = false);
    
    void SetValuesPoint(const WDL_TypedBuf<BL_GUI_FLOAT> &xValues,
                        const WDL_TypedBuf<BL_GUI_FLOAT> &yValues);
    
    // For UST
    void SetValuesPointEx(const WDL_TypedBuf<BL_GUI_FLOAT> &xValues,
                          const WDL_TypedBuf<BL_GUI_FLOAT> &yValues,
                          bool singleScale = false, bool scaleX = true,
                          bool centerFlag = false);
    
    void SetColorWeight(const WDL_TypedBuf<BL_GUI_FLOAT> &colorWeights);
    
    
    void SetValuesPointWeight(const WDL_TypedBuf<BL_GUI_FLOAT> &xValues,
                              const WDL_TypedBuf<BL_GUI_FLOAT> &yValues,
                              const WDL_TypedBuf<BL_GUI_FLOAT> &weights);
    
    // Curves data
    void FillAllValues(BL_GUI_FLOAT val);
    
    void SetValues(const WDL_TypedBuf<BL_GUI_FLOAT> *values);
    void SetValues2(const WDL_TypedBuf<BL_GUI_FLOAT> *values);
    void SetValues3(const WDL_TypedBuf<BL_GUI_FLOAT> *values);
    
    // Use simple decimation
    void SetValuesDecimateSimple(const WDL_TypedBuf<BL_GUI_FLOAT> *values);
    
    // Optimized version
    // Remove points if there is more points density than graph pixel density
    void SetValuesDecimate(const WDL_TypedBuf<BL_GUI_FLOAT> *values,
                           bool isWaveSignal = false);
    
    // Fixed for positive and negative maxima for sample/wave type values
    void SetValuesDecimate2(const WDL_TypedBuf<BL_GUI_FLOAT> *values,
                            BL_FLOAT decFactor,
                            bool isWaveSignal = false);
    
    // Fixed for some flat sections at 0
    void SetValuesDecimate3(const WDL_TypedBuf<BL_GUI_FLOAT> *values,
                            BL_FLOAT decFactor,
                            bool isWaveSignal = false);
    
    // NEW: Apply db scale on x before decimation
    // (utility method)
    //
    // GOOD: very good accuracy on low frequencies !
    //
    // minValue is the min sample value, or the min db value
    void SetValuesXDbDecimate(const WDL_TypedBuf<BL_GUI_FLOAT> *values,
                              int bufferSize, BL_GUI_FLOAT sampleRate,
                              BL_GUI_FLOAT decimFactor);
    
    // When the values are in dB
    void SetValuesXDbDecimateDb(const WDL_TypedBuf<BL_GUI_FLOAT> *values,
                                int bufferSize, BL_GUI_FLOAT sampleRate,
                                BL_GUI_FLOAT decimFactor,
                                BL_GUI_FLOAT minValueDb);
    
    // T is x normalized
    void SetValue(BL_GUI_FLOAT t, BL_GUI_FLOAT val);
    
    void SetSingleValueH(BL_GUI_FLOAT val);
    
    void SetSingleValueV(BL_GUI_FLOAT val);
    
    void PushValue(BL_GUI_FLOAT val);
    
    void SetSingleValueH(bool flag);
    
    void SetSingleValueV(bool flag);
    
    void SetOptimSameColor(bool flag);
    
protected:
    BL_GUI_FLOAT ConvertX(BL_GUI_FLOAT val, BL_GUI_FLOAT width);
    BL_GUI_FLOAT ConvertY(BL_GUI_FLOAT val, BL_GUI_FLOAT height);
    
    // Optimized versions
    void ConvertX(WDL_TypedBuf<BL_GUI_FLOAT> *vals, BL_GUI_FLOAT width);
    void ConvertY(WDL_TypedBuf<BL_GUI_FLOAT> *vals, BL_GUI_FLOAT height);
    
    friend class GraphControl12;
    
    // Description
    char *mDescription;
    int mDescrColor[4];
    
    // Scale
    bool mYdBScale;
    BL_GUI_FLOAT mMinY;
    BL_GUI_FLOAT mMaxY;
    
    // For Points
    
    // If true, display points instead of curves
    bool mPointStyle;
    bool mPointsAsLinesPolar;
    bool mPointsAsLines;
    
    bool mXdBScale;
    BL_GUI_FLOAT mMinX;
    BL_GUI_FLOAT mMaxX;
    
    CurveColor mColor;
    BL_GUI_FLOAT mAlpha;
    
    BL_GUI_FLOAT mLineWidth;
    BL_GUI_FLOAT mPointSize;
    
    // For UST
    //
    
    // Avoid peaks in lines that have bit stroke width
    
    bool mBevelFlag;
    
    bool mPointOverlay;
    
    // Unused...
    bool mDoSmooth;
    
    bool mCurveFill;
    BL_GUI_FLOAT mCurveFillOriginY;
    
    BL_GUI_FLOAT mFillAlpha;
    // To fill over the curve
    BL_GUI_FLOAT mFillAlphaUp;
    BL_GUI_FLOAT mFillAlphaUpFlag;
    
    CurveColor mFillColor;
    bool mFillColorSet;
    
    bool mSingleValueH;
    bool mSingleValueV;
    
    BL_GUI_FLOAT mPowerScaleX;
    
    int mNumValues;
    WDL_TypedBuf<BL_GUI_FLOAT> mXValues;
    WDL_TypedBuf<BL_GUI_FLOAT> mYValues;
    
    WDL_TypedBuf<BL_GUI_FLOAT> mWeights;
    bool mWeightMultAlpha;
    CurveColor mWeightTargetColor;
    bool mUseWeightTargetColor;
    
    // Hack for scrolling
    int mPushCounter;
    BL_GUI_FLOAT mTmpPushValue;
    
    // Bounds
    bool mLimitToBounds;
    
    // Optimization, when all the points or lines have the same color
    bool mOptimSameColor;
    
    BL_FLOAT mViewSize[2];
    BL_GUI_FLOAT mBounds[4];
    BL_FLOAT mYScaleFactor;
    BL_FLOAT mAutoAdjustFactor;
    
protected:
    ParamSmoother mParamSmoother;
};

#endif
