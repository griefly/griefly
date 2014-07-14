#include "Grille.h"
#include "MobInt.h"

Grille::Grille()
{
    transparent = true;
    passable = false;

    tickSpeed = 5;
    pixSpeed = 1;

    v_level = 10;

    SetSprite("icons/structures.dmi");
    SetState("grille");

    name = "Grille";
};

void Grille::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<IMob> m;
    m = item;
    if (m)
        checkMove(m->dMove);
}