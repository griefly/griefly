#pragma once

#include "Item.h"

#include "View.h"

class BaseSlot
{
public:
    virtual bool Set(id_ptr_on<Item> ptr) = 0;
    virtual id_ptr_on<Item>  Get() = 0;
    virtual void Remove() = 0;
    virtual void Draw() = 0;
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
private:
    View view_;
    id_ptr_on<T> item_;
    int posx_;
    int posy_;
};

