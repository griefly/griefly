#pragma once

#include "representation/ViewInfo.h"

#include "core/Hashes.h"
#include "core/SaveableOperators.h"

namespace kv
{
class Item;

using SlotTypeBase = quint32;

enum class SlotType : SlotTypeBase
{
    DEFAULT = 0,
    SUIT,
    HEAD,
    ANYTHING,
    FEET,
    UNIFORM
};

inline FastSerializer& operator<<(FastSerializer& serializer, const SlotType& slot_type)
{
    serializer << static_cast<SlotTypeBase>(slot_type);
    return serializer;
}

inline FastDeserializer& operator>>(FastDeserializer& deserializer, SlotType& slot_type)
{
    SlotTypeBase chunk;
    deserializer >> chunk;
    slot_type = static_cast<SlotType>(chunk);
    return deserializer;
}

inline unsigned int Hash(const SlotType& slot_type)
{
    return Hash(static_cast<SlotTypeBase>(slot_type));
}

struct Button
{
    ViewInfo view;
    QPair<int, int> position = {0, 0};
    QString name;
};

inline FastSerializer& operator<<(FastSerializer& serializer, const Button& slot)
{
    serializer << slot.view;
    serializer << slot.position;
    serializer << slot.name;
    return serializer;
}

inline FastDeserializer& operator>>(FastDeserializer& deserializer,Button& slot)
{
    deserializer >> slot.view;
    deserializer >> slot.position;
    deserializer >> slot.name;
    return deserializer;
}

inline unsigned int Hash(const Button& slot)
{
    unsigned int retval = 0;
    retval += Hash(slot.view);
    retval += Hash(slot.position);
    retval += Hash(slot.name);
    return retval;
}

struct Slot : Button
{
    // TODO (?): append ViewInfo to ViewInfo as layer
    QString overlay_sprite;
    QString overlay_state_postfix;
    IdPtr<Item> item;
    SlotType type = SlotType::DEFAULT;
};

inline FastSerializer& operator<<(FastSerializer& serializer, const Slot& slot)
{
    serializer << static_cast<const Button&>(slot);

    serializer << slot.overlay_sprite;
    serializer << slot.overlay_state_postfix;
    serializer << slot.item;
    serializer << slot.type;
    return serializer;
}

inline FastDeserializer& operator>>(FastDeserializer& deserializer, Slot& slot)
{
    deserializer >> static_cast<Button&>(slot);

    deserializer >> slot.overlay_sprite;
    deserializer >> slot.overlay_state_postfix;
    deserializer >> slot.item;
    deserializer >> slot.type;
    return deserializer;
}

inline unsigned int Hash(const Slot& slot)
{
    unsigned int retval = Hash(static_cast<const Button&>(slot));

    retval += Hash(slot.overlay_sprite);
    retval += Hash(slot.overlay_state_postfix);
    retval += Hash(slot.item);
    retval += Hash(slot.type);
    return retval;
}

inline bool IsTypeMatch(const Slot& slot, const SlotType& type)
{
    if (slot.type == SlotType::ANYTHING)
    {
        return true;
    }
    return slot.type == type;
}

}
