#pragma once

#include <QVector>

#include "core/objects/Object.h"
#include "representation/ViewInfo.h"

#include "Slot.h"
#include "core/SaveableOperators.h"

namespace kv
{
class Human;
class Item;

class HumanInterface2 : public Object
{
public:
    DECLARE_SAVEABLE(HumanInterface2, Object);
    REGISTER_CLASS_AS(HumanInterface2);
    HumanInterface2();

    void SetOwner(IdPtr<Human> human);

    void HandleClick(const QString& name);

    IdPtr<Item> GetItem(const QString& slot_name);
    void RemoveItem(const QString& slot_name);
    bool InsertItem(const QString& slot_name, IdPtr<Item> item);

    void Represent();

    // TODO: RemoveItem with IdPtr
    // TODO: ForEach

private:
    void ApplyActiveHandOnSlot(Slot* slot);

    IdPtr<Human> KV_SAVEABLE(owner_);
    QVector<Slot> KV_SAVEABLE(slots_);
};
END_DECLARE(HumanInterface2)

}

