#pragma once

#include "IMovable.h"

class Grille: public IMovable
{
public:
    DECLARE_SAVED(Grille, IMovable);
    DECLARE_GET_TYPE_ITEM(Grille);
    Grille();
    //virtual void Bump(id_ptr_on<IMovable> item) override;
};
ADD_TO_TYPELIST(Grille);
