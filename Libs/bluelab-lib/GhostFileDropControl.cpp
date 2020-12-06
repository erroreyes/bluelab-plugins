#include <GhostPluginInterface.h>

#include "GhostFileDropControl.h"

void
GhostFilesDropControl::OnFilesDropped(const char *fileNames)
{
    // Assume there is only one file
    ((GhostPluginInterface *)mPlug)->OpenFile(fileNames);
}
