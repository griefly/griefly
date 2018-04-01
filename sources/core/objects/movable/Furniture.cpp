#include "Furniture.h"

using namespace kv;

Bed::Bed()
{
    anchored_ = true;
    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("bed");

    SetName("Bed");
}

Chair::Chair()
{
    anchored_ = true;

    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("chair");

    SetName("Chair");
}

void Chair::AfterWorldCreation()
{
    Movable::AfterWorldCreation();

    if (GetDir() == Dir::NORTH)
    {
        v_level = 10;
    }
}

Stool::Stool()
{
    anchored_ = true;

    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("stool");

    SetName("Stool");
}
