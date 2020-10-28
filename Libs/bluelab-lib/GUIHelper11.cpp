//
//  GUIHelper11.cpp
//  UST-macOS
//
//  Created by applematuer on 9/25/20.
//
//

#include <lice.h>

#include <GraphControl11.h>
#include <BLUtils.h>

#include <IBitmapControlAnim.h>
#include <IHelpButtonControl.h>

#include "GUIHelper11.h"

// Text
//#define TITLE_TEXT_FONT "font-bold"
//#define VALUE_TEXT_FONT "font-bold"
//#define VERSION_TEXT_FONT "font-bold"
#define TITLE_TEXT_FONT "font-regular"
#define VALUE_TEXT_FONT "font-regular"
#define VERSION_TEXT_FONT "font-regular"

#define TEXTFIELD_BITMAP "textfield.png"


GUIHelper11::GUIHelper11(Style style)
{
    mStyle = style;
    
    if (style == STYLE_UST)
    {
        mCreateTitles = false;
        
        mTitleTextSize = 13.0;
        mTitleTextOffsetX = 0.0;
        mTitleTextOffsetY = -18.0;
        mTitleTextColor = IColor(255, 110, 110, 110);
        
        mValueCaptionOffset = 0.0;
        mValueTextSize = 14.0;
        mValueTextOffsetX = -1.0;
        mValueTextOffsetY = 1.0;
        mValueTextColor = IColor(255, 240, 240, 255);
        mValueTextFGColor = mValueTextColor;
        mValueTextBGColor = IColor(0, 0, 0, 0);
        
        mVersionTextSize = 12.0;
        //mVersionTextOffset = 3.0;
        mVersionTextOffsetX = 100.0;
        mVersionTextOffsetY = 3.0;
        mVersionTextColor = IColor(255, 110 , 110, 110);
        
        mVumeterColor = IColor(255, 131 , 152, 214);
        mVumeterNeedleColor = IColor(255, 237, 120, 31);
        mVumeterNeedleDepth = 2.0; //4.0;
    }
    
    if (style == STYLE_BLUELAB)
    {
        mCreateTitles = true;
        
        mTitleTextSize = 16.0; //20.0; //13.0;
        mTitleTextOffsetX = 0.0;
        mTitleTextOffsetY = -23.0;
        mTitleTextColor = IColor(255, 110, 110, 110);
        
        mTitleTextSizeBig = 20.0;
        mTitleTextOffsetXBig = 0.0;
        mTitleTextOffsetYBig = -32.0; //-23.0;
        
        mValueCaptionOffset = 0.0;
        mValueTextSize = 16.0; //14.0;
        mValueTextOffsetX = -1.0;
        mValueTextOffsetY = 14.0; //13.0;
        mValueTextColor = IColor(255, 240, 240, 255);
        mValueTextFGColor = mValueTextColor;
        mValueTextBGColor = IColor(0, 0, 0, 0);
        
        mVersionTextSize = 12.0;
        //mVersionTextOffset = 3.0;
        mVersionTextOffsetX = 100.0;
        mVersionTextOffsetY = 3.0;
        mVersionTextColor = IColor(255, 110 , 110, 110);
        
        mVumeterColor = IColor(255, 131 , 152, 214);
        mVumeterNeedleColor = IColor(255, 237, 120, 31);
        mVumeterNeedleDepth = 2.0; //4.0;
        
        mLogoOffsetX = 0.0;
        mLogoOffsetY = -1.0;
        mAnimLogoSpeed = 0.5;
        
        mPlugNameOffsetX = 5.0;
        mPlugNameOffsetY = 6.0;
        
        mTrialOffsetX = 0.0;
        mTrialOffsetY = 7.0;
        
        mHelpButtonOffsetX = -44.0;
        mHelpButtonOffsetY = -4.0;
    }
}

GUIHelper11::~GUIHelper11() {}

