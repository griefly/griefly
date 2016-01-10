#include "Utils.h"

#include <fstream>

bool utils::IsFileExist(const std::string& name)
{
    std::ifstream f(name.c_str());
    if (f.good())
    {
        f.close();
        return true;
    }
    f.close();
    return false;
}
