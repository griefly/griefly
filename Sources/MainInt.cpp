#include "MainInt.h"
#include "MapClass.h"
#include "ItemFabric.h"

std::unordered_map<unsigned int, item_creator>* itemList()
{
    static std::unordered_map<unsigned int, item_creator>* result = new std::unordered_map<unsigned int, item_creator>;
    return result; 
}

std::unordered_map<unsigned int, item_creator_saved>* itemListSaved()
{
    static std::unordered_map<unsigned int, item_creator_saved>* result = new std::unordered_map<unsigned int, item_creator_saved>;
    return result; 
}

void IMainObject::delThis()
{
    GetItemFabric()->idTable()[id_] = 0;
    if (GetFreq())
        GetItemFabric()->RemoveProcessingItem(GetId());
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
    if (!GetId())
        return;

    if (how_often_ != 0)
    {
        GetItemFabric()->AddProcessingItem(GetId());
    }
    else
    {
        GetItemFabric()->RemoveProcessingItem(GetId());
    }
}

void IMainObject::SetId(size_t id)
{
    id_ = id;
    SetFreq(GetFreq());
}
