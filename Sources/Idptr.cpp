#include "Idptr.h"

#include "MainObject.h"
#include "ItemFabric.h"
#include "OnMapBase.h"
#include "AutogenMetadata.h"

bool is_creators_inited()
{
    static bool is = false;
    if (!is)
    {
        is = true;
        return false;
    }
    return true;
}

std::unordered_map<std::string, item_creator>* itemList()
{
    static std::unordered_map<std::string, item_creator>* result
            = new std::unordered_map<std::string, item_creator>;
    if (!is_creators_inited())
    {
        InitCreators();
        InitRealTypes();
    }
    return result;
}

std::unordered_map<std::string, item_creator_saved>* itemListSaved()
{
    static std::unordered_map<std::string, item_creator_saved>* result
            = new std::unordered_map<std::string, item_creator_saved>;
    if (!is_creators_inited())
    {
        InitCreators();
        InitRealTypes();
    }
    return result;
}

IMainObject* GetFromIdTable(size_t id)
{
    return GetItemFabric().idTable()[id];
}
