#include "FloorTile.h"

FloorTile::FloorTile(size_t id) : Item(id)
{
    SetState("tile");

    name = "Floor tile";
}
