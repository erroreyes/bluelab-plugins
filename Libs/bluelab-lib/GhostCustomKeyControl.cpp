#include "GhostCustomKeyControl.h"

GhostCustomKeyControl::GhostCustomKeyControl(GhostPluginInterface *plug)
{
    mPlug = plug;
}

bool
GhostCustomKeyControl::OnKeyDown(int x, int y, int key, IMouseMod* pMod)
{
#if !SA_API
    // Non-standalone version
    // => Disable keyboard shortcuts on plugin versions !
    
    return false;
#endif
    
#if 0
    fprintf(stderr, "key: %d\n", key);
#endif
    
    if (key == 0)
        // Spacebar
    {
        if (!mPlug->PlayStarted())
        {
            mPlug->StartPlay();

	    // TODO
	    
            // Synchronize the play button state
            mPlug->GetGUI()->SetParameterFromPlug(kPlayStop, 1, false);
        }
        else
        {
            mPlug->StopPlay();
            
            // Synchronize the play button state
            mPlug->GetGUI()->SetParameterFromPlug(kPlayStop, 0, false);
        }
        
        return true;
    }
    
    if (key == 41)
        // Return
    {
        mPlug->RewindView();
    }
    
    if ((key == 38) && pMod->Cmd) // cmd-x
    {
        mPlug->DoCutCommand();
    }
    
    if ((key == 16) && pMod->Cmd) // cmd-b
    {
        mPlug->DoGainCommand();
    }
    
#if !GHOST_LITE_VERSION || GHOST_LITE_ENABLE_REPLACE
    // cmd-w was not transmitted here
    if ((key == 28) && pMod->Cmd) // cmd-n
    {
        mPlug->DoReplaceCommand();
    }
#endif
    
#if !GHOST_LITE_VERSION || GHOST_LITE_ENABLE_COPY_PASTE
    if ((key == 17) && pMod->Cmd) // cmd-c
    {
        mPlug->DoCopyCommand();
    }
    
    if ((key == 36) && pMod->Cmd) // cmd-v
    {
        mPlug->DoPasteCommand();
    }
#endif
    
    if ((key == 40) && pMod->Cmd) // cmd-z
    {
        mPlug->UndoLastCommand();
    }
    
    return true;
}
