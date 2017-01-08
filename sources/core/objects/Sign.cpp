#include "Sign.h"

Sign::Sign(quint32 id) : ITurf(id)
{
    transparent = true;
    SetPassable(D_ALL, Passable::FULL);
    v_level = 6;

    SetSprite("icons/decals.dmi");
    SetState("bluecross2"); 

    name = "Sign";

    sign_type_ = "bluecross2";
}

void Sign::AfterWorldCreation()
{
    ITurf::AfterWorldCreation();
    SetState(sign_type_);
}

void Sign::AttackBy(IdPtr<Item> item)
{
}
