#include <stdio.h>

#include "Debug.h"
#include "MainInt.h"

#include "Mob.h"

const char* const DEFAULT_FOLDER = "Saves/";
const char* const SAVE_WITHOUT_NUMBER = "save";

const int AMOUNT = 15;

std::string unique_name = "Default";

Debug::Impl::UnsyncDebug& Debug::UnsyncDebug()
{
    static Debug::Impl::UnsyncDebug unsync_;
    return unsync_;
}

Debug::Impl::UnsyncDebug::UnsyncDebug()
{
    counter_ = 0;
}

std::string Debug::Impl::UnsyncDebug::GetNextNameToSave(const std::string& folder)
{
    std::stringstream converter;
    converter << MAIN_TICK;
    return folder + unique_name + SAVE_WITHOUT_NUMBER + converter.str();
}

bool Debug::Impl::UnsyncDebug::Save()
{
    if (NetClient::GetNetClient()->Hash() != GetItemFabric()->get_hash_last())
        ++counter_;
    else
        --counter_;

    const unsigned int MAX_FAIL = 6;
    if (counter_ >= MAX_FAIL)
    {
        system("TestKill.bat");
    }

    std::stringstream converter;
    converter << MAIN_TICK - HASH_OFTEN * AMOUNT;
    remove((DEFAULT_FOLDER + unique_name + SAVE_WITHOUT_NUMBER + converter.str()).c_str());
    GetItemFabric()->saveMap(GetNextNameToSave(DEFAULT_FOLDER).c_str());
    return true;
}

void Debug::SetUniqueName(const std::string& name)
{
    unique_name = name;
}

std::string Debug::GetUniqueName()
{
    return unique_name;
}