#pragma once

#include "Item.h"

namespace kv
{

class Weldingtool : public Item
{
public:
    DECLARE_SAVEABLE(Weldingtool, Item);
    REGISTER_CLASS_AS(Weldingtool);
    Weldingtool();
    virtual void Process() override;
    virtual void AttackBy(IdPtr<Item> item) override;
    void TurnOn();
    void TurnOff();
    bool Working() const { return on_; }
    int AddFuel(int amount);
private:
    int KV_SAVEABLE(fuel_);
    bool KV_SAVEABLE(on_);
};
END_DECLARE(Weldingtool);

}