IBKnobControl *
GUIHelper11::CreateKnob(IGraphics *graphics,
                        float x, float y,
                        const char *bitmapFname, int nStates,
                        int paramIdx, const char *title,
                        Size titleSize,
                        ICaptionControl **caption)
{
    IBitmap bitmap = graphics->LoadBitmap(bitmapFname, nStates);
    IBKnobControl *knob = new IBKnobControl(x, y, bitmap, paramIdx);
    graphics->AttachControl(knob);
    
    if (mCreateTitles)
    {
        // Title
        if ((title != NULL) && (strlen(title) > 0))
        {
            CreateTitle(graphics, x + bitmap.W()/2, y, title, titleSize);
        }
    }
    
    ICaptionControl *caption0 =
                CreateValue(graphics,
                            x + bitmap.W()/2, y + bitmap.H()/bitmap.N(),
                            paramIdx);
    if (caption != NULL)
        *caption = caption0;
    
    return knob;
}

#ifdef IGRAPHICS_NANOVG
GraphControl11 *
GUIHelper11::CreateGraph(Plugin *plug, IGraphics *graphics,
                         float x, float y,
                         const char *bitmapFname, int paramIdx,
                         int numCurves, int numPoints,
                         const char *overlayFname)
{
    IBitmap bitmap = graphics->LoadBitmap(bitmapFname);
    
    const char *resPath = graphics->GetSharedResourcesSubPath();
    char fontPath[MAX_PATH];
    sprintf(fontPath, "%s/%s", resPath, "font.ttf");
    
    IRECT rect(x, y, x + bitmap.W(), y + bitmap.H());
    GraphControl11 *graph = new GraphControl11(plug, graphics, rect, paramIdx,
                                               numCurves, numPoints, fontPath);
    
    graph->SetBackgroundImage(bitmap);
    
    if (overlayFname != NULL)
    {
        char bmpPath[MAX_PATH];
        sprintf(bmpPath, "%s/%s", resPath, overlayFname);
        
        IBitmap overlayBitmap = graphics->LoadBitmap(bmpPath);
        
        graph->SetOverlayImage(overlayBitmap);
    }
    
    graphics->AttachControl(graph);
    
    return graph;
}
#endif // IGRAPHICS_NANOVG

IBSwitchControl *
GUIHelper11::CreateSwitchButton(IGraphics *graphics,
                                float x, float y,
                                const char *bitmapFname, int nStates,
                                int paramIdx, const char *title)
{
    IBitmap bitmap = graphics->LoadBitmap(bitmapFname, nStates);
    
    IBSwitchControl *button = new IBSwitchControl(x, y, bitmap, paramIdx);
    graphics->AttachControl(button);
    
    return button;
}

IBSwitchControl *
GUIHelper11::CreateToggleButton(IGraphics *graphics,
                                float x, float y,
                                const char *bitmapFname,
                                int paramIdx, const char *title)
{
    IBSwitchControl *button = CreateSwitchButton(graphics, x, y,
                                                 bitmapFname, 2,
                                                 paramIdx, title);
    
    return button;
}

VumeterControl *
GUIHelper11::CreateVumeter(IGraphics *graphics,
                           float x, float y,
                           const char *bitmapFname, int nStates,
                           int paramIdx, const char *title)
{
    // TODO
    /*VumeterControl *vumeter = CreateKnob(graphics,
                                         x, y,
                                         bitmapFname, nStates,
                                         paramIdx, title);*/
    
    //graphics->AttachControl(vumeter);
    
    IBitmap bitmap = graphics->LoadBitmap(bitmapFname, nStates);
    VumeterControl *vumeter = new IBKnobControl(x, y, bitmap, paramIdx);
    vumeter->SetInteractionDisabled(true);
    
    graphics->AttachControl(vumeter);
    
    return vumeter;
}

BLVumeterControl *
GUIHelper11::CreateVumeterV(IGraphics *graphics,
                            float x, float y,
                            const char *bitmapFname,
                            int paramIdx, const char *title)

