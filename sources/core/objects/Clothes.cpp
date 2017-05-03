#include "Clothes.h"

#include "Slot.h"

Cloth::Cloth()
{
    name = "SimpleCloth";
}

Armor::Armor()
{
    SetSprite("icons/suits.dmi");
    SetState("armor");

    type = Slots::SUIT;

    name = "Armor";
}

Helmet::Helmet()
{
    SetSprite("icons/hats.dmi");
    SetState("helmet");

    type = Slots::HEAD;

    name = "Armor";
}

Tophat::Tophat()
{
    SetSprite("icons/hats.dmi");
    SetState("tophat");

    type = Slots::HEAD;

    name = "Tophat";
}

EngineUniform::EngineUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("engine");

    type = Slots::UNIFORM;

    name = "Engine uniform";
}

RedUniform::RedUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("red");

    type = Slots::UNIFORM;

    name = "Red uniform";
}

JanitorUniform::JanitorUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("janitor");

    type = Slots::UNIFORM;

    name = "Janitor uniform";
}

ClownUniform::ClownUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("clown");

    type = Slots::UNIFORM;

    name = "Clown uniform";
}

BarmanUniform::BarmanUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("ba_suit");

    type = Slots::UNIFORM;

    name = "Barman uniform";
}

OrangeBoots::OrangeBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("orange");

    type = Slots::FEET;

    name = "Orange boots";
}

BlackBoots::BlackBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("black");

    type = Slots::FEET;

    name = "Black boots";
}

ClownBoots::ClownBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("clown");

    type = Slots::FEET;

    name = "Clown boots";
}

MedicalUniform::MedicalUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("medical");

    type = Slots::UNIFORM;

    name = "Medical uniform";
}

WhiteShoes::WhiteShoes()
{
    SetSprite("icons/shoes.dmi");
    SetState("white");

    type = Slots::FEET;

    name = "White shoes";
}
