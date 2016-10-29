#pragma once

#include "Structure.h"

class FlatGlass: public Structure
{
public:
    DECLARE_SAVED(FlatGlass, Structure);
    DECLARE_GET_TYPE_ITEM(FlatGlass);
    FlatGlass(quint32 id);

    virtual void AfterWorldCreation() override;

    virtual void Bump(IdPtr<IMovable> item) override;
    virtual bool Rotate(Dir dir) override;
};
ADD_TO_TYPELIST(FlatGlass);

class ReinforcedFlatGlass: public FlatGlass
{
public:
    DECLARE_SAVED(ReinforcedFlatGlass, FlatGlass);
    DECLARE_GET_TYPE_ITEM(ReinforcedFlatGlass);
    ReinforcedFlatGlass(quint32 id);
};
ADD_TO_TYPELIST(ReinforcedFlatGlass);