{
    // Background bitmap
    float width;
    float height;
    CreateBitmap(graphics,
                 x, y,
                 bitmapFname,
                 0.0f, 0.0f,
                 &width, &height);
    
    IRECT rect(x, y, x + width, y + height);
    BLVumeterControl *result = new BLVumeterControl(rect,
                                                    mVumeterColor,
                                                    paramIdx);
    result->SetInteractionDisabled(true);
    graphics->AttachControl(result);
    
    return result;
}

BLVumeterNeedleControl *
GUIHelper11::CreateVumeterNeedleV(IGraphics *graphics,
                                  float x, float y,
                                  const char *bitmapFname,
                                  int paramIdx, const char *title)

{
    // Background bitmap
    float width;
    float height;
    CreateBitmap(graphics,
                 x, y,
                 bitmapFname,
                 0.0f, 0.0f,
                 &width, &height);
    
    IRECT rect(x, y, x + width, y + height);
    BLVumeterNeedleControl *result = new BLVumeterNeedleControl(rect,
                                                                mVumeterNeedleColor,
                                                                mVumeterNeedleDepth,
                                                                paramIdx);
    result->SetInteractionDisabled(true);
    graphics->AttachControl(result);
    
    return result;
    
}

ITextControl *
GUIHelper11::CreateText(IGraphics *graphics, float x, float y,
                        const char *textStr, float size,
                        const IColor &color, EAlign align,
                        float offsetX, float offsetY)
{
    IText text(size, color, TITLE_TEXT_FONT, align);
    
    float width = GetTextWidth(graphics, text, textStr);
    
    IRECT rect(x + offsetX, y + offsetY,
               (x + offsetX + width), (y  + size + offsetY));

    
    ITextControl *textControl = new ITextControl(rect, textStr, text);
    
    graphics->AttachControl(textControl);
    
    return textControl;
}

IBitmapControl *
GUIHelper11::CreateBitmap(IGraphics *graphics,
                          float x, float y,
                          const char *bitmapFname,
                          //EAlign align,
                          float offsetX, float offsetY,
                          float *width, float *height)
{
    IBitmap bitmap = graphics->LoadBitmap(bitmapFname, 1);
    
    if (width != NULL)
        *width = bitmap.W();
    if (height != NULL)
        *height = bitmap.H()/bitmap.N();;
    
    IBitmapControl *result = new IBitmapControl(x + offsetX, y + offsetY, bitmap);
    result->SetInteractionDisabled(true);
    graphics->AttachControl(result);
    
    return result;
}

void
GUIHelper11::CreateVersion(Plugin *plug, IGraphics *graphics,
                           const char *versionStr, Position pos)
{
    // Lower left corner
    char versionStr0[256];
    sprintf(versionStr0, "v%s", versionStr);
    
    float x  = 0.0;
    float y  = 0.0;
    
    EAlign textAlign = EAlign::Near;
    
    if (pos == LOWER_LEFT)
    {
        x = mVersionTextOffsetX;
        y = graphics->Height() - mVersionTextSize - mVersionTextOffsetY;
        
        textAlign = EAlign::Near;
    }
    
    if (pos == LOWER_RIGHT)
    {
        int strWidth = (int)(strlen(versionStr0)*mVersionTextSize);
        
        x = graphics->Width() - strWidth/2 - mVersionTextOffsetX + mVersionTextSize/2;
        x += mVersionTextSize/2;
        
        y = graphics->Height() - mVersionTextOffsetY - mVersionTextSize;
        
        textAlign = EAlign::Near;
    }
    
    IText versionText(mVersionTextSize,
                      mVersionTextColor, VERSION_TEXT_FONT, textAlign);
    
    if (pos == BOTTOM)
    {
        float textWidth = GetTextWidth(graphics, versionText, versionStr0);
        
        x = graphics->Width()/2 - textWidth/2.0;
        
        y = graphics->Height() - mVersionTextSize - mVersionTextOffsetY;
        
        //textAlign = EAlign::Center;
    }
    
    float textWidth = GetTextWidth(graphics, versionText, versionStr0);
    
    IRECT rect(x, y, x + textWidth, y + mVersionTextSize);
    
    ITextControl *textControl = new ITextControl(rect, versionStr0, versionText);
    
    graphics->AttachControl(textControl);
}

