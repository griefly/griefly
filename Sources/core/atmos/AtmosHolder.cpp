#include "AtmosHolder.h"

#include "../Constheader.h"

unsigned int loc_gases[GASES_NUM];
unsigned int loc_energy;

void AtmosHolder::Connect(AtmosHolder* guest, int level_owner, int level_guest, int div)
{
    if (level_owner > MAX_GAS_LEVEL)
    {
        level_owner = MAX_GAS_LEVEL;
    }
    if (level_owner < 0)
    {
        level_owner = 0;
    }
    if (level_guest > MAX_GAS_LEVEL)
    {
        level_guest = MAX_GAS_LEVEL;
    }
    if (level_guest < 0)
    {
        level_guest = 0;
    }
    if (div > MAX_GAS_LEVEL)
    {
        div = MAX_GAS_LEVEL;
    }
    if (div < 0)
    {
        div = 0;
    }

    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        unsigned int gase_owner = (data_ptr_->gases[i] * level_owner) / MAX_GAS_LEVEL;
        data_ptr_->gases[i] -= gase_owner;

        unsigned int gase_guest = (guest->data_ptr_->gases[i] * level_guest) / MAX_GAS_LEVEL;
        guest->data_ptr_->gases[i] -= gase_guest;

        loc_gases[i] = gase_owner + gase_guest;
    }
    
    unsigned int energy_owner = (data_ptr_->energy * level_owner) / MAX_GAS_LEVEL;
    data_ptr_->energy -= energy_owner;

    unsigned int energy_guest = (guest->data_ptr_->energy * level_guest) / MAX_GAS_LEVEL;
    guest->data_ptr_->energy -= energy_guest;

    loc_energy = energy_owner + energy_guest;

    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        unsigned int gase_owner = (loc_gases[i] * div) / MAX_GAS_LEVEL;
        unsigned int gase_guest = loc_gases[i] - gase_owner;

        data_ptr_->gases[i] += gase_owner;
        guest->data_ptr_->gases[i] += gase_guest;
    }

    energy_owner = (loc_energy * div) / MAX_GAS_LEVEL;
    energy_guest = loc_energy - energy_owner;

    data_ptr_->energy += energy_owner;
    guest->data_ptr_->energy += energy_guest;

    UpdateMacroParams();
    guest->UpdateMacroParams();
}

void AtmosHolder::UpdateMacroParams()
{
    ::UpdateMacroParams(data_ptr_);
}

void AtmosHolder::SetVolume(unsigned int volume)
{
    data_ptr_->volume = volume;
    UpdateMacroParams();
}

void AtmosHolder::AddGase(unsigned int gase, unsigned int amount)
{
    data_ptr_->gases[gase] += amount;
    UpdateMacroParams();
}

void AtmosHolder::AddEnergy(int energy)
{
    data_ptr_->energy += energy;
    UpdateMacroParams();
}

unsigned int AtmosHolder::GetEnergy()
{
    return data_ptr_->energy;
}
unsigned int AtmosHolder::GetPressure()
{
    return data_ptr_->pressure;
}
unsigned int AtmosHolder::GetTemperature()
{
    return data_ptr_->temperature;
}
unsigned int AtmosHolder::GetVolume()
{
    return data_ptr_->volume;
}

unsigned int AtmosHolder::GetGase(unsigned int gase)
{
    return data_ptr_->gases[gase];
}

unsigned int AtmosHolder::RemoveGase(unsigned int gase, unsigned int amount)
{
    unsigned int retval = amount;
    if (amount > data_ptr_->gases[gase])
    {
        retval = data_ptr_->gases[gase];
        data_ptr_->gases[gase] = 0;
        UpdateMacroParams();
        return retval;
    }
    data_ptr_->gases[gase] -= amount;
    UpdateMacroParams();
    return retval;
}

void AtmosHolder::Truncate()
{
    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        data_ptr_->gases[i] = 0;
    }
    data_ptr_->energy = 0;

    UpdateMacroParams();
}

std::ostream& operator<<(std::stringstream& file, const AtmosHolder& atmos_holder)
{
    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        file << atmos_holder.data_ptr_->gases[i] << " ";
    }
    file << atmos_holder.data_ptr_->energy << " ";
    file << atmos_holder.data_ptr_->pressure << " ";
    file << atmos_holder.data_ptr_->volume << " ";
    file << atmos_holder.data_ptr_->temperature << " ";

    return file;
}

std::istream& operator>>(std::stringstream& file, AtmosHolder& atmos_holder)
{
    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        file >> atmos_holder.data_ptr_->gases[i];
    }
    file >> atmos_holder.data_ptr_->energy;
    file >> atmos_holder.data_ptr_->pressure;
    file >> atmos_holder.data_ptr_->volume;
    file >> atmos_holder.data_ptr_->temperature;

    return file;
}
