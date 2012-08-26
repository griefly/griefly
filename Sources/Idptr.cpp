#include "Idptr.h"
#include "MainInt.h"
#include "MapClass.h"

IMainItem* id_ptr::operator*()
{
    return IMainItem::fabric->idTable()[id];
};

IMainItem* id_ptr::operator->()
{
    return IMainItem::fabric->idTable()[id];
};

id_ptr id_ptr::operator=(size_t id_new)
{
    id = id_new;
    return *this;
};
