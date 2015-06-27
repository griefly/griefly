#include "Clothes.h"

SimpleCloth::SimpleCloth(size_t id) : Item(id)
{
    SetSprite("icons/suits.dmi");
    SetState("armor");

    type = "SUIT";

    name = "SimpleCloth";
}
