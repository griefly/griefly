#include "Idptr.h"

#include "MainInt.h"
#include "ItemFabric.h"
#include "OnMapBaseInt.h"

IMainObject* GetFromIdTable(size_t id)
{
    return GetItemFabric()->idTable()[id];
}