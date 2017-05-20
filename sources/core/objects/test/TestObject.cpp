#include "TestObject.h"

// LCOV_EXCL_START
using namespace kv;

TestObject::TestObject()
    : after_world_creation_(0),
      process_(0),
      creation_callback_([](){}),
      process_callback_([](){}),
      destructor_callback_([](){})
{
    // Nothing
}

TestObject::~TestObject()
{
    destructor_callback_();
}

void TestObject::AfterWorldCreation()
{
    ++after_world_creation_;
    creation_callback_();
}

void TestObject::Process()
{
    ++process_;
    process_callback_();
}

void TestObject::SetCreationCallback(std::function<void ()> callback)
{
    creation_callback_ = callback;
}

void TestObject::SetProcessCallback(std::function<void ()> callback)
{
    process_callback_ = callback;
}

void TestObject::SetDestructorCallback(std::function<void ()> callback)
{
    destructor_callback_ = callback;
}
// LCOV_EXCL_STOP
