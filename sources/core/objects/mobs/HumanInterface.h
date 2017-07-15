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

    IdPtr<Item> GetItem(const QString& slot);
    bool RemoveItem(const QString& slot);
    bool InsertItem(const QString& slot, IdPtr<Item> item);

    // TODO: RemoveItem with IdPtr
    // TODO: ForEach

private:
    IdPtr<Human> KV_SAVEABLE(owner_);
    // TODO: uncomment when #434 will be ready
    // QVector<Slot> KV_S/AVEABLE(slots_);
};
END_DECLARE(HumanInterface2)

}

