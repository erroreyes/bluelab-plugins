#include <GUIHelper11.h>
#include <IGUIResizeButtonControl.h>
#include <GraphControl11.h>

#include "ResizeGUIPluginInterface.h"

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

void
ResizeGUIPluginInterface::GUIResizeParamChange(int paramNum,
                                               int params[],
                                               IGUIResizeButtonControl *buttons[],
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
    
    int val = GetPlug()->GetParam(params[paramNum])->Int();
    if (val == 1)
    {
        // Reset the two other buttons
        
        // For the moment, keep the only case of Ableton Windows
        // (because we already have tested all plugs on Mac,
        // and half of the hosts on Windows)
        if (!winPlatform || !fixAbletonWin ||
            (GetPlug()->GetHost() != kHostAbletonLive))
        {
            for (int i = 0; i < numParams; i++)
            {
                if (i != paramNum)
                    GUIHelper11::ResetParameter(GetPlug(), params[i]);
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
    }
}

void
ResizeGUIPluginInterface::GUIResizePreResizeGUI(IGUIResizeButtonControl *buttons[],
                                                int numButtons)
{
    IGraphics *pGraphics = GetPlug()->GetUI();
    if (pGraphics == NULL)
        return;
    
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
ResizeGUIPluginInterface::GUIResizeComputeOffsets(int defaultGUIWidth,
                                                  int defaultGUIHeight,
                                                  int newGUIWidth,
                                                  int newGUIHeight,
                                                  int *offsetX,
                                                  int *offsetY)
{
    *offsetX = newGUIWidth - defaultGUIWidth;
    *offsetY = newGUIHeight - defaultGUIHeight;
}

#if 0
void
ResizeGUIPluginInterface::GUIResizePostResizeGUI(GraphControl11 *graph,
                                                 int graphWidthSmall,
                                                 int graphHeightSmall,
                                                 int offsetX, int offsetY)
{
    IGraphics *pGraphics = GetPlug()->GetUI();
    
    int newGraphWidth = graphWidthSmall + offsetX;
    int newGraphHeight = graphHeightSmall + offsetY;
    
    if (graph != NULL)
        graph->Resize(newGraphWidth, newGraphHeight);
    
    // Re-attach the graph
    if (graph != NULL)
        pGraphics->AttachControl(graph);
}
#endif
