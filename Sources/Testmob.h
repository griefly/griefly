#pragma once

#include "OnMapInt.h"

class CTestmob: public IOnMapObject
{
public:
    DECLARE_GET_TYPE_ITEM(testmob);
    DECLARE_SAVED(CTestmob, IOnMapObject);
    virtual void process();

    CTestmob();
};
ADD_TO_TYPELIST(CTestmob);