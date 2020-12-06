#ifndef GHOST_CUSTOM_CONTROL_H
#define GHOST_CUSTOM_CONTROL_H

#include <GhostPluginInterface.h>

class GhostCustomControl : public GraphCustomControl
{
public:
    GhostCustomControl(GhostPluginInterface *plug);
    
    virtual ~GhostCustomControl() {}
    
    void Resize(int prevWidth, int prevHeight,
                int newWidth, int newHeight);
    
    void SetSpectrogramDisplay(SpectrogramDisplay *spectroDisplay);
    
    void SetSelectionType(Ghost::SelectionType selectionType);
    
    virtual void OnMouseDown(int x, int y, IMouseMod* pMod);
    virtual void OnMouseUp(int x, int y, IMouseMod* pMod);
    virtual void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod);
    virtual bool OnMouseDblClick(int x, int y, IMouseMod* pMod);
    virtual void OnMouseWheel(int x, int y, IMouseMod* pMod, float d);
    virtual bool OnKeyDown(int x, int y, int key, IMouseMod* pMod) { return false; }
    
    virtual void OnMouseOver(int x, int y, IMouseMod* pMod) {}
    virtual void OnMouseOut() {}
    
    void UpdateZoomSelection(BL_FLOAT zoomChange);
    
    void GetSelection(BL_FLOAT selection[4]);
    void SetSelection(BL_FLOAT x0, BL_FLOAT y0,
                      BL_FLOAT x1, BL_FLOAT y1);
    
    void SetSelectionActive(bool flag);
    
    // Refresh the selection (for example after zoom)
    void UpdateSelection(bool updateCenterPos);
    
protected:
    bool InsideSelection(int x, int y);
    
    void SelectBorders(int x, int y);
    bool BorderSelected();
    
    // Update the selection depending on the selection type
    void UpdateSelectionType();
    
    GhostPluginInterface *mPlug;
    
    // Used to detect pure mouse up, without drag
    bool mPrevMouseDrag;
    
    int mStartDrag[2];
  
    BL_FLOAT mPrevMouseY;
    
    bool mSelectionActive;
    BL_FLOAT mSelection[4];
    
    bool mPrevMouseInsideSelect;
    
    bool mBorderSelected[4];
    
    // Inside or outside spectro ?
    bool mPrevMouseDownInsideSpectro;
    bool mPrevMouseDownInsideMiniView;
    
    SpectrogramDisplay *mSpectroDisplay;
    
    Ghost::SelectionType mSelectionType;
    
    // Detect if we actually made the mouse down insde the spectrogram
    // (FIXES: mouse up on resize button to a bigger size, and then the mouse up
    // is inside the graph at the end (without previous mouse down inside)
    bool mPrevMouseDown;
};


#endif
