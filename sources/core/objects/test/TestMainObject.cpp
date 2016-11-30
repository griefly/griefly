#include "TestMainObject.h"

TestMainObject::TestMainObject(quint32 id) : IMainObject(id)
{
    after_world_creation_ = 0;
    process_ = 0;
}

void TestMainObject::AfterWorldCreation()
{
    ++after_world_creation_;
}

void TestMainObject::Process()
{
    ++process_;
}
