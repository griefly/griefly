#pragma once

#include "../MainObject.h"

class TestMainObject : public IMainObject
{
public:
    DECLARE_SAVED(TestMainObject, IMainObject);
    DECLARE_GET_TYPE_ITEM(TestMainObject);

    TestMainObject(quint32 id);

    virtual void AfterWorldCreation() override;
    virtual void Process() override;

    int KV_SAVEBLE(after_world_creation_);
    int KV_SAVEBLE(process_);
};
ADD_TO_TYPELIST(TestMainObject);

