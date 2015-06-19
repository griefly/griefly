#pragma once

#include "MobInt.h"

class CAliveMob : public IMob
{
public:
    DECLARE_GET_TYPE_ITEM(alive)
    DECLARE_SAVED(CAliveMob, IMob);
    virtual void process();
    virtual void live();
    virtual void aaMind();

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
    CAliveMob(size_t id);
};
ADD_TO_TYPELIST(CAliveMob);
