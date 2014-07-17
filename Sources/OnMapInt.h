#pragma once

#include <list>

#include "OnMapBaseInt.h"
#include "GLSprite.h"
#include "ASpritesClass.h"
#include "HelperStructers.h"
#include "Metadata.h"

class SmallItem;

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

    virtual bool IsPassable(Dir direct) const override
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

    void SetPassable(Dir direct, bool p)
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
        return step_x + temp;
    }
    virtual int GetDrawY() const override
    {
        int temp = owner->GetDrawY();
        return step_y + temp;
    }
    virtual void process() override
    {
        processPhysics();
    }
    virtual void processPhysics();

    //void mobMove();
    static bool isVisible(int x, int y);
    virtual void attack_by(id_ptr_on<SmallItem> item, int force = -1);
    
    virtual void delThis() override;
    virtual std::list<HashAmount> insertLiquid(std::list<HashAmount>);
    virtual void processImage(DrawType type) override;//draw this item
    void DrawMain(int frame_shift, int x, int y);
    virtual bool IsTransp(int x, int y) override;
    void SetSprite(const std::string& T_SPR);
    const GLSprite* GetSprite();

    void SetState(const std::string& name);
    const ImageMetadata::SpriteMetadata* GetMetadata();
    int GetImageState() const { return image_state_; }
public:
    int KV_ON_LOAD(step_x, 0);
    int KV_ON_LOAD(step_y, 0);
    int KV_SAVEBLE(v_level);
    bool KV_SAVEBLE(passable_down);
    bool KV_SAVEBLE(passable_up);
    bool KV_SAVEBLE(passable_left);
    bool KV_SAVEBLE(passable_right);
    bool KV_SAVEBLE(passable_all);
    bool KV_SAVEBLE(transparent);
    int KV_SAVEBLE(burn_power);//0 - 1 - MUCH MUCH
    std::string KV_SAVEBLE(name);
public:
    std::string KV_SAVEBLE(T_SPR);
    const GLSprite* KV_ON_LOAD(sprite_, nullptr);

    std::string KV_SAVEBLE(state_);
    const ImageMetadata::SpriteMetadata* KV_ON_LOAD(metadata_, nullptr);
private:
    int KV_ON_LOAD(image_state_, -1);
    int KV_ON_LOAD(last_frame_tick_, SDL_GetTicks());
};

ADD_TO_TYPELIST(IOnMapObject);
