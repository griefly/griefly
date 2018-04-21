#include "Furniture.h"

using namespace kv;

Bed::Bed()
{
    SetAnchored(true);
    SetVisibleLevel(visible_level::STRUCTURE);

    SetSprite("icons/objects.dmi");
    SetState("bed");

    SetName("Bed");
}

Chair::Chair()
{
    SetAnchored(true);

    SetVisibleLevel(visible_level::STRUCTURE);

    SetSprite("icons/objects.dmi");
    SetState("chair");

    SetName("Chair");
}

void Chair::AfterWorldCreation()
{
    Movable::AfterWorldCreation();

    if (GetDir() == Dir::NORTH)
    {
        SetVisibleLevel(visible_level::STANDING_MOB + 1);
    }
}

Stool::Stool()
{
    SetAnchored(true);

    SetVisibleLevel(visible_level::STRUCTURE);

    SetSprite("icons/objects.dmi");
    SetState("stool");

    SetName("Stool");
}
