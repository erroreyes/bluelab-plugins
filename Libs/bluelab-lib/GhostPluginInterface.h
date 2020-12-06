#ifndef GHOST_PLUGIN_INTERFACE_H
#define GHOST_PLUGIN_INTERFACE_H

class GhostPluginInterface
{
 public:
  enum GhostPlugMode
  {
    VIEW = 0,
    ACQUIRE,
    EDIT,
    RENDER
  };
  
  virtual void UpdateSelection(double x0, double y0, double x1, double y1,
			       bool updateCenterPos,
			       bool activateDrawSelection = false,
			       bool updateCustomControl = false) = 0;

  virtual enum GhostPlugMode GetMode() = 0;

  virtual void GetGraphSize(int *width, int *height) = 0;

  virtual void SetBarActive(bool flag) = 0;
  virtual void SetBarPos(BL_FLOAT x) = 0;
  virtual void ResetPlayBar() = 0;

  virtual void StartPlay() = 0;
  virtual void StopPlay() = 0;
  virtual bool PlayStarted() = 0;
  
  virtual bool IsSelectionActive() = 0;
  
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
};

#endif
