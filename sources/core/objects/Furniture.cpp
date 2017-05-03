#include "Furniture.h"

Bed::Bed()
{
    anchored_ = true;
    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("bed");

    name = "Bed";
}

Chair::Chair()
{
    anchored_ = true;

    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("chair");

    name = "Chair";
}

void Chair::AfterWorldCreation()
{
    IMovable::AfterWorldCreation();

    if (GetDir() == Dir::UP)
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

    name = "Stool";
}
