#ifndef MONSTRCLASS_H
#define MONSTRCLASS_H

#include "CAliveMob.h"

class CMonstr : public CAliveMob
{
public:
    DECLARE_GET_TYPE_ITEM(monstr);
    DECLARE_SAVED(CMonstr, CAliveMob);
    bool KV_SAVEBLE(moveToTarget);
    int KV_SAVEBLE(FindTargetTime);
    virtual void aaMind();
    virtual void live();
    void findTarget();
    CMonstr();
};
ADD_TO_TYPELIST(CMonstr);

#endif