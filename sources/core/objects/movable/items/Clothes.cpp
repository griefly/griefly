#include "Clothes.h"

using namespace kv;

Cloth::Cloth()
{
    SetName("SimpleCloth");
}

Armor::Armor()
{
    SetSprite("icons/suits.dmi");
    SetState("armor");

    type = SlotType::SUIT;

    SetName("Armor");
}

Helmet::Helmet()
{
    SetSprite("icons/hats.dmi");
    SetState("helmet");

    type = SlotType::HEAD;

    SetName("Armor");
}

Tophat::Tophat()
{
    SetSprite("icons/hats.dmi");
    SetState("tophat");

    type = SlotType::HEAD;

    SetName("Tophat");
}

EngineUniform::EngineUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("engine");

    type = SlotType::UNIFORM;

    SetName("Engine uniform");
}

RedUniform::RedUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("red");

    type = SlotType::UNIFORM;

    SetName("Red uniform");
}

JanitorUniform::JanitorUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("janitor");

    type = SlotType::UNIFORM;

    SetName("Janitor uniform");
}

ClownUniform::ClownUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("clown");

    type = SlotType::UNIFORM;

    SetName("Clown uniform");
}

BarmanUniform::BarmanUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("ba_suit");

    type = SlotType::UNIFORM;

    SetName("Barman uniform");
}

OrangeBoots::OrangeBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("orange");

    type = SlotType::FEET;

    SetName("Orange boots");
}

BlackBoots::BlackBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("black");

    type = SlotType::FEET;

    SetName("Black boots");
}

ClownBoots::ClownBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("clown");

    type = SlotType::FEET;

    SetName("Clown boots");
}

MedicalUniform::MedicalUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("medical");

    type = SlotType::UNIFORM;

    SetName("Medical uniform");
}

WhiteShoes::WhiteShoes()
{
    SetSprite("icons/shoes.dmi");
    SetState("white");

    type = SlotType::FEET;

    SetName("White shoes");
}