// Static logo
void
GUIHelper11::CreateLogo(Plugin *plug, IGraphics *graphics,
                        const char *logoFname, Position pos)
{
    IBitmap bmp = graphics->LoadBitmap(logoFname, 1);
    
    float x = 0.0;
    float y = 0.0;
    
    if (pos == TOP)
    {
        // Upper right corner
        x = graphics->Width() - bmp.W();
        y = 0;
    }
    
    if (pos == BOTTOM)
    {
        // Lower right corner
        x = graphics->Width() - bmp.W();
        y = graphics->Height() - bmp.H();
    }
    
    IBitmapControl *control = new IBitmapControl(x + mLogoOffsetX,
                                                 y + mLogoOffsetY, bmp);
    control->SetInteractionDisabled(true);
    
    graphics->AttachControl(control);
}

// Static logo
void
GUIHelper11::CreateLogoAnim(Plugin *plug, IGraphics *graphics,
                            const char *logoFname, int nStates, Position pos)
{
    IBitmap bmp = graphics->LoadBitmap(logoFname, nStates);
    
    float x = 0.0;
    float y = 0.0;
    
    if (pos == TOP)
    {
        // Upper right corner
        x = graphics->Width() - bmp.W();
        y = 0;
    }
    
    if (pos == BOTTOM)
    {
        // Lower right corner
        x = graphics->Width() - bmp.W();
        y = graphics->Height() - bmp.H()/bmp.N();
    }
    
    IBitmapControlAnim *control = new IBitmapControlAnim(x + mLogoOffsetX,
                                                         y + mLogoOffsetY,
                                                         bmp, kNoValIdx,
                                                         mAnimLogoSpeed,
                                                         true, true);
    control->SetInteractionDisabled(true);
    
    graphics->AttachControl(control);
}

void
GUIHelper11::CreateHelpButton(Plugin *plug, IGraphics *graphics,
                              const char *bmpFname,
                              const char *manualFileName,
                              Position pos)
{
    IBitmap bitmap = graphics->LoadBitmap(bmpFname, 1);
    
    float x = 0.0;
    float y = 0.0;
    
    //if (pos == TOP) // TODO
    
    if (pos == BOTTOM)
    {
        // Lower right corner
        x = graphics->Width() - bitmap.W() + mHelpButtonOffsetX;
        y = graphics->Height() - bitmap.H() + mHelpButtonOffsetY;
    }
    
    char fullFileName[1024];
    
#ifndef WIN32 // Mac
    WDL_String wdlResDir;
    BLUtils::GetFullPlugResourcesPath(*plug, &wdlResDir);
    const char *resDir = wdlResDir.Get();
    
    sprintf(fullFileName, "%s/%s", resDir, manualFileName);
#else
    // On windows, we must load the resource from dll, save it to the temp file
    // before re-opening it
#if 0 // iPlug1
    IGraphicsWin *graphWin = (IGraphicsWin *)graphics;
    void *resBuf;
    long resSize;
    bool res = graphWin->LoadWindowsResource(manResId, "PDF",
                                             &resBuf, &resSize);
    if (!res)
        return;
#endif

    WDL_TypedBuf<uint8_t> resBuf = graphics->LoadResource(MANUAL_FN, "RCDATA");
    long resSize = resBuf.GetSize();
    if (resSize == 0)
        return;

    TCHAR tempPathBuffer[MAX_PATH];
    GetTempPath(MAX_PATH, tempPathBuffer);

#if 0 // iPlug1
    // Remove the "manual" directory from the path
    fileName = BLUtils::GetFileName(fileName);
    sprintf(fullFileName, "%s%s", tempPathBuffer, fileName);
#endif

    sprintf(fullFileName, "%s%s", tempPathBuffer, MANUAL_FN);
    
    FILE *file = fopen(fullFileName, "wb");
    fwrite(resBuf.Get(), 1, resSize, file);
    fclose(file);
#endif
    
    IBitmapControl *control = new IHelpButtonControl(x, y, bitmap,
                                                     kNoValIdx,
                                                     fullFileName);
    
#if GUI_OBJECTS_SORTING
    mBackObjects.push_back(control);
#else
    graphics->AttachControl(control);
#endif
}


