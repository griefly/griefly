#include "Grille.h"
#include "MobInt.h"

Grille::Grille()
{
    transparent = true;
    SetPassable(D_ALL, false);

    tickSpeed = 5;
    pixSpeed = 1;

    v_level = 5;

    SetSprite("icons/structures.dmi");
    SetState("grille");

    name = "Grille";
};