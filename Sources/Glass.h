#pragma once

#include "Structure.h"

class FlatGlass: public Structure
{
public:
    DECLARE_SAVED(FlatGlass, Structure);
    DECLARE_GET_TYPE_ITEM(FlatGlass);
    FlatGlass(size_t id);

    virtual void Bump(id_ptr_on<IMovable> item) override;
};
ADD_TO_TYPELIST(FlatGlass);

class ReinforcedFlatGlass: public FlatGlass
{
public:
    DECLARE_SAVED(ReinforcedFlatGlass, FlatGlass);
    DECLARE_GET_TYPE_ITEM(ReinforcedFlatGlass);
    ReinforcedFlatGlass(size_t id);
};
ADD_TO_TYPELIST(ReinforcedFlatGlass);
