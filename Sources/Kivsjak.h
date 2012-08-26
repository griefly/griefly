#pragma once

#include "CAliveMob.h"

class Kivsjak: public CAliveMob
{
public:
    DECLARE_SAVED(Kivsjak, CAliveMob);
    DECLARE_GET_TYPE_ITEM(kivsjak);
    virtual void aaMind() override;
    virtual bool checkMove(Dir direct) override;
    virtual void live() override;
    int KV_SAVEBLE(food);
    Kivsjak();
};

ADD_TO_TYPELIST(Kivsjak);