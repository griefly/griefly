#pragma once

#include "representation/ViewInfo.h"

namespace kv
{
class Item;

enum class ItemSlot : quint8
{
    DEFAULT,
    SUIT,
    HEAD,
    ANYTHING,
    FEET,
    UNIFORM
};

inline FastSerializer& operator<<(FastSerializer& serializer, const ItemSlot& item_slot)
{
    serializer << static_cast<quint8>(item_slot);
    return file;
}

inline FastDeserializer& operator>>(FastDeserializer& deserializer, ItemSlot& item_slot)
{
    quint8 size;
    deserializer >> size;
    item_slot = static_cast<ItemSlot>(size);
    return file;
}

inline unsigned int hash(const ItemSlot& item_slot)
{
    return hash(static_cast<quint32>(item_slot));
}

struct Slot
{
    ViewInfo view;
    IdPtr<Item> item;
    int posx;
    int posy;
    ItemSlot type;
    QString name;
};

}
