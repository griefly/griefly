#include "MainInt.h"
#include "MapClass.h"
#include "ItemFabric.h"

std::hash_map<unsigned int, item_creator>* itemList()
{
    static std::hash_map<unsigned int, item_creator>* result = new std::hash_map<unsigned int, item_creator>;
    return result; 
};

std::hash_map<unsigned int, item_creator>* itemListSaved()
{
    static std::hash_map<unsigned int, item_creator>* result = new std::hash_map<unsigned int, item_creator>;
    return result; 
};

void IMainItem::delThis()
{
    GetItemFabric()->idTable()[id_] = 0;
    if (GetFreq())
        GetItemFabric()->RemoveProcessingItem(GetId());
    delete this;
}

bool IMainItem::saveSelf(std::stringstream& file)
{
    file << " " << T_ITEM() << " ";
    file << " " << id_ << " ";
//    file << " " << master.ret_id() << " ";
    file << " " << how_often_ << " ";
    return true;
};

bool IMainItem::loadSelf(std::stringstream& file)
{
    file >> how_often_;

    SetFreq(GetFreq());

    // It empty becouse all data load main outer function
    return true;
};

void IMainItem::SetFreq(int freq)
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

void IMainItem::SetId(size_t id)
{
    id_ = id;
    SetFreq(GetFreq());
}