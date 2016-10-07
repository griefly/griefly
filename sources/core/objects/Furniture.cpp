#include "Furniture.h"

Bed::Bed(size_t id) : IMovable(id)
{
    anchored = true;
    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("bed");

    name = "Bed";
}

Chair::Chair(size_t id) : IMovable(id)
{
    anchored = true;

    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("chair");

    name = "Chair";
}


Stool::Stool(size_t id) : IMovable(id)
{
    anchored = true;

    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("stool");

    name = "Stool";
}
