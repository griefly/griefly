#pragma once

#include "Object.h"

#include <QVector>

#include "Interfaces.h"
#include "SaveableOperators.h"

namespace kv
{

class ObjectProcessor : public Object, public ObjectProcessorInterface
{
public:
    DECLARE_SAVEABLE(ObjectProcessor, Object);
    REGISTER_CLASS_AS(ObjectProcessor);

    ObjectProcessor();

    void Add(quint32 object) override;
    void RunProcess() override;
private:
    QVector<IdPtr<Object>> KV_SAVEABLE(objects_);
};
END_DECLARE(ObjectProcessor);

}

