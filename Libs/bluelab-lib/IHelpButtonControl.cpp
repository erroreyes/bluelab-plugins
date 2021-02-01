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
    //char cmd[1024];
    char cmd[2048];
    
#ifdef __APPLE__
    sprintf(cmd, "open \"%s\"", mFileName);
    
    system(cmd);
#endif
    
#ifdef WIN32

#ifndef VST3_API
    ShellExecute(NULL, "open", mFileName, NULL, NULL, SW_SHOWNORMAL);
#else
    ShellExecute(NULL, (LPCWSTR)"open", (LPCWSTR)mFileName, NULL, NULL, SW_SHOWNORMAL);
#endif

#endif

#ifdef __linux__
    sprintf(cmd, "xdg-open \"%s\"", mFileName);
    
    system(cmd);
#endif
    
    IBitmapControl::OnMouseDown(x, y, mod);
}
