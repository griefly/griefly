#pragma once

#include "OnMapInt.h"

class CTestmob: public IOnMapItem
{
public:
    DECLARE_GET_TYPE_ITEM(testmob);
    DECLARE_SAVED(CTestmob, IOnMapItem);
    virtual void process();

    CTestmob();
};
ADD_TO_TYPELIST(CTestmob);