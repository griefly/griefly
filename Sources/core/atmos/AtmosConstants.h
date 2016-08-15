#pragma once

const unsigned int OXYGEN = 0;
const unsigned int OXYGEN_FREEDOM = 5;
const unsigned int OXYGEN_WEIGHT = 16 * 2;
const char* const OXYGEN_NAME = "Oxygen";

const unsigned int CO2 = 1;
const unsigned int CO2_FREEDOM = 6;
const unsigned int CO2_WEIGHT = 44;
const char* const CO2_NAME = "CO2";

const unsigned int NYTROGEN = 2;
const unsigned int NYTROGEN_FREEDOM = 5;
const unsigned int NYTROGEN_WEIGHT = 14 * 3;
const char* const NYTROGEN_NAME = "Nytrogen";

const unsigned int GASES_NUM = 3;

const unsigned int GASES_WEIGHT[GASES_NUM] = {OXYGEN_WEIGHT, CO2_WEIGHT, NYTROGEN_WEIGHT};
const unsigned int GASES_FREEDOM[GASES_NUM] = {OXYGEN_FREEDOM, CO2_FREEDOM, NYTROGEN_FREEDOM};
const char* const GASES_NAME[GASES_NUM] = {OXYGEN_NAME, CO2_NAME, NYTROGEN_NAME};

const int MAX_GAS_LEVEL = 1000;

typedef int AtmosState;
const AtmosState NON_SIMULATED = -1;
const AtmosState SPACE = 0;
const AtmosState SIMULATED = 1;

const unsigned int ENERGY_CONST = 100;

struct AtmosData
{
    unsigned int gases[GASES_NUM];
    unsigned int energy;
    int pressure;
    unsigned int volume;
    unsigned int temperature;
};

inline void UpdateMacroParams(AtmosData* data)
{
    // E = (freedom / 2) * moles * T
    // T = E / (... + ... + ...)

    unsigned int sum = 0;
    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        sum += data->gases[i] * GASES_FREEDOM[i];
    }
    sum /= 2;

    if (sum == 0)
    {
        data->temperature = 0;
    }
    else
    {
        data->temperature = (data->energy * ENERGY_CONST) / sum;
    }

    // PV = moles * T
    // P = (moles * T) / V

    sum = 0;
    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        sum += data->gases[i];
    }

    if (data->volume == 0)
    {
        data->pressure = 0;
    }
    else
    {
        data->pressure = (sum * data->temperature) / data->volume;
    }
}
