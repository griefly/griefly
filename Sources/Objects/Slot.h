#pragma once

#include "Item.h"

#include "ViewInfo.h"

#include "hashes.h"
#include "Representation.h"

namespace Slots
{
    const std::string SUIT = "SUIT";
    const std::string HEAD = "HEAD";
    const std::string ANYTHING = "ANYTHING";
    const std::string FEET = "FEET";
    const std::string UNIFORM = "UNIFORM";
    const std::string DEFAULT = "DEFAULT";
}

class BaseSlot
{
public:
    virtual bool Set(id_ptr_on<Item> ptr) = 0;
    virtual id_ptr_on<Item>  Get() = 0;
    virtual void Remove() = 0;
    virtual void Draw() = 0;
    virtual std::ostream& operator<<(std::stringstream& file) = 0;
    virtual std::istream& operator>>(std::stringstream& file) = 0;
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
    }
    virtual bool Set(id_ptr_on<Item> ptr) override
    {
        if (!(item_ = ptr))
        {
            item_ = 0;
            return false;
        }
        return true;
    }
    virtual id_ptr_on<Item> Get() override
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
    void SetName(const std::string& name)
    {
        name_ = name;
    }

    void SetType(const std::string& type)
    {
        type_ = type;
    }
    bool MatchType(const std::string& type)
    {
        if (type_ == Slots::ANYTHING)
        {
            return true;
        }
        if (type_.find(type) == std::string::npos)
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
    virtual std::ostream& operator<<(std::stringstream& file) override
    {
        file << view_ << " ";
        file << item_ << " ";
        file << posx_ << " ";
        file << posy_ << " ";
        file << type_ << " ";
        return file;
    }
    virtual std::istream& operator>>(std::stringstream& file) override
    {
        file >> view_;
        file >> item_;
        file >> posx_;
        file >> posy_;
        file >> type_;
        return file;
    }
    virtual unsigned int hash_member() const override
    {
        return   hash(view_)
               + hash(item_)
               + hash(posx_)
               + hash(posy_)
               + hash(type_);
    }
private:
    ViewInfo view_;
    id_ptr_on<T> item_;
    int posx_;
    int posy_;
    std::string type_;
    std::string name_;
};

template <class T>
inline unsigned int hash(const Slot<T>& slot)
{
    return slot.hash_member();
}

