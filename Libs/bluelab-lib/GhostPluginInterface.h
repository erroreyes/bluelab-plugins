#ifndef GHOST_PLUGIN_INTERFACE_H
#define GHOST_PLUGIN_INTERFACE_H

#include <BLTypes.h>

// Zoom on the pointer inside of on the bar (for zoom center)
#define ZOOM_ON_POINTER 1

class GhostPluginInterface
{
 public:
  enum PlugMode
  {
    VIEW = 0,
    ACQUIRE,
    EDIT,
    RENDER
  };
  
  enum SelectionType
  {
    RECTANGLE = 0,
    HORIZONTAL,
    VERTICAL
  };
    
  GhostPluginInterface();
  virtual ~GhostPluginInterface();
    
  virtual void UpdateSelection(BL_FLOAT x0, BL_FLOAT y0,
                               BL_FLOAT x1, BL_FLOAT y1,
                               bool updateCenterPos,
                               bool activateDrawSelection = false,
                               bool updateCustomControl = false) = 0;

  virtual enum PlugMode GetMode() = 0;

  virtual void GetGraphSize(int *width, int *height) = 0;

  virtual bool IsBarActive() = 0;
  virtual void SetBarActive(bool flag) = 0;
  virtual void SetBarPos(BL_FLOAT x) = 0;
  virtual void ResetPlayBar() = 0;

  virtual void StartPlay() = 0;
  virtual void StopPlay() = 0;
  virtual bool PlayStarted() = 0;
  virtual void ClearBar() = 0;
    
  virtual bool IsSelectionActive() = 0;
  virtual void UpdateZoomSelection(BL_FLOAT selection[4],
                                   BL_FLOAT zoomChange) = 0;
  virtual void SelectionChanged() = 0;
  virtual void BeforeSelTranslation() = 0;
  virtual void AfterSelTranslation() = 0;
    
  virtual bool PlayBarOutsideSelection() = 0;
    
  virtual void UpdateZoom(BL_FLOAT zoomChange) = 0;
  virtual void SetZoomCenter(int x) = 0;
  virtual void Translate(int dX) = 0;

  virtual void SetNeedRecomputeData(bool flag) = 0;

  virtual void RewindView() = 0;

  virtual void DoCutCommand() = 0;
  virtual void DoGainCommand() = 0;
  virtual void DoReplaceCommand() = 0;
  virtual void DoCopyCommand() = 0;
  virtual void DoPasteCommand() = 0;
  virtual void UndoLastCommand() = 0;

  virtual void CheckRecomputeData() = 0;

  virtual void OpenFile(const char *fileName) = 0;
    
  virtual void SetPlayStopParameter(int value) = 0;
  
  // For Protools
  bool PlaybackWasRestarted(unsigned long long delay);
    
protected:
  unsigned long long mPrevUpTime;
};

#endif
