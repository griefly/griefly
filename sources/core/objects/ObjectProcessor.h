#pragma once

#include "Object.h"

#include <unordered_set>

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

    std::unordered_set<quint32> objects_lookup_;
    void LoadObjectsPlaces();
    KV_ON_LOAD_CALL(LoadObjectsPlaces);
};
END_DECLARE(ObjectProcessor);

}

