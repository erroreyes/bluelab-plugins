//
//  IHelpButtonControl.cpp
//  BL-Rebalance-macOS
//
//  Created by applematuer on 10/5/20.
//
//

#include "IHelpButtonControl.h"

void
IHelpButtonControl::OnMouseDown(float x, float y, const IMouseMod &mod)
{
    // Don't forget that path can have white spaces inside!
    char cmd[1024];
    
#ifdef __APPLE__
    sprintf(cmd, "open \"%s\"", mFileName);
    
    system(cmd);
#endif
    
#ifdef WIN32
    ShellExecute(NULL, "open", mFileName, NULL, NULL, SW_SHOWNORMAL);
#endif
    
    IBitmapControl::OnMouseDown(x, y, mod);
}
