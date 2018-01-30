#pragma once

#include "Object.h"

namespace kv
{

class ObjectProcessor : public Object
{
public:
    DECLARE_SAVEABLE(ObjectProcessor, Object);
    REGISTER_CLASS_AS(ObjectProcessor);

    // TODO:
    ObjectProcessor() { }
    void AfterWorldCreation() override { }
private:
};
END_DECLARE(ObjectProcessor);

}

