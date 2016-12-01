#include "FloorTile.h"

FloorTile::FloorTile(quint32 id) : Item(id)
{
    SetState("tile");
    type_ = "floor";
    name = "Floor tile";
}
