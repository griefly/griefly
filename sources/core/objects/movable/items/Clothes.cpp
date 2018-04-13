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

    SetItemType(SlotType::SUIT);

    SetName("Armor");
}

Helmet::Helmet()
{
    SetSprite("icons/hats.dmi");
    SetState("helmet");

    SetItemType(SlotType::HEAD);

    SetName("Armor");
}

Tophat::Tophat()
{
    SetSprite("icons/hats.dmi");
    SetState("tophat");

    SetItemType(SlotType::HEAD);

    SetName("Tophat");
}

EngineUniform::EngineUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("engine");

    SetItemType(SlotType::UNIFORM);

    SetName("Engine uniform");
}

RedUniform::RedUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("red");

    SetItemType(SlotType::UNIFORM);

    SetName("Red uniform");
}

JanitorUniform::JanitorUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("janitor");

    SetItemType(SlotType::UNIFORM);

    SetName("Janitor uniform");
}

ClownUniform::ClownUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("clown");

    SetItemType(SlotType::UNIFORM);

    SetName("Clown uniform");
}

BarmanUniform::BarmanUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("ba_suit");

    SetItemType(SlotType::UNIFORM);

    SetName("Barman uniform");
}

OrangeBoots::OrangeBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("orange");

    SetItemType(SlotType::FEET);

    SetName("Orange boots");
}

BlackBoots::BlackBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("black");

    SetItemType(SlotType::FEET);

    SetName("Black boots");
}

ClownBoots::ClownBoots()
{
    SetSprite("icons/shoes.dmi");
    SetState("clown");

    SetItemType(SlotType::FEET);

    SetName("Clown boots");
}

MedicalUniform::MedicalUniform()
{
    SetSprite("icons/uniforms.dmi");
    SetState("medical");

    SetItemType(SlotType::UNIFORM);

    SetName("Medical uniform");
}

WhiteShoes::WhiteShoes()
{
    SetSprite("icons/shoes.dmi");
    SetState("white");

    SetItemType(SlotType::FEET);

    SetName("White shoes");
}
