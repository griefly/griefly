#include "Sign.h"

using namespace kv;

WallSign::WallSign()
{
    SetTransparency(true);
    SetPassable(Dir::ALL, passable::FULL);
    SetVisibleLevel(visible_level::ITEM + 1);

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
