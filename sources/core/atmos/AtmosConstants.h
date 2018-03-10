#pragma once

#include <qglobal.h>

namespace atmos
{
    const int OXYGEN = 0;
    const int OXYGEN_FREEDOM = 5;
    const int OXYGEN_WEIGHT = 16 * 2;
    const char* const OXYGEN_NAME = "Oxygen";

    const int CO2 = 1;
    const int CO2_FREEDOM = 6;
    const int CO2_WEIGHT = 44;
    const char* const CO2_NAME = "CO2";

    const int NYTROGEN = 2;
    const int NYTROGEN_FREEDOM = 5;
    const int NYTROGEN_WEIGHT = 14 * 3;
    const char* const NYTROGEN_NAME = "Nytrogen";

    const int PLASMA = 3;
    const int PLASMA_FREEDOM = 8;
    const int PLASMA_WEIGHT = 139;
    const char* const PLASMA_NAME = "Plasma";

    const int GASES_NUM = 4;

    const int GASES_WEIGHT[GASES_NUM] = {OXYGEN_WEIGHT, CO2_WEIGHT, NYTROGEN_WEIGHT, PLASMA_WEIGHT};
    const int GASES_FREEDOM[GASES_NUM] = {OXYGEN_FREEDOM, CO2_FREEDOM, NYTROGEN_FREEDOM, PLASMA_FREEDOM};
    const char* const GASES_NAME[GASES_NUM] = {OXYGEN_NAME, CO2_NAME, NYTROGEN_NAME, PLASMA_NAME};

    const int MAX_GAS_LEVEL = 1000;

    using AtmosState = int;
    const AtmosState NON_SIMULATED = -1;
    const AtmosState SPACE = 0;
    const AtmosState SIMULATED = 1;

    const int ENERGY_CONST = 100;

    const int O2_MULTIPLIER = 2;

    struct AtmosData
    {
        int gases[GASES_NUM];
        int energy;
        int pressure;
        qint16 volume;
        int temperature;
        bool fire;
    };

    inline void ProccesBurning(AtmosData* data)
    {
        if (!data->fire)
        {
            return;
        }
        const int oxygen = data->gases[OXYGEN];
        const int plasma = data->gases[PLASMA];

        const int max_burn = std::min(oxygen / O2_MULTIPLIER, plasma);

        if (max_burn == 0)
        {
            data->fire = false;
            return;
        }
        const int MIN_MAX_BURN = 1;
        const int BURN_SCALE = 32;
        const int burn = std::min(max_burn, std::max(MIN_MAX_BURN, max_burn / BURN_SCALE));

        data->gases[OXYGEN] -= burn * O2_MULTIPLIER;
        data->gases[PLASMA] -= burn;
        data->gases[CO2] += burn;

        const int ENERGY_MULTIPLIER = 4;
        data->energy += burn * ENERGY_MULTIPLIER;
    }

    inline void UpdateMacroParams(AtmosData* data)
    {
        // E = (freedom / 2) * moles * T
        // T = E / (... + ... + ...)

        int sum = 0;
        for (quint32 i = 0; i < GASES_NUM; ++i)
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
        for (quint32 i = 0; i < GASES_NUM; ++i)
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
}
