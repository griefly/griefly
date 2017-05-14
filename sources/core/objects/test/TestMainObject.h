#pragma once

#include "../MainObject.h"

#include <functional>

// LCOV_EXCL_START
namespace kv
{

class TestMainObject : public Object
{
public:
    DECLARE_SAVEABLE(TestMainObject, Object);
    REGISTER_CLASS_AS(TestMainObject);

    TestMainObject();
    ~TestMainObject();

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
END_DECLARE(TestMainObject);
// LCOV_EXCL_STOP

}
