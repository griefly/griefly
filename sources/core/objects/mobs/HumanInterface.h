#pragma once

#include <QVector>

#include "core/objects/Object.h"
#include "representation/ViewInfo.h"
#include "core/SaveableOperators.h"

#include "Slot.h"

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
private:
    IdPtr<Human> KV_SAVEABLE(owner_);
    QMap<QString, IdPtr<Item>> KV_SAVEABLE(slots_);
};
END_DECLARE(HumanInterface2)

}

