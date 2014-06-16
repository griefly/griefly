#pragma once

#include <string>

namespace Debug
{
    namespace Impl
    {
        class UnsyncDebug 
        {
        public:
            bool Save();
            UnsyncDebug();
        private:
            std::string GetNextNameToSave(const std::string& folder);
            unsigned int counter_;
        };
    }

    void SetUniqueName(const std::string& name);

    std::string GetUniqueName();

    Impl::UnsyncDebug& UnsyncDebug();
}