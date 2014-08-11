#pragma once

#include <list>

#include "OnMapBaseInt.h"
#include "GLSprite.h"
#include "ASpritesClass.h"
#include "HelperStructers.h"
#include "Metadata.h"
#include "View.h"

class IOnMapObject: public IOnMapBase
{
public:
    IOnMapObject();
    DECLARE_GET_TYPE_ITEM(ionmapitem);
    DECLARE_SAVED(IOnMapObject, IOnMapBase);
    void checkSelfLevel();

    virtual bool IsVisibleByPlayer() const override
    {
        return owner.valid() && owner->IsVisibleByPlayer();
    }

    virtual PassableLevel GetPassable(Dir direct) const override
    {
        switch (direct)
        {
        case D_UP:    return passable_up;
        case D_DOWN:  return passable_down;
        case D_LEFT:  return passable_left;
        case D_RIGHT: return passable_right;
        case D_ALL:   return passable_all;
        }
        return false;
    }

    void SetPassable(Dir direct, PassableLevel p)
    {
        switch (direct)
        {
        case D_UP:    passable_up = p;    break;
        case D_DOWN:  passable_down = p;  break;
        case D_LEFT:  passable_left = p;  break;
        case D_RIGHT: passable_right = p; break;
        case D_ALL:   passable_all = p;   break;
        }
    }

    virtual bool IsTransparent() const override
    {
        return transparent;
    }
    virtual int GetDrawX() const override
    {
        int temp = owner->GetDrawX();
        return temp;
    }
    virtual int GetDrawY() const override
    {
        int temp = owner->GetDrawY();
        return temp;
    }
    virtual void process() override
    {
        processPhysics();
    }
    virtual void processPhysics();

    //void mobMove();
    static bool isVisible(int x, int y);
    
    virtual void delThis() override;
    virtual std::list<HashAmount> insertLiquid(std::list<HashAmount>);
    virtual void processImage(DrawType type) override;//draw this item
    void DrawMain(int frame_shift, int x, int y);
    virtual bool IsTransp(int x, int y) override;
    void SetSprite(const std::string& T_SPR);
    const GLSprite* GetSprite();

    void SetState(const std::string& name);
    const ImageMetadata::SpriteMetadata* GetMetadata();
    View* GetView() { return &view_; }
public:
    int KV_SAVEBLE(v_level);
    PassableLevel KV_SAVEBLE(passable_down);
    PassableLevel KV_SAVEBLE(passable_up);
    PassableLevel KV_SAVEBLE(passable_left);
    PassableLevel KV_SAVEBLE(passable_right);
    PassableLevel KV_SAVEBLE(passable_all);

    PassableLevel KV_SAVEBLE(passable_level);

    bool KV_SAVEBLE(transparent);
    int KV_SAVEBLE(burn_power);//0 - 1 - MUCH MUCH
    std::string KV_SAVEBLE(name);
protected:
    View KV_SAVEBLE(view_);
};

ADD_TO_TYPELIST(IOnMapObject);
