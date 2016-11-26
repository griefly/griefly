#include "AutogenMetadata.h"

bool AreCreatorsInited()
{
    static bool is = false;
    if (!is)
    {
        is = true;
        return false;
    }
    return true;
}

std::unordered_map<QString, ItemCreator>* GetItemsCreators()
{
    static std::unordered_map<QString, ItemCreator>* result
        = new std::unordered_map<QString, ItemCreator>;
    if (!AreCreatorsInited())
    {
        InitCreators();
        InitRealTypes();
    }
    return result;
}

std::unordered_map<QString, VoidItemCreator>* GetVoidItemsCreators()
{
    static std::unordered_map<QString, VoidItemCreator>* result
        = new std::unordered_map<QString, VoidItemCreator>;
    if (!AreCreatorsInited())
    {
        InitCreators();
        InitRealTypes();
    }
    return result;
}
