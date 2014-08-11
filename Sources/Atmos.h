#pragma once

#include <vector>

#include "AtmosHolder.h"

class Atmosphere
{
public:
    Atmosphere();
    void Resize(int x, int y, int z);
    void Process();
private:
    std::vector<std::vector<std::vector<AtmosHolder>>> squares_;
};