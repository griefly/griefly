#pragma once

#include "../MainObject.h"

namespace kv
{

class UnsyncGenerator : public Object
{
public:
    DECLARE_SAVED(UnsyncGenerator, Object);
    DECLARE_GET_TYPE_ITEM(UnsyncGenerator)
    UnsyncGenerator();
    void PerformUnsync();
private:
    QString KV_SAVEBLE(unsync_string_);

    void SetThisAsUnsyncGenerator();
    KV_ON_LOAD_CALL(SetThisAsUnsyncGenerator);
};
ADD_TO_TYPELIST(UnsyncGenerator);

}
