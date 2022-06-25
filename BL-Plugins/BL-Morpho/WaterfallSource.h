#ifndef WATERFALL_SOURCE_H
#define WATERFALL_SOURCE_H

#include "IPlug_include_in_plug_hdr.h"
using namespace iplug;
using namespace igraphics;

#include <BLTypes.h>

#include <Morpho_defs.h>

// Parent class of SoSource and SySource
class GUIHelper12;
class View3DPluginInterface;
class MorphoWaterfallGUI;
class WaterfallSource
{
public:
    WaterfallSource(BL_FLOAT sampleRate, MorphoPlugMode plugMode);
    virtual ~WaterfallSource();

    virtual void Reset(BL_FLOAT sampleRate);

    virtual void OnUIOpen();
    virtual void OnUIClose();

    virtual void SetViewEnabled(bool flag);

    virtual void CreateWaterfallControls(GUIHelper12 *guiHelper,
                                         Plugin *plug,
                                         View3DPluginInterface *view3DListener,
                                         int grapX, int graphY,
                                         const char *graphBitmapFN);

    virtual void SetWaterfallViewMode(WaterfallViewMode mode);
    virtual WaterfallViewMode GetWaterfallViewMode() const;

    virtual void SetWaterfallCameraAngle0(BL_FLOAT angle);
    virtual void SetWaterfallCameraAngle1(BL_FLOAT angle);
    virtual void SetWaterfallCameraFov(BL_FLOAT angle);

protected:
    WaterfallViewMode mWaterfallViewMode;

    BL_FLOAT mWaterfallAngle0;
    BL_FLOAT mWaterfallAngle1;
    BL_FLOAT mWaterfallCameraFov;

    MorphoWaterfallGUI *mWaterfallGUI;
};

#endif
