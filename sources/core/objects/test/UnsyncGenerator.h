#pragma once

#include "../Object.h"

namespace kv
{

class UnsyncGenerator : public Object
{
public:
    DECLARE_SAVEABLE(UnsyncGenerator, Object);
    REGISTER_CLASS_AS(UnsyncGenerator);
    UnsyncGenerator();
    void PerformUnsync();
private:
    QString KV_SAVEABLE(unsync_string_);
};
END_DECLARE(UnsyncGenerator);

}
