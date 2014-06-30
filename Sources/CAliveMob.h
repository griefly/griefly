#pragma once

#include "MobInt.h"
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
    
    //virtual bool checkMove(Dir direct);
    int KV_SAVEBLE(dmg);//brute damage
    int KV_SAVEBLE(injuries);//anti-penetrete
    int KV_SAVEBLE(burn);
    int KV_SAVEBLE(interior);//blood&digestive system dmg
    int KV_SAVEBLE(bloodless);
    int KV_SAVEBLE(oxyless);

    int KV_SAVEBLE(max_dmg);
    int KV_SAVEBLE(tick_sm);
    CAliveMob();
};
ADD_TO_TYPELIST(CAliveMob);