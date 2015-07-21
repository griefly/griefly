#include "Idptr.h"

#include "MainInt.h"
#include "ItemFabric.h"
#include "OnMapBaseInt.h"

std::unordered_map<std::string, item_creator>* itemList()
{
    static std::unordered_map<std::string, item_creator>* result
            = new std::unordered_map<std::string, item_creator>;
    return result;
}

std::unordered_map<std::string, item_creator_saved>* itemListSaved()
{
    static std::unordered_map<std::string, item_creator_saved>* result
            = new std::unordered_map<std::string, item_creator_saved>;
    return result;
}

IMainObject* GetFromIdTable(size_t id)
{
    return GetItemFabric()->idTable()[id];
}
