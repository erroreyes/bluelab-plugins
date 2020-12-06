#include "GhostTriggerControl.h"

void
GhostTriggerControl::OnGUIIdle()
{
    ((Ghost *)mPlug)->CheckRecomputeData();
}
