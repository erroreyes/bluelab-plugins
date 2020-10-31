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
#include <IRadioButtonsControl.h>
#include <IGUIResizeButtonControl.h>

#include "GUIHelper11.h"

// Text
//#define TITLE_TEXT_FONT "font-bold"
//#define VALUE_TEXT_FONT "font-bold"
//#define VERSION_TEXT_FONT "font-bold"
#define TITLE_TEXT_FONT "font-regular"
#define VALUE_TEXT_FONT "font-regular"
#define VERSION_TEXT_FONT "font-regular"

#define TEXTFIELD_BITMAP "textfield.png"

// Demo mode
#if DEMO_VERSION
#define DEMO_MODE 1
#endif

#ifndef DEMO_MODE
#define DEMO_MODE 0 // 1
#endif

#define DEMO_MESSAGE "[DEMO] Please consider buying if you like it!"

// Ableton, Windows
// - play
// - change to medium GUI
// - change to small GUI
// => after a little while, the medium GUI button is selected again automatically,
// and the GUI starts to resize, then it freezes
//
// FIX: use SetValueFromUserInput() to avoid a call by the host to VSTSetParameter()
// NOTE: may still freeze some rare times
#define FIX_ABLETON_RESIZE_GUI_FREEZE 1

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
        
        mDemoTextSize = 10.0;
        mDemoTextOffsetX = 2.0;
        mDemoTextOffsetY = 2.0;
        mDemoTextColor = IColor(255, 200, 0, 0);
        
        mRadioLabelTextSize = 15;
        mRadioLabelTextOffsetX = 6.0;
        mRadioLabelTextColor = IColor(255, 100, 100, 161);
        
        mButtonLabelTextOffsetX = 3.0;
        mButtonLabelTextOffsetY = 3.0;
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
        *height = bitmap.H()/bitmap.N();
    
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
GUIHelper11::CreateDemoMessage(IGraphics *graphics)
{
#if DEMO_MODE
    int x = mDemoTextOffsetX;
    int y = graphics->Height() - mDemoTextSize - mDemoTextOffsetY;
    
    ITextControl *textControl = CreateText(graphics,
                                           x, y,
                                           DEMO_MESSAGE,
                                           mDemoTextSize,
                                           mDemoTextColor, EAlign::Near);
    
    // Avoids that the trial message masks the interaction on the help button for example
    textControl->SetInteractionDisabled(true);
#endif
}

IRadioButtonsControl *
GUIHelper11::CreateRadioButtons(IGraphics *graphics,
                                float x, float y,
                                const char *bitmapFname,
                                int numButtons, float size, int paramIdx,
                                bool horizontalFlag, const char *title,
                                EAlign align,
                                EAlign titleAlign,
                                const char **radioLabels)
{
    IBitmap bitmap = graphics->LoadBitmap(bitmapFname, 2);
    
    float titleX = x;
    if (titleAlign == EAlign::Center)
        titleX = x + bitmap.W()/2;
    else if (titleAlign == EAlign::Far)
        titleX = x + bitmap.W();
    
    // Title
    CreateTitle(graphics, titleX, y, title, SIZE_DEFAULT, titleAlign);
    
    // Radio labels
    if (numButtons > 0)
    {
        if (!horizontalFlag)
        {
            int labelX = x;
            if (align == EAlign::Far)
                labelX = labelX - bitmap.W() - mRadioLabelTextOffsetX;
            
            if (align == EAlign::Near)
            {
                labelX = labelX + bitmap.W() + mRadioLabelTextOffsetX;
            }
            
            int spaceBetween = (size - numButtons*bitmap.H()/2)/(numButtons - 1);
            int stepSize = bitmap.H()/2 + spaceBetween;
            
            for (int i = 0; i < numButtons; i++)
            {
                IText labelText(mRadioLabelTextSize,
                                mRadioLabelTextColor,
                                TITLE_TEXT_FONT,
                                align);
                
                int labelY = y + i*stepSize;
                const char *labelStr = radioLabels[i];
                CreateRadioLabelText(graphics,
                                     labelX, labelY,
                                     labelStr,
                                     bitmap,
                                     labelText,
                                     align);
            }
        }
        else
        {
            // NOTE: not really tested
            
            int labelY = x;
            //if (titleAlign == IText::kAlignFar)
            //    labelX = labelX - bitmap.W - RADIO_LABEL_TEXT_OFFSET;
            
            if (align == EAlign::Near)
            {
                // TODO: implement this
            }
            
            int spaceBetween = (size - numButtons*bitmap.H()/2)/(numButtons - 1);
            int stepSize = bitmap.H()/2 + spaceBetween;
            
            for (int i = 0; i < numButtons; i++)
            {
                // new
                IText labelText(mRadioLabelTextSize,
                                mRadioLabelTextColor,
                                TITLE_TEXT_FONT,
                                align);
                
                int labelX = x + i*stepSize;
                const char *labelStr = radioLabels[i];
                CreateRadioLabelText(graphics,
                                     labelX, labelY,
                                     labelStr,
                                     bitmap,
                                     labelText,
                                     align);
            }
        }
    }
    
    // Buttons
    IRECT rect(x, y, x + size, y + bitmap.H());
    EDirection direction = EDirection::Horizontal;
    if (!horizontalFlag)
    {
        rect = IRECT(x, y, x + bitmap.W(), y + size);
        
        direction = EDirection::Vertical;
    }
    
    IRadioButtonsControl *control =
        new IRadioButtonsControl(rect,
                                 paramIdx, numButtons,
                                 bitmap, direction);
    
    graphics->AttachControl(control);
    
    return control;
}


