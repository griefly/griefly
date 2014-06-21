#include "visible_points.h"

#include "Mob.h"
#include "MapEditor.h"

const std::list<point>* GetVisible()
{
    if (GetManager())
        return GetManager()->visiblePoint;
    return GetMapEditor()->GetVisible();
}