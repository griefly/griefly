#pragma once

#include "Human.h"

// TODO: classes (?)

namespace professions
{
    void ToSecurityOfficer(IdPtr<kv::Human> human);
    void ToDoctor(IdPtr<kv::Human> human);
    void ToAssistant(IdPtr<kv::Human> human);
    void ToClown(IdPtr<kv::Human> human);
    void ToBarman(IdPtr<kv::Human> human);
}
