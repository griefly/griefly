#pragma once

#include <array>

const unsigned int OXYGEN = 0;
const unsigned int OXYGEN_FREEDOM = 5;
const unsigned int OXYGEN_WEIGHT = 16 * 2;

const unsigned int CO2 = 1;
const unsigned int CO2_FREEDOM = 6;
const unsigned int CO2_WEIGHT = 44;

const unsigned int NYTROGEN = 2;
const unsigned int NYTROGEN_FREEDOM = 5;
const unsigned int NYTROGEN_WEIGHT = 14 * 3;

const unsigned int GASES_NUM = 3;

const unsigned int GASES_WEIGHT[GASES_NUM] = {OXYGEN_WEIGHT, CO2_WEIGHT, NYTROGEN_WEIGHT};
const unsigned int GASES_FREEDOM[GASES_NUM] = {OXYGEN_FREEDOM, CO2_FREEDOM, NYTROGEN_FREEDOM};

const int MAX_GAS_LEVEL = 1000;

class AtmosHolder
{
public:
    AtmosHolder()
    {
        for (size_t i = 0; i < GASES_NUM; ++i)
            gases_[i] = 0;
        temperature_ = 0;
        volume_ = 0;
        energy_ = 0;
        pressure_ = 0;
    }
    void Connect(AtmosHolder* near, bool one_side = false, int level = MAX_GAS_LEVEL);
    void AddEnergy(unsigned int energy);
    void AddGase(unsigned int gase, unsigned int amount);
    void SetVolume(unsigned int volume);
    unsigned int GetEnergy();
    unsigned int GetPressure();
    unsigned int GetTemperature();
    unsigned int GetVolume();
private:
    std::array<unsigned int, GASES_NUM> gases_;
    unsigned int pressure_;
    unsigned int energy_;
    unsigned int temperature_;
    unsigned int volume_;
};