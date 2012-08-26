#pragma once
#include "DrawInt.h"
class SmallItem;
struct HashAmount;

class IOnMapItem: public IDraw
{
public:
    IOnMapItem();
    DECLARE_GET_TYPE_ITEM(ionmapitem)
    DECLARE_SAVED(IOnMapItem, IDraw);
    void checkSelfLevel();
    static void checkForEachLevel(int posx, int posy);
    static void setEachLevel(int posx, int posy, bool level);
    static bool checkLevel(int posx, int posy);
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
public:
    int KV_SAVEBLE(lastMove);
    int KV_SAVEBLE(tickSpeed);
    int KV_SAVEBLE(pixSpeed);
    Dir KV_SAVEBLE(dMove);
    bool KV_SAVEBLE(passable);
    bool KV_SAVEBLE(transparent);
    int KV_SAVEBLE(posx);//local coord, 1..n, N ý n
    int KV_SAVEBLE(posy);
    bool KV_SAVEBLE(level);//0 - 1
    struct Z_Impl
    {
        static const int counter = __COUNTER__;
    };
    template<>
    __forceinline void KV_LOAD_FUNC<Z_Impl::counter>(std::stringstream& file)
    {
        LoadInMap();
        KV_LOAD_FUNC<Z_Impl::counter + 1>(file);
    }
    template<>
    __forceinline void KV_SAVE_FUNC<Z_Impl::counter>(std::stringstream& file)
    {
        KV_SAVE_FUNC<Z_Impl::counter + 1>(file);
    }
    int KV_SAVEBLE(burn_power);//0 - 1 - MUCH MUCH
    std::string KV_SAVEBLE(name);
private:
    void LoadInMap();
};

ADD_TO_TYPELIST(IOnMapItem);