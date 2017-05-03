#include "TestMainObject.h"

// LCOV_EXCL_START
TestMainObject::TestMainObject()
    : after_world_creation_(0),
      process_(0),
      creation_callback_([](){}),
      process_callback_([](){}),
      destructor_callback_([](){})
{
    // Nothing
}

TestMainObject::~TestMainObject()
{
    destructor_callback_();
}

void TestMainObject::AfterWorldCreation()
{
    ++after_world_creation_;
    creation_callback_();
}

void TestMainObject::Process()
{
    ++process_;
    process_callback_();
}

void TestMainObject::SetCreationCallback(std::function<void ()> callback)
{
    creation_callback_ = callback;
}

void TestMainObject::SetProcessCallback(std::function<void ()> callback)
{
    process_callback_ = callback;
}

void TestMainObject::SetDestructorCallback(std::function<void ()> callback)
{
    destructor_callback_ = callback;
}
// LCOV_EXCL_STOP
