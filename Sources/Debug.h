#pragma once

#include <string>

namespace Debug
{
    namespace Impl
    {
        class UnsyncDebug
        {
        public:
            UnsyncDebug();
            bool Save();
        private:
            bool SaveAnsyncWorlds();
            std::string GetNextNameToSave(const std::string& folder);
            std::string GetNextFolderToSave();
            std::string last_name_;
            unsigned int counter_;
        };
    }
    Impl::UnsyncDebug& UnsyncDebug();

}