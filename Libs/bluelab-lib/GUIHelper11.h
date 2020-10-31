//
//  GUIHelper11.hpp
//  UST-macOS
//
//  Created by applematuer on 9/25/20.
//
//

#ifndef GUIHelper11_h
#define GUIHelper11_h

#include <IControls.h>

#include <BLVumeterControl.h>
#include <BLVumeterNeedleControl.h>
#include <ResizeGUIPluginInterface.h>

#include "IPlug_include_in_plug_hdr.h"


using namespace iplug;
using namespace iplug::igraphics;

class GraphControl11;
class IRadioButtonsControl;
class IGUIResizeButtonControl;

// TODO
#define VumeterControl IBKnobControl

class GUIHelper11
{
public:
    enum Style
    {
        STYLE_BLUELAB,
        STYLE_UST
    };
    
    enum Position
    {
        TOP,
        BOTTOM,
        LOWER_LEFT,
        LOWER_RIGHT
    };
    
    enum Size
    {
        SIZE_DEFAULT,
        SIZE_BIG
    };
    
    GUIHelper11(Style style);
    
    virtual ~GUIHelper11();
    
    IBKnobControl *CreateKnob(IGraphics *graphics,
                              float x, float y,
                              const char *bitmapFname, int nStates,
                              int paramIdx, const char *title = NULL,
                              Size titleSize = SIZE_DEFAULT,
                              ICaptionControl **caption = NULL);
    
#ifdef IGRAPHICS_NANOVG
    GraphControl11 *CreateGraph(Plugin *plug, IGraphics *graphics,
                                float x, float y,
                                const char *bitmapFname, int paramIdx,
                                int numCurves, int numPoints,
                                const char *overlayFname = NULL);
#endif // IGRAPHICS_NANOVG
    
    IBSwitchControl *CreateSwitchButton(IGraphics *graphics,
                                        float x, float y,
                                        const char *bitmapFname, int nStates,
                                        int paramIdx, const char *title = NULL);
    
    IBSwitchControl *CreateToggleButton(IGraphics *graphics,
                                        float x, float y,
                                        const char *bitmapFname,
                                        int paramIdx, const char *title = NULL);
    
    VumeterControl *CreateVumeter(IGraphics *graphics,
                                  float x, float y,
                                  const char *bitmapFname, int nStates,
                                  int paramIdx, const char *title = NULL);
    
    BLVumeterControl *CreateVumeterV(IGraphics *graphics,
                                     float x, float y,
                                     const char *bitmapFname,
                                     int paramIdx, const char *title = NULL);
    
    BLVumeterNeedleControl *CreateVumeterNeedleV(IGraphics *graphics,
                                                 float x, float y,
                                                 const char *bitmapFname,
                                                 int paramIdx, const char *title = NULL);
    
    ITextControl *CreateText(IGraphics *graphics,
                             float x, float y,
                             const char *textStr, float size,
                             const IColor &color, EAlign align,
                             float offsetX = 0.0, float offsetY = 0.0);

    
    IBitmapControl *CreateBitmap(IGraphics *graphics,
                                 float x, float y,
                                 const char *bitmapFname,
                                 //EAlign align = EAlign::Near,
                                 float offsetX = 0.0, float offsetY = 0.0,
                                 float *width = NULL, float *height = NULL);

    
    void CreateVersion(Plugin *plug, IGraphics *graphics,
                       const char *versionStr, Position pos);
    
    void CreateLogo(Plugin *plug, IGraphics *graphics,
                    const char *logoFname, Position pos);
    
    void CreateLogoAnim(Plugin *plug, IGraphics *graphics,
                        const char *logoFname,
                        int nStates, Position pos);
    
    void CreatePlugName(Plugin *plug, IGraphics *graphics,
                        const char *plugNameFname, Position pos);

    
    void CreateHelpButton(Plugin *plug, IGraphics *graphics,
                          const char *bmpFname,
                          const char *manualFileName,
                          Position pos);

    void CreateDemoMessage(IGraphics *graphics);
    
