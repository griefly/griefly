#pragma once

#include "../MainObject.h"

#include <functional>

class TestMainObject : public IMainObject
{
public:
    DECLARE_SAVED(TestMainObject, IMainObject);
    DECLARE_GET_TYPE_ITEM(TestMainObject);

    TestMainObject(quint32 id);

    virtual void AfterWorldCreation() override;
    virtual void Process() override;

    int after_world_creation_;
    int process_;

    void SetCreationCallback(std::function<void()> callback);
    void SetProcessCallback(std::function<void()> callback);
private:
    std::function<void()> creation_callback_;
    std::function<void()> process_callback_;
};
ADD_TO_TYPELIST(TestMainObject);

