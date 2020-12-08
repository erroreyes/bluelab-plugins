#include <GhostPluginInterface.h>

#include "GhostFileDropControl.h"

void
//GhostFilesDropControl::OnFilesDropped(const char *fileNames)
GhostFileDropControl::OnDrop(const char *fileNames)
{
    // Assume there is only one file
    //((GhostPluginInterface *)mPlug)->OpenFile(fileNames);
    mPlug->OpenFile(fileNames);
}
