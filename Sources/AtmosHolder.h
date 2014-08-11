#pragma once

#include <array>

#include <sstream>

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
    friend std::istream& operator>>(std::stringstream& file, AtmosHolder& atmos_holder);
    friend std::ostream& operator<<(std::stringstream& file, const AtmosHolder& atmos_holder);
    friend unsigned int hash(const AtmosHolder& atmos_holder);
public:
    AtmosHolder()
    {
        for (size_t i = 0; i < GASES_NUM; ++i)
            gases_[i] = 0;
        temperature_ = 0;
        volume_ = 0;
        energy_ = 0;
        pressure_ = 0;
        volume_ = 1;
    }
    void Connect(AtmosHolder* guest, 
                 int level_owner = MAX_GAS_LEVEL, int level_guest = MAX_GAS_LEVEL,
                 int div = MAX_GAS_LEVEL / 2 /*100% - owner take all*/);
    void AddEnergy(unsigned int energy);
    void AddGase(unsigned int gase, unsigned int amount);
    void SetVolume(unsigned int volume);
    unsigned int GetEnergy();
    unsigned int GetPressure();
    unsigned int GetTemperature();
    unsigned int GetVolume();
    unsigned int GetGase(unsigned int gase);
    
    unsigned int RemoveGase(unsigned int gase, unsigned int amount);

    void UpdateMacroParams();
private:
    std::array<unsigned int, GASES_NUM> gases_;
    unsigned int energy_;
    
    unsigned int volume_;

    unsigned int pressure_;
    unsigned int temperature_;
};

std::istream& operator>>(std::stringstream& file, AtmosHolder& atmos_holder);
std::ostream& operator<<(std::stringstream& file, const AtmosHolder& atmos_holder);

inline unsigned int hash(const AtmosHolder& atmos_holder)
{
    unsigned int retval = 0;
    for (size_t i = 0; i < GASES_NUM; ++i)
        retval += atmos_holder.gases_[i];
    retval += atmos_holder.energy_;
    retval += atmos_holder.volume_;
    retval += atmos_holder.pressure_;
    retval += atmos_holder.temperature_;
    return retval;
}
