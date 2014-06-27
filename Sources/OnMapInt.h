#pragma once

#include <list>

#include "OnMapBaseInt.h"
#include "GLSprite.h"
#include "ASpritesClass.h"
#include "HelperStructers.h"

class SmallItem;

// TODO: make IMovable
class IOnMapItem: public IOnMapBase
{
public:
    IOnMapItem();
    DECLARE_GET_TYPE_ITEM(ionmapitem)
    DECLARE_SAVED(IOnMapItem, IOnMapBase);
    void checkSelfLevel();
    static void checkForEachLevel(int posx, int posy);
    static void setEachLevel(int posx, int posy, bool level);
    static bool checkLevel(int posx, int posy);

    virtual bool IsVisibleByPlayer() const override
    {
        return owner.valid() && owner->IsVisibleByPlayer();
    }

    virtual bool IsPassable() const override
    {
        return passable;
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
    void processMove();//for move
    void move(Dir direct);//pix move
    virtual bool checkMove(Dir direct);
    //void mobMove();
    static bool isVisible(int x, int y);
    virtual void attack_by(id_ptr_on<SmallItem> item, int force = -1);
    
    virtual void ignite(int force){};
    void nowIsntVisible();
    bool checkMoveTime();
    bool checkPassable();
    bool mainMove();
    virtual void delThis();
    virtual std::list<HashAmount> insertLiquid(std::list<HashAmount>);
    virtual void processImage(DrawType type) override;//draw this item
    virtual bool IsTransp(int x, int y) override;
    void SetSprite(const std::string& T_SPR);
    const GLSprite* GetSprite();

    void SetSpriteTop(const std::string& T_SPR);
    const GLSprite* GetSpriteTop();
public:
    int KV_ON_LOAD(step_x, 0);
    int KV_ON_LOAD(step_y, 0);
    int KV_SAVEBLE(v_level);
    int KV_SAVEBLE(imageStateH);
    int KV_SAVEBLE(imageStateW);
    int KV_SAVEBLE(lastMove);
    int KV_SAVEBLE(tickSpeed);
    int KV_SAVEBLE(pixSpeed);
    Dir KV_SAVEBLE(dMove);
    bool KV_SAVEBLE(passable);
    bool KV_SAVEBLE(transparent);
    int KV_SAVEBLE(burn_power);//0 - 1 - MUCH MUCH
    std::string KV_SAVEBLE(name);
public:
    std::string KV_SAVEBLE(T_SPR);
    GLSprite* KV_ON_LOAD(sprite_, nullptr);

    std::string KV_SAVEBLE(T_SPR_TOP);
    GLSprite* KV_ON_LOAD(sprite_top_, nullptr);
};

ADD_TO_TYPELIST(IOnMapItem);
