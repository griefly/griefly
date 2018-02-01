#pragma once

#include "Object.h"

#include <QVector>

#include "SaveableOperators.h"

namespace kv
{

class ObjectProcessor : public Object
{
public:
    DECLARE_SAVEABLE(ObjectProcessor, Object);
    REGISTER_CLASS_AS(ObjectProcessor);

    ObjectProcessor();

    void Add(IdPtr<Object> object);
    void RunProcess();
private:
    QVector<IdPtr<Object>> KV_SAVEABLE(objects_);
};
END_DECLARE(ObjectProcessor);

}

