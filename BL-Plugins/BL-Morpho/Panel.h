#ifndef PANEL_H
#define PANEL_H

#include <vector>
using namespace std;

#include "IPlug_include_in_plug_hdr.h"

using namespace iplug;
using namespace iplug::igraphics;

class Panel
{
public:
    Panel(IGraphics *graphics) { mGraphics = graphics; }
    virtual ~Panel() { Clear(); }

    void SetGraphics(IGraphics *graphics) { mGraphics = graphics; }
    
    void Add(IControl *control) { mControls.push_back(control); }

    void Add(const vector<IControl *> &controls)
    { mControls.insert(mControls.end(), controls.begin(), controls.end()); }
    
    void Clear()
    {
        if (mGraphics == NULL)
            return;

        for (int i = 0; i < mControls.size(); i++)
        {
            IControl *control = mControls[i];
            mGraphics->RemoveControl(control);
        }

        mControls.clear();
    }

protected:
    IGraphics *mGraphics;
    
    vector<IControl *> mControls;
};

#endif
