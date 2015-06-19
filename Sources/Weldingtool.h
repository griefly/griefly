#pragma once

#include "Item.h"

class Weldingtool: public Item
{
public:
    DECLARE_SAVED(Weldingtool, Item);
    DECLARE_GET_TYPE_ITEM(Weldingtool);
    Weldingtool(size_t id);
    virtual void process() override;
    virtual void AttackBy(id_ptr_on<Item> item) override;
    void TurnOn();
    void TurnOff();
    bool Working() const { return on_; }
private:
    int KV_SAVEBLE(fuel_);
    bool KV_SAVEBLE(on_);
};
ADD_TO_TYPELIST(Weldingtool);
