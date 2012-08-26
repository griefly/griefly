#include "MainInt.h"
#include "MapClass.h"

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


MapMaster* IMainItem::map = nullptr;

//std::stringstream IMainItem::rawData;

IMob* IMainItem::mob = nullptr;

Manager* IMainItem::mobMaster = nullptr;

ItemFabric* IMainItem::fabric = nullptr;

void IMainItem::delThis()
{
    fabric->idTable()[id] = 0;
    delete this;
}

bool IMainItem::saveSelf(std::stringstream& file)
{
    file << " " << T_ITEM() << " ";
    file << " " << id << " ";
    file << " " << master.ret_id() << " ";
    return true;
};

bool IMainItem::loadSelf(std::stringstream& file)
{
    //It empty.
    return true;
};