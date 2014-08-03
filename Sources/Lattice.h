#pragma once

#include "Structure.h"

class Lattice: public Structure
{
public:
    DECLARE_SAVED(Lattice, Structure);
    DECLARE_GET_TYPE_ITEM(Lattice);
    Lattice();
};
ADD_TO_TYPELIST(Lattice);