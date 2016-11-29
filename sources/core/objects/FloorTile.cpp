#include "FloorTile.h"

FloorTile::FloorTile(quint32 id) : Item(id)
{
    SetState("tile");
    type = "floor";
    name = "Floor tile";
}

WhiteFloorTile::WhiteFloorTile(quint32 id) : FloorTile(id)
{
    SetState("tile");
    type = "whitebot";
    name = "White Floor tile";
}
