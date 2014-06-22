#pragma once

#include "CAliveMob.h"

#include "sound.h"

class SmallItem;

class COrk : public CAliveMob
{
public:
    DECLARE_GET_TYPE_ITEM(ork)
    DECLARE_SAVED(COrk, CAliveMob)
    COrk();
    virtual void aaMind();
    virtual void processGUImsg(const Message& msg) override;
    virtual void attack_by(id_ptr_on<SmallItem> atk, int force = -1) override;
    virtual void live() override;
    virtual bool checkMove(Dir direct) override;
    id_ptr_on<SmallItem> KV_SAVEBLE(in_hand);
    int KV_SAVEBLE(jump_time);
    sf::Sound jump_sound;
    sf::Sound step_sound;
};

ADD_TO_TYPELIST(COrk);
