#include "Idptr.h"

#include "MainInt.h"
#include "ItemFabric.h"

IMainItem* GetFromIdTable(size_t id)
{
    return GetItemFabric()->idTable()[id];
}