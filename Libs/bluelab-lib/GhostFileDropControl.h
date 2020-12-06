#ifndef GHOST_FILE_DROP_CONTROL_H
#define GHOST_FILE_DROP_CONTROL_H

class GhostFilesDropControl : public IFilesDropControl
{
public:
    GhostFilesDropControl(IPlugBase* pPlug)
    : IFilesDropControl(pPlug) {}
    
    virtual ~GhostFilesDropControl() {}
    
    void OnFilesDropped(const char *fileNames);
};

#endif
