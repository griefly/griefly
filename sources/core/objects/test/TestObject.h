#pragma once

#include "objects/Object.h"
#include "Hearer.h"

#include <functional>

// LCOV_EXCL_START
namespace kv
{

class TestObject : public Object
{
public:
    DECLARE_SAVEABLE(TestObject, Object);
    REGISTER_CLASS_AS(TestObject);

    TestObject();
    ~TestObject();

    virtual void AfterWorldCreation() override;
    virtual void Process() override;

    int KV_SAVEABLE(after_world_creation_);
    int KV_SAVEABLE(process_);

    void SetCreationCallback(std::function<void()> callback);
    void SetProcessCallback(std::function<void()> callback);
    void SetDestructorCallback(std::function<void()> callback);
private:
    std::function<void()> creation_callback_;
    std::function<void()> process_callback_;
    std::function<void()> destructor_callback_;
};
END_DECLARE(TestObject);

class TestHearer : public Object, public Hearer
{
public:
    DECLARE_SAVEABLE(TestHearer, Object);
    REGISTER_CLASS_AS(TestHearer);

    TestHearer();
    ~TestHearer();

    virtual Hearer* ToHearer() override { return this; }

    virtual QVector<Position> GetHeardPoints() const override;
    virtual void Hear(const Phrase& phrase) override;
};
END_DECLARE(TestHearer);

}
// LCOV_EXCL_STOP
