#include <ResizeGUIPluginInterface.h>

void
ResizeGUIPluginInterface::ApplyGUIResize(int guiSizeIdx)
{
    int newGUIWidth;
    int newGUIHeight;
    GetNewGUISize(guiSizeIdx, &newGUIWidth, &newGUIHeight);
    
    PreResizeGUI(newGUIWidth, newGUIHeight);
    
    if (mPlug->GetUI() != NULL)
        mPlug->GetUI()->Resize(newGUIWidth, newGUIHeight, 1.0f, true);
    
    PostResizeGUI();
}
