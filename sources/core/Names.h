#pragma once

#include <string>
#include <vector>

class SyncRandom;
class Names
{
public:
    Names(SyncRandom* random);
    std::string GetMaleName();
private:
    void LoadNames();
    std::vector<std::string> male_names_;
    std::vector<std::string> last_;
    SyncRandom* random_;
};
