#include "AtmosHolder.h"

void AtmosHolder::Connect(AtmosHolder* near, bool one_side, int level)
{
    if (level > MAX_GAS_LEVEL)
        level = MAX_GAS_LEVEL;
    if (level < 1)
        level = 1;

    std::array<unsigned int, GASES_NUM> loc_gases_;

}