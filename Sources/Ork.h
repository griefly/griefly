#pragma once

#include "calivemob.h"

class SmallItem;

class COrk : public CAliveMob
{
public:
    DECLARE_GET_TYPE_ITEM(ork)
    DECLARE_SAVED(COrk, CAliveMob)
    COrk();
    virtual void aaMind();
    virtual void processGUImsg(std::string& msg) override;
    virtual void attack_by(id_ptr_on<SmallItem> atk, int force = -1) override;
    virtual void live() override;
    id_ptr_on<SmallItem> KV_SAVEBLE(in_hand);
};

ADD_TO_TYPELIST(COrk);