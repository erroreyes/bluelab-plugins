//
//  GUIHelpers.h
//  Transient
//
//  Created by Apple m'a Tuer on 03/06/17.
//
//

#ifndef __Transient__GUIHelpers2__
#define __Transient__GUIHelpers2__

#include "IPlug_include_in_plug_hdr.h"

#include <VuMeterControl.h>

#define TEXT_SIZE 14
#define TEXT_OFFSET 3

#define TEXT_OFFSET2 11

#define TEXT_COLOR_R 216
#define TEXT_COLOR_G 216
#define TEXT_COLOR_B 255


#define TEXT_FONT "Tahoma"


// New version of GUIHelpers, for version 2.0 of the plugins (new interface)
class GUIHelpers2
{
public:
    static ITextControl *CreateText(IPlug *plug, IGraphics *graphics, int width, int height, int x, int y,
                                    const char *label);
    
    // Create a knob with text. The text can be added to the graphics after, using AddText()
    static IKnobMultiControl *CreateKnobWithText(IPlug *plug, IGraphics *graphics, IBitmap *bitmap,
                                                int x, int y, int param, const char *label, IText **text = NULL, int yoffset = 0);
    
    static IKnobMultiControl *CreateEnumKnobWithText(IPlug *plug, IGraphics *graphics, IBitmap *bitmap,
                                                     int x, int y, int param,
                                                     double sensivity,
                                                     const char *label, IText **text = NULL, int yoffset = 0);
    
    static void AddText(IPlug *plug, IGraphics *graphics, IBitmap *bitmap, IText *text, int x, int y, int param);
    
    static void UpdateText(IPlug *plug, int paramIdx);
    
    static IToggleButtonControl *CreateToggleButtonWithText(IPlug *plug,
                                                            IGraphics *graphics, IBitmap *bitmap,
                                                            int x, int y, int param, const char *label);
    
    static VuMeterControl *CreateVuMeterWithText(IPlug *plug, IGraphics *graphics, IBitmap *bitmap,
                                                 int x, int y, int param, const char *label,
                                                 bool alignLeft);
    
    static int GetTextOffset();
    
private:
    static IColor mTextColor;
};

#endif /* defined(__Transient__GUIHelpers2__) */
