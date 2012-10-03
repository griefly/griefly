#include "Debug.h"
#include "MainInt.h"

const char* const DEFAULT_FOLDER = "LastSaves/";
const char* const SAVE_WITHOUT_NUMBER = "save";

Debug::Impl::UnsyncDebug& Debug::UnsyncDebug()
{
    static Debug::Impl::UnsyncDebug unsync_;
    return unsync_;
}

Debug::Impl::UnsyncDebug::UnsyncDebug()
{
    counter_ = 1;
    last_name_ = GetNextNameToSave(DEFAULT_FOLDER);
}

std::string Debug::Impl::UnsyncDebug::GetNextNameToSave(const std::string& folder)
{
    std::stringstream converter;
    converter << counter_++;
    return folder + SAVE_WITHOUT_NUMBER + converter.str();
}

bool Debug::Impl::UnsyncDebug::Save()
{
    IMainItem::fabric->saveMap("");
    // TODO: do it
}
