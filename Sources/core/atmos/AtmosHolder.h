#pragma once

#include <array>
#include <sstream>

#include "AtmosConstants.h"
#include "AtmosGrid.h"

class AtmosHolder
{
    friend std::istream& operator>>(std::stringstream& file, AtmosHolder& atmos_holder);
    friend std::ostream& operator<<(std::stringstream& file, const AtmosHolder& atmos_holder);
    friend unsigned int hash(const AtmosHolder& atmos_holder);
public:
    AtmosHolder()
    {
        for (size_t i = 0; i < GASES_NUM; ++i)
        {
            data_.gases[i] = 0;
        }
        data_.energy = 0;
        data_.pressure = 0;
        temperature_ = 0;
        volume_ = 1;
        data_ptr_ = &data_;
    }
    void Connect(AtmosHolder* guest, 
                 int level_owner = MAX_GAS_LEVEL, int level_guest = MAX_GAS_LEVEL,
                 int div = MAX_GAS_LEVEL / 2 /*100% - owner take all*/);
    void AddEnergy(int energy);
    void AddGase(unsigned int gase, unsigned int amount);
    void SetVolume(unsigned int volume);
    unsigned int GetEnergy();
    unsigned int GetPressure();
    unsigned int GetTemperature();
    unsigned int GetVolume();
    unsigned int GetGase(unsigned int gase);
    
    unsigned int RemoveGase(unsigned int gase, unsigned int amount);

    void Truncate();

    void UpdateMacroParams();
private:
    AtmosData* data_ptr_;
    AtmosData data_;
    
    unsigned int volume_;
    unsigned int temperature_;
};

std::istream& operator>>(std::stringstream& file, AtmosHolder& atmos_holder);
std::ostream& operator<<(std::stringstream& file, const AtmosHolder& atmos_holder);

inline unsigned int hash(const AtmosHolder& atmos_holder)
{
    unsigned int retval = 0;
    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        retval += atmos_holder.data_ptr_->gases[i];
    }
    retval += atmos_holder.data_ptr_->energy;
    retval += atmos_holder.data_ptr_->pressure;
    retval += atmos_holder.volume_;
    retval += atmos_holder.temperature_;
    return retval;
}
