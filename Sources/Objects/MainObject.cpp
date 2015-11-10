#include "MainObject.h"
#include "Map.h"
#include "ItemFabric.h"

void IMainObject::delThis()
{
    GetFactory().GetIdTable()[id_] = nullptr;
    delete this;
}

bool IMainObject::saveSelf(std::stringstream& file)
{
    file << " " << T_ITEM() << " ";
    file << " " << id_ << " ";
//    file << " " << master.ret_id() << " ";
    file << " " << how_often_ << " ";
    return true;
};

bool IMainObject::loadSelf(std::stringstream& file)
{
    file >> how_often_;

    SetFreq(GetFreq());

    // It empty becouse all data load main outer function
    return true;
};

void IMainObject::SetFreq(int freq)
{
    how_often_ = freq;

    // TODO: Why is it here?
    if (!GetId())
        return;

    if (how_often_ != 0)
    {
        GetFactory().AddProcessingItem(GetId());
    }
}

void IMainObject::SetId(size_t id)
{
    id_ = id;
    SetFreq(GetFreq());
}
