#pragma once

#include <vector>

#include "AtmosHolder.h"

class Atmosphere
{
public:
    Atmosphere();
    void Resize(int x, int y, int z);
    void Process();
    AtmosHolder* GetHolder(int x, int y, int z)
    {
        return &squares_[x][y][z];
    }
private:
    std::vector<std::vector<std::vector<AtmosHolder>>> squares_;
};