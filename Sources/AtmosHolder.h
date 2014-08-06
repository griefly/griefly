#pragma once

#include <array>

const unsigned int OXYGEN = 0;
const unsigned int OXYGEN_WEIGHT = 16 * 2;

const unsigned int CO2 = 1;
const unsigned int CO2_WEIGHT = 44;

const unsigned int NYTROGEN = 2;
const unsigned int NYTROGEN_WEIGHT = 14 * 3;

const unsigned int GASES_NUM = 3;

const unsigned int GASES_WEIGHT[GASES_NUM] = {OXYGEN_WEIGHT, CO2_WEIGHT, NYTROGEN_WEIGHT};

class AtmosHolder
{
public:
    AtmosHolder()
    {
        for (size_t i = 0; i < GASES_NUM; ++i)
            gases_[i] = 0;
        temperature_ = 0;
        volume_ = 0;
    }
    void Connect(AtmosHolder* near);
private:
    std::array<unsigned int, GASES_NUM> gases_;
    unsigned int temperature_;
    unsigned int volume_;
};