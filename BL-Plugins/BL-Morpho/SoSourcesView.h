#ifndef SO_SOURCES_VIEW_H
#define SO_SOURCES_VIEW_H

#include <BLUtilsFile.h>

#include <Morpho_defs.h>

// For listener
#include <ITabsBarControl.h>

class SoSource;
class SoSourcesViewListener
{
public:
    virtual void SoSourceChanged(const SoSource *source) = 0;
    virtual void OnRemoveSource(int sourceNum) = 0;
};

//class ITabsBarControl;
class SoSourceManager;
class MorphoObj;
class GUIHelper12;
class View3DPluginInterface;
class SoSourcesView : public ITabsBarListener
{
 public:
    SoSourcesView(Plugin *plug,
                  View3DPluginInterface *view3DListener,
                  MorphoObj *morphoObj,
                  SoSourceManager *sourceManager);
    virtual ~SoSourcesView();

    void SetListener(SoSourcesViewListener *listener);
    
    void SetTabsBar(ITabsBarControl *tabsBar);

    void SetGUIHelper(GUIHelper12 *guiHelper);
    void SetSpectroGUIParams(int graphX, int graphY,
                             const char *graphBitmapFN);
    void SetWaterfallGUIParams(int graphX, int graphY,
                               const char *graphBitmapFN);
    
    // Create a new tab and a new corresponding source
    void CreateNewTab();

    // Tabs bar
    void NewTab();
    void SelectTab(int tabNum);
    
    void OnTabSelected(int tabNum) override;
    void OnTabClose(int tabNum) override;

    void ReCreateTabs(int numTabs);

    void ClearGUI();
    void Refresh();

    //
    void OnUIOpen();
    void OnUIClose();

    void OnIdle();

    // Re-create spectrograms when returning to So mode
    void RecreateControls();
    
 protected:
    void EnableCurrentView();
    
    //
    Plugin *mPlug;
    View3DPluginInterface *mView3DListener;
    MorphoObj *mMorphoObj;

    ITabsBarControl *mTabsBar;

    SoSourcesViewListener *mListener;
    
    SoSourceManager *mSourceManager;

    //
    GUIHelper12 *mGUIHelper;
    
    // Spectro GUI
    int mSpectroGraphX;
    int mSpectroGraphY;
    char mSpectroGraphBitmapFN[FILENAME_SIZE];

    // Waterfall GUI
    int mWaterfallGraphX;
    int mWaterfallGraphY;
    char mWaterfallGraphBitmapFN[FILENAME_SIZE];
};

#endif