    IRadioButtonsControl *
        CreateRadioButtons(IGraphics *graphics,
                           float x, float y,
                           const char *bitmapFname,
                           int numButtons, float size, int paramIdx,
                           bool horizontalFlag, const char *title,
                           EAlign align = EAlign::Center,
                           EAlign titleAlign = EAlign::Center,
                           const char **radioLabels = NULL);
    
    IControl *CreateGUIResizeButton(ResizeGUIPluginInterface *plug,
                                    IGraphics *graphics,
                                    float x, float y,
                                    const char *bitmapFname,
                                    char *label,
                                    int resizeWidth, int resizeHeight);
    
    // NOTE: not sure it is still useful
    static void UpdateText(Plugin *plug, int paramIdx);
    
    static void ResetParameter(Plugin *plug, int paramIdx);
    
    // GUI resize
    static void GUIResizeParamChange(Plugin *plug, int paramNum,
                                     int params[], IGUIResizeButtonControl *buttons[],
                                     int guiWidth, int guiHeight,
                                     int numParams);
    
    static void GUIResizePreResizeGUI(IGraphics *pGraphics,
                                      IGUIResizeButtonControl *buttons[],
                                      int numButtons);
    
    static void GUIResizeComputeOffsets(int newGUIWidth,
                                        int newGUIHeight,
                                        int guiWidths[],
                                        int guiHeights[],
                                        int numSizes,
                                        int *offsetX,
                                        int *offsetY);
    
    static void GUIResizePostResizeGUI(Plugin *plug,
                                       GraphControl11 *graph,
                                       int graphWidthSmall,
                                       int graphHeightSmall,
                                       int offsetX, int offsetY);
    
    
protected:
    void CreateTitle(IGraphics *graphics, float x, float y,
                     const char *title, Size size,
                     EAlign align = EAlign::Center);
    
    ITextControl *CreateText(IGraphics *graphics, float x, float y,
                             const char *textStr, const IText &text,
                             float offsetX, float offsetY,
                             EAlign align = EAlign::Center);
    
    ICaptionControl *CreateValue(IGraphics *graphics, float x, float y,
                                 int paramIdx);
    
    ITextControl *CreateValueText(IGraphics *graphics, float x, float y,
                                  int paramIdx);

    float GetTextWidth(IGraphics *graphics, const IText &text, const char *textStr);
    
    ITextControl *CreateRadioLabelText(IGraphics *graphics,
                                       float x, float y,
                                       const char *textStr,
                                       const IBitmap &bitmap,
                                       const IText &text,
                                       EAlign align = EAlign::Near);
    
    
    //
    Style mStyle;
    
    bool mCreateTitles;
    
    float mTitleTextSize;
    float mTitleTextOffsetX;
    float mTitleTextOffsetY;
    IColor mTitleTextColor;
    
    float mTitleTextSizeBig;
    float mTitleTextOffsetXBig;
    float mTitleTextOffsetYBig;
    
    float mValueCaptionOffset;
    float mValueTextSize;
    float mValueTextOffsetX;
    float mValueTextOffsetY;
    IColor mValueTextColor;
    IColor mValueTextFGColor;
    IColor mValueTextBGColor;
    
    float mVersionTextSize;
    float mVersionTextOffsetX;
    float mVersionTextOffsetY;
    IColor mVersionTextColor;
    
    IColor mVumeterColor;
    IColor mVumeterNeedleColor;
    float mVumeterNeedleDepth;
    
    float mLogoOffsetX;
    float mLogoOffsetY;
    float mAnimLogoSpeed;
    
    float mPlugNameOffsetX;
    float mPlugNameOffsetY;
    
    float mTrialOffsetX;
    float mTrialOffsetY;
    
    float mHelpButtonOffsetX;
    float mHelpButtonOffsetY;
    
    float mDemoTextSize;
    float mDemoTextOffsetX;
    float mDemoTextOffsetY;
    IColor mDemoTextColor;
    
    float mRadioLabelTextSize;
    float mRadioLabelTextOffsetX;
    IColor mRadioLabelTextColor;
    
    float mButtonLabelTextOffsetX;
    float mButtonLabelTextOffsetY;
};

#endif /* GUIHelper11_hpp */
