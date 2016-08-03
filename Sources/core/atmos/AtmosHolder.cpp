#include "AtmosHolder.h"

#include "../Constheader.h"

std::array<unsigned int, GASES_NUM> loc_gases;
unsigned int loc_energy;

void AtmosHolder::Connect(AtmosHolder* guest, int level_owner, int level_guest, int div)
{
    if (level_owner > MAX_GAS_LEVEL)
        level_owner = MAX_GAS_LEVEL;
    if (level_owner < 0)
        level_owner = 0;
    if (level_guest > MAX_GAS_LEVEL)
        level_guest = MAX_GAS_LEVEL;
    if (level_guest < 0)
        level_guest = 0;
    if (div > MAX_GAS_LEVEL)
        div = MAX_GAS_LEVEL;
    if (div < 0)
        div = 0;

    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        unsigned int gase_owner = (gases_[i] * level_owner) / MAX_GAS_LEVEL;
        gases_[i] -= gase_owner;

        unsigned int gase_guest = (guest->gases_[i] * level_guest) / MAX_GAS_LEVEL;
        guest->gases_[i] -= gase_guest;

        loc_gases[i] = gase_owner + gase_guest;
    }
    
    unsigned int energy_owner = (energy_ * level_owner) / MAX_GAS_LEVEL;
    energy_ -= energy_owner;

    unsigned int energy_guest = (guest->energy_ * level_guest) / MAX_GAS_LEVEL;
    guest->energy_ -= energy_guest;

    loc_energy = energy_owner + energy_guest;

    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        unsigned int gase_owner = (loc_gases[i] * div) / MAX_GAS_LEVEL;
        unsigned int gase_guest = loc_gases[i] - gase_owner;

        gases_[i] += gase_owner;
        guest->gases_[i] += gase_guest;
    }

    energy_owner = (loc_energy * div) / MAX_GAS_LEVEL;
    energy_guest = loc_energy - energy_owner;

    energy_ += energy_owner;
    guest->energy_ += energy_guest;

    UpdateMacroParams();
    guest->UpdateMacroParams();
}

const unsigned int ENERGY_CONST = 100;

void AtmosHolder::UpdateMacroParams()
{
    // E = (freedom / 2) * moles * T
    // T = E / (... + ... + ...)

    unsigned int sum = 0;
    for (size_t i = 0; i < GASES_NUM; ++i)
        sum += gases_[i] * GASES_FREEDOM[i];
    sum /= 2;

    if (sum == 0)
        temperature_ = 0;
    else
    {
        temperature_ = (energy_ * ENERGY_CONST) / sum;
     //   if (temperature_ == 0)
     //       SYSTEM_STREAM << "Energy: " << energy_ << ", sum: " << sum << std::endl;
    }

    // PV = moles * T
    // P = (moles * T) / V

    sum = 0;
    for (size_t i = 0; i < GASES_NUM; ++i)
        sum += gases_[i];
    
    if (volume_ == 0)
        pressure_ = 0;
    else
    {
        pressure_ = (sum * temperature_) / volume_;
    }


 //   if (pressure_ == 0 && temperature_ == 0 && energy_ != 0 && sum != 0)
//        SYSTEM_STREAM << "WUTOFUCK gas holder huge error" << std::endl;
}

void AtmosHolder::SetVolume(unsigned int volume)
{
    volume_ = volume;
    UpdateMacroParams();
}

void AtmosHolder::AddGase(unsigned int gase, unsigned int amount)
{
    gases_[gase] += amount;
    UpdateMacroParams();
}

void AtmosHolder::AddEnergy(int energy)
{
    energy_ += energy;
    UpdateMacroParams();
}

unsigned int AtmosHolder::GetEnergy()
{
    return energy_;
}
unsigned int AtmosHolder::GetPressure()
{
    return pressure_;
}
unsigned int AtmosHolder::GetTemperature()
{
    return temperature_;
}
unsigned int AtmosHolder::GetVolume()
{
    return volume_;
}

unsigned int AtmosHolder::GetGase(unsigned int gase)
{
    return gases_[gase];
}

unsigned int AtmosHolder::RemoveGase(unsigned int gase, unsigned int amount)
{
    unsigned int retval = amount;
    if (amount > gases_[gase])
    {
        retval = gases_[gase];
        gases_[gase] = 0;
        UpdateMacroParams();
        return retval;
    }
    gases_[gase] -= amount;
    UpdateMacroParams();
    return retval;
}

void AtmosHolder::Truncate()
{
    for (size_t i = 0; i < GASES_NUM; ++i)
        gases_[i] = 0;

    energy_ = 0;

    UpdateMacroParams();
}

std::ostream& operator<<(std::stringstream& file, const AtmosHolder& atmos_holder)
{
    for (size_t i = 0; i < GASES_NUM; ++i)
        file << atmos_holder.gases_[i] << " ";
    file << atmos_holder.energy_ << " ";
    file << atmos_holder.volume_ << " ";
    file << atmos_holder.temperature_ << " ";
    file << atmos_holder.pressure_ << " ";
    return file;
}

std::istream& operator>>(std::stringstream& file, AtmosHolder& atmos_holder)
{
    for (size_t i = 0; i < GASES_NUM; ++i)
        file >> atmos_holder.gases_[i];
    file >> atmos_holder.energy_;
    file >> atmos_holder.volume_;
    file >> atmos_holder.temperature_;
    file >> atmos_holder.pressure_;
    return file;
}
