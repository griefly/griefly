#include "Sign.h"

using namespace kv;

WallSign::WallSign()
{
    SetTransparency(true);
    SetPassable(Dir::ALL, passable::FULL);
    SetVisibleLevel(6);

    SetSprite("icons/decals.dmi");
    SetState("bluecross2"); 

    SetName("Sign");

    sign_type_ = "bluecross2";
}

void WallSign::AfterWorldCreation()
{
    MaterialObject::AfterWorldCreation();
    SetState(sign_type_);
}

void WallSign::AttackBy(IdPtr<Item> item)
{
}
