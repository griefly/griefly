#pragma once

#include "Item.h"

#include "View.h"

#include "hashes.h"

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
        view_.SetSprite("screen_retro.dmi");
        item_ = 0;
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
    virtual void Draw() override
    {
        view_.Draw(0, 32 * posx_, 32 * posy_);
        if (item_)
            item_->DrawMain(0, 32 * posx_, 32 * posy_);
    }
    View* GetView()
    {
        return &view_;
    }
    bool Click(int x, int y)
    {
        return view_.Click(x, y);
    }
    virtual std::ostream& operator<<(std::stringstream& file) override
    {
        file << view_ << " ";
        file << item_ << " ";
        file << posx_ << " ";
        file << posy_ << " ";
        return file;
    }
    virtual std::istream& operator>>(std::stringstream& file) override
    {
        file >> view_;
        file >> item_;
        file >> posx_;
        file >> posy_;
        return file;
    }
    virtual unsigned int hash_member() const override
    {
        return   hash(view_)
               + hash(item_)
               + hash(posx_)
               + hash(posy_);
    }
private:
    View view_;
    id_ptr_on<T> item_;
    int posx_;
    int posy_;
};

template <class T>
inline unsigned int hash(const Slot<T>& slot)
{
    return slot_.hash();
}

