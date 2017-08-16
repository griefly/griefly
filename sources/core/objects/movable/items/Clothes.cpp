#include "Clothes.h"

using namespace kv;

Cloth::Cloth()
{
    name = "SimpleCloth";
}

Armor::Armor()
{
    SetSprite("icons/suits.dmi");
    SetState("armor");

    type = SlotType::SUIT;

    name = "Armor";
}

Helmet::Helmet()
{
    SetSprite("icons/hats.dmi");
    SetState("helmet");

    type = SlotType::HEAD;

    name = "Armor";
}

Tophat::Tophat()
{
    SetSprite("icons/hats.dmi");
    SetState("tophat");

    type = SlotType::HEAD;

    name = "Tophat";
}

EngineUniform::EngineUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("engine");

    type = SlotType::UNIFORM;

    name = "Engine uniform";
}

RedUniform::RedUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("red");

    type = SlotType::UNIFORM;

    name = "Red uniform";
}

JanitorUniform::JanitorUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("janitor");

    type = SlotType::UNIFORM;

    name = "Janitor uniform";
}

ClownUniform::ClownUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("clown");

    type = SlotType::UNIFORM;

    name = "Clown uniform";
}

BarmanUniform::BarmanUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("ba_suit");

    type = SlotType::UNIFORM;

    name = "Barman uniform";
}

OrangeBoots::OrangeBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("orange");

    type = SlotType::FEET;

    name = "Orange boots";
}

BlackBoots::BlackBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("black");

    type = SlotType::FEET;

    name = "Black boots";
}

ClownBoots::ClownBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("clown");

    type = SlotType::FEET;

    name = "Clown boots";
}

MedicalUniform::MedicalUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("medical");

    type = SlotType::UNIFORM;

    name = "Medical uniform";
}

WhiteShoes::WhiteShoes()
{
    SetSprite("icons/shoes.dmi");
    SetState("white");

    type = SlotType::FEET;

    name = "White shoes";
}
