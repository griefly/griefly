#ifndef CALIVEMOB_H
#define CALIVEMOB_H

#include "MobInt.h"

#define __KV_CLASS__ CAliveMob

#include "LiquidHolder.h"

class CAliveMob : public IMob
{
public:
    DECLARE_GET_TYPE_ITEM(alive)
    DECLARE_SAVED(CAliveMob, IMob);
    virtual void process();
    virtual void live();
    virtual void aaMind();

    virtual void attack_by(id_ptr_on<SmallItem> atk, int force = -1) override;

    virtual void DeinitGUI() override;
    virtual void InitGUI() override;
    virtual void processGUI() override;
    
    virtual void ignite(int force) override;
    //virtual bool checkMove(Dir direct);
    int KV_SAVEBLE(dmg);//brute damage
    int KV_SAVEBLE(injuries);//anti-penetrete
    int KV_SAVEBLE(burn);
    int KV_SAVEBLE(interior);//blood&digestive system dmg
    int KV_SAVEBLE(bloodless);
    int KV_SAVEBLE(oxyless);

    int KV_SAVEBLE(max_dmg);
    id_ptr_on<LiquidHolder> KV_SAVEBLE(inside);
    int KV_SAVEBLE(tick_sm);
    CAliveMob();
};
ADD_TO_TYPELIST(CAliveMob);

#endif