#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct HashTick
{
    unsigned int hash;
    unsigned int tick;
};

struct HashHistory
{
    std::unordered_map
        <unsigned int, std::vector<unsigned int>> net_values;
    std::vector<HashTick> local_values;
};

namespace Debug
{
    namespace Impl
    {
        class UnsyncDebug 
        {
        public:
            void AddNetSyncPair(unsigned int hash_value, unsigned int tick_value);
            void AddLocalSyncPair(unsigned int hash_value, unsigned int tick_value);

            std::string PrintHashInfo();

            void ProcessDebug();

            void GenerateAndSaveReport();

            bool Save();
            UnsyncDebug();
        private:
            bool report_generated_;

            bool IsReportNeeded();

            HashHistory hash_history_;

            std::string GetNextNameToSave(const std::string& folder);
            unsigned int counter_;
        };
    }

    void SetUniqueName(const std::string& name);

    std::string GetUniqueName();

    Impl::UnsyncDebug& UnsyncDebug();
}
