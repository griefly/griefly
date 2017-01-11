#include "Sign.h"

WallSign::WallSign(quint32 id) : IOnMapObject(id)
{
    transparent = true;
    SetPassable(D_ALL, Passable::FULL);
    v_level = 6;

    SetSprite("icons/decals.dmi");
    SetState("bluecross2"); 

    name = "Sign";

    sign_type_ = "bluecross2";
}

void WallSign::AfterWorldCreation()
{
    IOnMapObject::AfterWorldCreation();
    SetState(sign_type_);
}

void WallSign::AttackBy(IdPtr<Item> item)
{
}