void
GUIHelper11::CreatePlugName(Plugin *plug, IGraphics *graphics,
                            const char *plugNameFname, Position pos)
{
    IBitmap bmp = graphics->LoadBitmap(plugNameFname, 1);
    
    float x = 0;
    float y = 0;
    
    if (pos == TOP)
    {
        // Upper left corner
        x = mPlugNameOffsetX;
        y = mPlugNameOffsetY;
    }
    
    if (pos == BOTTOM)
    {
        // Lower left corner
        x = mPlugNameOffsetX;
        y = graphics->Height() - bmp.H() - mPlugNameOffsetY;
        
        y -= mTrialOffsetY;
    }
    
    IBitmapControl *control = new IBitmapControl(x, y, bmp);
    control->SetInteractionDisabled(true);
    
    graphics->AttachControl(control);
}

void
GUIHelper11::UpdateText(Plugin *plug, int paramIdx)
{
    // bl-iplug2: makes infinite loop with OnParamChange()
    //double normValue = plug->GetParam(paramIdx)->Value();
    //plug->SetParameterValue(paramIdx, normValue);
}

void
GUIHelper11::CreateTitle(IGraphics *graphics, float x, float y,
                         const char *title, Size size)
{
    float titleSize = mTitleTextSize;
    float textOffsetX = mTitleTextOffsetX;
    float textOffsetY = mTitleTextOffsetY;
    
    if (size == SIZE_BIG)
    {
        titleSize = mTitleTextSizeBig;
        textOffsetX = mTitleTextOffsetXBig;
        textOffsetY = mTitleTextOffsetYBig;
    }
    
    IText text(titleSize, mTitleTextColor, TITLE_TEXT_FONT, EAlign::Center);
    float width = GetTextWidth(graphics, text, title);
    
    x -= width/2.0;
    
    ITextControl *control = CreateText(graphics, x, y,
                                       title, text,
                                       textOffsetX, textOffsetY);
    
    control->SetInteractionDisabled(true);
    
    graphics->AttachControl(control);
}

float
GUIHelper11::GetTextWidth(IGraphics *graphics, const IText &text, const char *textStr)
{
    // Measure
    IRECT textRect;
    graphics->MeasureText(text, (char *)textStr, textRect);

    float width = textRect.W();

    return width;
}

ITextControl *
GUIHelper11::CreateText(IGraphics *graphics, float x, float y,
                        const char *textStr, const IText &text,
                        float offsetX, float offsetY)
{
    float width = GetTextWidth(graphics, text, textStr);
    
    IRECT rect(x + offsetX, y + offsetY,
               (x + offsetX + width), (y  + mTitleTextSize + offsetY));
    
    ITextControl *textControl = new ITextControl(rect, textStr, text);
    
    graphics->AttachControl(textControl);
    
    return textControl;
}

ICaptionControl *
GUIHelper11::CreateValue(IGraphics *graphics, float x, float y,
                         int paramIdx)
{
    // Bitmap
    float width;
    float height;
    CreateBitmap(graphics, x, y, TEXTFIELD_BITMAP,
                 mValueTextOffsetX, mValueTextOffsetY,
                 &width, &height);

    // Value
    IRECT bounds(x - width/2.0 + mValueTextOffsetX,
                 y + mValueTextOffsetY,
                 x + width/2.0 + mValueTextOffsetX,
                 y + height + mValueTextOffsetY);
    
    IText text(mValueTextSize, mValueTextColor, VALUE_TEXT_FONT,
               EAlign::Center, EVAlign::Middle, 0.0,
               mValueTextBGColor, mValueTextFGColor);
    ICaptionControl *caption = new ICaptionControl(bounds, paramIdx, text,
                                                   mValueTextBGColor);
    caption->DisablePrompt(false); // Here is the magic !
    graphics->AttachControl(caption);
    
    return caption;
}
