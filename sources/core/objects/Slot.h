#pragma once

#include "core/objects/movable/items/Item.h"

#include "representation/ViewInfo.h"

#include "core/Hashes.h"
#include "representation/Representation.h"

namespace Slots
{
    const QString SUIT = "SUIT";
    const QString HEAD = "HEAD";
    const QString ANYTHING = "ANYTHING";
    const QString FEET = "FEET";
    const QString UNIFORM = "UNIFORM";
    const QString DEFAULT = "DEFAULT";
}

class BaseSlot
{
public:
    virtual bool Set(IdPtr<kv::Item> ptr) = 0;
    virtual IdPtr<kv::Item>  Get() = 0;
    virtual void Remove() = 0;
    virtual void Draw() = 0;
    virtual kv::FastSerializer& operator<<(kv::FastSerializer& file) = 0;
    virtual kv::FastDeserializer& operator>>(kv::FastDeserializer& file) = 0;
    virtual unsigned int hash_member() const = 0;
};

template<class T>
class Slot: public BaseSlot
{
public:
    Slot()
    {
        view_.SetSprite("icons/screen_retro.dmi");
        item_ = 0;
        type_ = Slots::ANYTHING;
        name_ = "noname";
        SetPos(0, 0);
    }
    virtual bool Set(IdPtr<kv::Item> ptr) override
    {
        if (!(item_ = ptr))
        {
            item_ = 0;
            return false;
        }
        return true;
    }
    virtual IdPtr<kv::Item> Get() override
    {
        return item_;
    }
    void Remove() override
    {
        item_ = 0;
    }
    void SetPos(int x, int y)
    {
        posx_ = x;
        posy_ = y;
    }
    void SetName(const QString& name)
    {
        name_ = name;
    }

    void SetType(const QString& type)
    {
        type_ = type;
    }
    bool MatchType(const QString& type)
    {
        if (type_ == Slots::ANYTHING)
        {
            return true;
        }
        if (type_.indexOf(type) == -1)
        {
            return false;
        }
        return true;
    }
    virtual void Draw() override
    {
        Draw(0);
    }
    virtual void Draw(int shift)
    {
        Representation::InterfaceUnit unit;
        unit.name = name_;
        unit.pixel_x = 32 * posx_;
        unit.pixel_y = 32 * posy_;
        unit.shift = shift;
        unit.view = view_;
        GetRepresentation().AddToNewFrame(unit);
        if (item_)
        {
            Representation::InterfaceUnit unit;
            unit.name = name_;
            unit.pixel_x = 32 * posx_;
            unit.pixel_y = 32 * posy_;
            unit.shift = 0;
            unit.view = *(item_->GetView());
            GetRepresentation().AddToNewFrame(unit);
        }
    }
    ViewInfo* GetView()
    {
        return &view_;
    }
    virtual kv::FastSerializer& operator<<(kv::FastSerializer& file) override
    {
        file << view_;
        file << item_;
        file << posx_;
        file << posy_;
        file << type_;
        file << name_;
        return file;
    }
    virtual kv::FastDeserializer& operator>>(kv::FastDeserializer& file) override
    {
        file >> view_;
        file >> item_;
        file >> posx_;
        file >> posy_;
        file >> type_;
        file >> name_;
        return file;
    }
    virtual unsigned int hash_member() const override
    {
        return   Hash(view_)
               + Hash(item_)
               + kv::Hash(posx_)
               + kv::Hash(posy_)
               + kv::Hash(type_)
               + kv::Hash(name_);
    }
private:
    ViewInfo view_;
    IdPtr<T> item_;
    int posx_;
    int posy_;
    QString type_;
    QString name_;
};

template <class T>
inline unsigned int hash(const Slot<T>& slot)
{
    return slot.hash_member();
}