void
GUIHelper11::UpdateText(Plugin *plug, int paramIdx)
{
    // bl-iplug2: makes infinite loop with OnParamChange()
    //double normValue = plug->GetParam(paramIdx)->Value();
    //plug->SetParameterValue(paramIdx, normValue);
}

void
GUIHelper11::ResetParameter(Plugin *plug, int paramIdx)
{
    if (plug->GetUI())
    {
        plug->GetParam(paramIdx)->SetToDefault();
        
        plug->SendParameterValueFromAPI(paramIdx, plug->GetParam(paramIdx)->Value(), false);
    }
}

void
GUIHelper11::GUIResizeParamChange(Plugin *plug, int paramNum,
                                  int params[], IGUIResizeButtonControl *buttons[],
                                  int guiWidth, int guiHeight,
                                  int numParams)
{
    // For fix Ableton Windows resize GUI
    bool winPlatform = false;
#ifdef WIN32
    winPlatform = true;
#endif
    
    bool fixAbletonWin = false;
#if FIX_ABLETON_RESIZE_GUI_FREEZE
    fixAbletonWin = true;
#endif
    
    int val = plug->GetParam(params[paramNum])->Int();
    if (val == 1)
    {
        // Reset the two other buttons
        
        // For the moment, keep the only case of Ableton Windows
        // (because we already have tested all plugs on Mac,
        // and half of the hosts on Windows)
        if (!winPlatform || !fixAbletonWin || (plug->GetHost() != kHostAbletonLive))
        {
            for (int i = 0; i < numParams; i++)
            {
                if (i != paramNum)
                    GUIHelper11::ResetParameter(plug, params[i]);
            }
        }
        else
            // Ableton Windows + fix enabled
        {
            for (int i = 0; i < numParams; i++)
            {
                if (i != paramNum)
                    buttons[i]->SetValueFromUserInput(0.0);
            }
        }
        
        // When initializing the plugin, the previous size was not set correctly
        // But without the following test, clicking on a size button made an infinite loop
        if ((buttons[paramNum] != NULL) &&
            (!buttons[paramNum]->IsMouseClicking()))
        {
            // If size will not change, OnWindowResize() won't be called,
            // then we need to avoid detaching graph in PreResizeGUI()
            IGraphics *pGraphics = plug->GetUI();
            int width = pGraphics->Width();
            int height = pGraphics->Height();
            
#if 0 // #bluelab: todo re-enable it!
            bool needDetachGraph = ((width != guiWidth) || (height != guiHeight));
            
            plug->GUIResizeSetNeedDetachGraph(needDetachGraph);
            
            plug->ResizeGUI(guiWidth, guiHeight);
            
            plug->GUIResizeSetNeedDetachGraph(true);
#endif
        }
    }
}

void
GUIHelper11::GUIResizePreResizeGUI(IGraphics *pGraphics,
                                   IGUIResizeButtonControl *buttons[],
                                   int numButtons)
{
    // Avoid memory corruption:
    // ResizeGUI() is called from the buttons
    // And during ResizeGUI(), the previous controls are deleted
    // (including the buttons)
    // Then we will delete button from its own code in OnMouseClick()
    // if the buttons are still attached
    bool isMouseClicking = false;
    for (int i = 0; i < numButtons; i++)
    {
        if ((buttons[i] != NULL) && buttons[i]->IsMouseClicking())
        {
            isMouseClicking = true;
            
            break;
        }
    }
    
    if (isMouseClicking)
    {
        for (int i = 0; i < numButtons; i++)
        {
            if (buttons[i] != NULL)
            {
                pGraphics->DetachControl(buttons[i]);
            }
        }
    }
}

