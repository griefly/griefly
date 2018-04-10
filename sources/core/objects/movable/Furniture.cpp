#include "Furniture.h"

using namespace kv;

Bed::Bed()
{
    SetAnchored(true);
    SetVisibleLevel(3);

    SetSprite("icons/objects.dmi");
    SetState("bed");

    SetName("Bed");
}

Chair::Chair()
{
    SetAnchored(true);

    SetVisibleLevel(3);

    SetSprite("icons/objects.dmi");
    SetState("chair");

    SetName("Chair");
}

void Chair::AfterWorldCreation()
{
    Movable::AfterWorldCreation();

    if (GetDir() == Dir::NORTH)
    {
        SetVisibleLevel(10);
    }
}

Stool::Stool()
{
    SetAnchored(true);

    SetVisibleLevel(3);

    SetSprite("icons/objects.dmi");
    SetState("stool");

    SetName("Stool");
}
