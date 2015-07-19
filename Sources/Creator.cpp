#include "Creator.h"

#include "Params.h"

id_ptr_on<IMob> this_mob = 0;
size_t creator = 0;

void ChangeMob(id_ptr_on<IMob> i)
{ 
    if (!GetParamsHolder().GetParamBool("-editor") && this_mob)
        this_mob->DeinitGUI();
    this_mob = i;

    if (this_mob.valid())
    {
        if (!GetParamsHolder().GetParamBool("-editor"))
            this_mob->InitGUI();
    }

    SYSTEM_STREAM << "\nTHIS MOB CHANGE: " << this_mob.ret_id() << " ";
}

id_ptr_on<IMob> GetMob()
{
    return this_mob;
}

void SetMob(size_t new_mob)
{
    this_mob = new_mob;
}

void SetCreator(size_t new_creator)
{
    creator = new_creator;
}
size_t GetCreator()
{
    return creator;
}