void
GUIHelper11::GUIResizeComputeOffsets(int newGUIWidth,
                                     int newGUIHeight,
                                     int guiWidths[],
                                     int guiHeights[],
                                     int numSizes,
                                     int *offsetX,
                                     int *offsetY)
{
    if (numSizes == 0)
        return;
    
    // Other controls
    *offsetX = 0;
    for (int i = 0; i < numSizes; i++)
    {
        if (newGUIWidth == guiWidths[i])
        {
            *offsetX = guiWidths[i] - guiWidths[0];
            
            break;
        }
    }
    
    *offsetY = 0;
    for (int i = 0; i < numSizes; i++)
    {
        if (newGUIHeight == guiHeights[i])
        {
            *offsetY = guiHeights[i] - guiHeights[0];
            
            break;
        }
    }
}

void
GUIHelper11::GUIResizePostResizeGUI(Plugin *plug,
                                    GraphControl11 *graph,
                                    int graphWidthSmall,
                                    int graphHeightSmall,
                                    int offsetX, int offsetY)
{
    IGraphics *pGraphics = plug->GetUI();
    
    int newGraphWidth = graphWidthSmall + offsetX;
    int newGraphHeight = graphHeightSmall + offsetY;
    
    if (graph != NULL)
        graph->Resize(newGraphWidth, newGraphHeight);
    
    // Re-attach the graph
    if (graph != NULL)
        pGraphics->AttachControl(graph);
    
#if 0 // #bluelab: todo; re-enable this
    //RefreshControlValues();
    pGraphics->RefreshAllControlsValues();
#endif
}

IControl *
GUIHelper11::CreateGUIResizeButton(ResizeGUIPluginInterface *plug, IGraphics *graphics,
                                   float x, float y,
                                   const char *bitmapFname,
                                   int paramIdx,
                                   char *label,
                                   int resizeWidth, int resizeHeight)
{
    int bmpFrames = 3;
    
    IBitmap bitmap = graphics->LoadBitmap(bitmapFname, bmpFrames);
    
    IRECT pR;
    pR.L = x;
    pR.T = y;
    pR.R = x + bitmap.W();
    pR.B = y + bitmap.H();
    
    // With rollover
    IControl *control =
        new IGUIResizeButtonControl(plug, x, y, bitmap,
                                    paramIdx,
                                    resizeWidth, resizeHeight);
    
    graphics->AttachControl(control);
    
    // Add the label
    CreateTitle(graphics,
                x + mButtonLabelTextOffsetX,
                y + bitmap.H()*1.5/((BL_FLOAT)bmpFrames) + mButtonLabelTextOffsetY,
                label, Size::SIZE_DEFAULT);
    
    return control;
}

void
GUIHelper11::RefreshAllParameters(Plugin *plug, int numParams)
{
    for (int i = 0; i < numParams; i++)
        plug->SendParameterValueFromAPI(i, plug->GetParam(i)->Value(), false);
}

void
GUIHelper11::CreateTitle(IGraphics *graphics, float x, float y,
                         const char *title, Size size, EAlign align)
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
    
    IText text(titleSize, mTitleTextColor, TITLE_TEXT_FONT, align);
    float width = GetTextWidth(graphics, text, title);
    
    if (align == EAlign::Center)
        x -= width/2.0;
    else if (align == EAlign::Far)
        x -= width;
    
    ITextControl *control = CreateText(graphics, x, y,
                                       title, text,
                                       textOffsetX, textOffsetY);
    
    control->SetInteractionDisabled(true);
    
    //graphics->AttachControl(control);
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
                        float offsetX, float offsetY,
                        EAlign align)
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

ITextControl *
GUIHelper11::CreateRadioLabelText(IGraphics *graphics,
                                  float x, float y,
                                  const char *textStr,
                                  const IBitmap &bitmap,
                                  const IText &text,
                                  EAlign align)
{
    float width = GetTextWidth(graphics, text, textStr);
    
    // Adjust for aligne center
    float dx = 0.0;
    
    // Adjust for align far
    if (align == EAlign::Far)
    {
        int bmpWidth = bitmap.W();
        dx = -width + bmpWidth;
    }
    
    IRECT rect(x + dx, y,
               (x + width) + dx, y + text.mSize);
    
    ITextControl *control = new ITextControl(rect, textStr, text);
    
    graphics->AttachControl(control);
    
    return control;
}
