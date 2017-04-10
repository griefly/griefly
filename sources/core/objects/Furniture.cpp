#include "Furniture.h"

Bed::Bed(quint32 id) : IMovable(id)
{
    anchored_ = true;
    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("bed");

    name = "Bed";
}

Chair::Chair(quint32 id) : IMovable(id)
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

Stool::Stool(quint32 id) : IMovable(id)
{
    anchored_ = true;

    v_level = 3;

    SetSprite("icons/objects.dmi");
    SetState("stool");

    name = "Stool";
}
