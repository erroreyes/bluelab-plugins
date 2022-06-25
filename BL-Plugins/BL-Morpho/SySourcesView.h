#ifndef SY_SOURCES_VIEW_H
#define SY_SOURCES_VIEW_H

#include <BLUtilsFile.h>

#include <Morpho_defs.h>

// For listener
#include <IXYPadControlExt.h>

class SySource;
class SySourcesViewListener
{
public:
    virtual void SySourceChanged(const SySource *source) = 0;
};

//class IXYPadControlExt;
class SySourceManager;
class MorphoObj;
class IIconLabelControl;
class GUIHelper12;
class View3DPluginInterface;
class SySourcesView : public IXYPadControlExtListener
{
public:
    SySourcesView(Plugin *plug,
                  View3DPluginInterface *view3DListener,
                  MorphoObj *morphoObj,
                  SySourceManager *sourceManager);
    virtual ~SySourcesView();
    
    void SetListener(SySourcesViewListener *listener);
    
    void SetXYPad(IXYPadControlExt *xyPad);
    void SetIconLabel(IIconLabelControl *xyPad);

    void SetGUIHelper(GUIHelper12 *guiHelper);
    void SetWaterfallGUIParams(int graphX, int graphY,
                               const char *graphBitmapFN);
    
    // IXYPadControlListener
    void OnHandleChanged(int handleNum) override;
    
    void ClearGUI();
    void Refresh();

    void OnUIOpen();
    void OnUIClose();

    void OnIdle();

    void RecreateControls();
    
protected:
    void EnableCurrentView();
    
    //
    Plugin *mPlug;
    View3DPluginInterface *mView3DListener;
    MorphoObj *mMorphoObj;
    
    IXYPadControlExt *mXYPad;
    IIconLabelControl *mIconLabel;

    SySourcesViewListener *mListener;

    SySourceManager *mSourceManager;

    //
    GUIHelper12 *mGUIHelper;
    
    // Waterfall GUI
    int mWaterfallGraphX;
    int mWaterfallGraphY;
    char mWaterfallGraphBitmapFN[FILENAME_SIZE];
};

#endif
