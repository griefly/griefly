#pragma once

#include "representation/ViewInfo.h"

#include "core/Hashes.h"

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

struct Slot
{
    ViewInfo view;
    IdPtr<Item> item;
    int posx;
    int posy;
    SlotType type;
    QString name;
};

inline FastSerializer& operator<<(FastSerializer& serializer, const Slot& slot)
{
    serializer << slot.view;
    serializer << slot.item;
    serializer << slot.posx;
    serializer << slot.posy;
    serializer << slot.type;
    serializer << slot.name;
    return serializer;
}

inline FastDeserializer& operator>>(FastDeserializer& deserializer, Slot& slot)
{
    deserializer >> slot.view;
    deserializer >> slot.item;
    deserializer >> slot.posx;
    deserializer >> slot.posy;
    deserializer >> slot.type;
    deserializer >> slot.name;
    return deserializer;
}

}

inline unsigned int hash(const kv::SlotType& slot_type)
{
    return hash(static_cast<kv::SlotTypeBase>(slot_type));
}

inline unsigned int hash(const kv::Slot& slot)
{
    unsigned int retval = 0;
    retval += hash(slot.view);
    retval += hash(slot.item);
    retval += hash(slot.posx);
    retval += hash(slot.posy);
    retval += hash(slot.type);
    retval += hash(slot.name);
    return retval;
}
