#include "Clothes.h"

#include "Slot.h"

Cloth::Cloth(size_t id) : Item(id)
{
    name = "SimpleCloth";
}

Armor::Armor(size_t id) : Cloth(id)
{
    SetSprite("icons/suits.dmi");
    SetState("armor");

    type = Slots::SUIT;

    name = "Armor";
}

Helmet::Helmet(size_t id) : Cloth(id)
{
    SetSprite("icons/hats.dmi");
    SetState("helmet");

    type = Slots::HEAD;

    name = "Armor";
}

EngineUniform::EngineUniform(size_t id) : Cloth(id)
{
    SetSprite("icons/uniforms.dmi");
    SetState("engine");

    type = Slots::UNIFORM;

    name = "Engine uniform";
}

OrangeBoots::OrangeBoots(size_t id) : Cloth(id)
{
    SetSprite("icons/shoes.dmi");
    SetState("orange");

    type = Slots::FEET;

    name = "Orange boots";
}
