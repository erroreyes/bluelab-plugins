#include "GhostFileDropControl.h"

void
GhostFilesDropControl::OnFilesDropped(const char *fileNames)
{
    // Assume there is only one file
    ((Ghost *)mPlug)->OpenFile(fileNames);
}
