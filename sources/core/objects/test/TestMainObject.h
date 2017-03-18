#pragma once

#include "../MainObject.h"

#include <functional>

// LCOV_EXCL_START
class TestMainObject : public IMainObject
{
public:
    DECLARE_SAVED(TestMainObject, IMainObject);
    DECLARE_GET_TYPE_ITEM(TestMainObject);

    TestMainObject(quint32 id);
    ~TestMainObject();

    virtual void AfterWorldCreation() override;
    virtual void Process() override;

    int KV_SAVEBLE(after_world_creation_);
    int KV_SAVEBLE(process_);

    void SetCreationCallback(std::function<void()> callback);
    void SetProcessCallback(std::function<void()> callback);
    void SetDestructorCallback(std::function<void()> callback);
private:
    std::function<void()> creation_callback_;
    std::function<void()> process_callback_;
    std::function<void()> destructor_callback_;
};
ADD_TO_TYPELIST(TestMainObject);
// LCOV_EXCL_STOP
