#pragma once

#include <array>

#include "AtmosConstants.h"
#include "AtmosGrid.h"

namespace atmos
{
    class AtmosHolder;
    unsigned int Hash(const AtmosHolder& atmos_holder);
    kv::FastDeserializer& operator>>(kv::FastDeserializer& file, atmos::AtmosHolder& atmos_holder);
    kv::FastSerializer& operator<<(kv::FastSerializer& file, const atmos::AtmosHolder& atmos_holder);
}

class Atmosphere;

namespace atmos
{
    class AtmosHolder
    {
        friend class ::Atmosphere;
        friend kv::FastDeserializer& operator>>(kv::FastDeserializer& file, atmos::AtmosHolder& atmos_holder);
        friend kv::FastSerializer& operator<<(kv::FastSerializer& file, const atmos::AtmosHolder& atmos_holder);
        friend unsigned int Hash(const atmos::AtmosHolder& atmos_holder);
    public:
        AtmosHolder()
        {
            for (quint32 i = 0; i < GASES_NUM; ++i)
            {
                data_.gases[i] = 0;
            }
            data_.energy = 0;
            data_.pressure = 0;
            data_.temperature = 0;
            data_.volume = 1;
            data_.fire = false;
            data_ptr_ = &data_;
        }
        void Connect(AtmosHolder* guest,
                     int level_owner = MAX_GAS_LEVEL, int level_guest = MAX_GAS_LEVEL,
                     int div = MAX_GAS_LEVEL / 2 /*100% - owner take all*/);
        void AddEnergy(int energy);
        void AddGase(int gase, int amount);
        void SetVolume(int volume);
        int GetEnergy() const;
        int GetPressure() const;
        int GetTemperature() const;
        int GetVolume() const;
        int GetGase(int gase) const;

        int RemoveGase(int gase, int amount);

        void Truncate();

        void Ignite();
        bool IsBurning() const { return data_ptr_->fire; }

        void UpdateMacroParams();
        void SetAtmosData(AtmosData* data)
        {
            data_ptr_ = data;
        }
    private:
        AtmosData* data_ptr_;
        AtmosData data_;
    };

    void AddDefaultValues(atmos::AtmosHolder* holder);

    inline unsigned int Hash(const atmos::AtmosHolder& atmos_holder)
    {
        unsigned int retval = 0;
        for (quint32 i = 0; i < atmos::GASES_NUM; ++i)
        {
            retval += atmos_holder.data_ptr_->gases[i];
        }
        retval += atmos_holder.data_ptr_->energy;
        retval += atmos_holder.data_ptr_->pressure;
        retval += atmos_holder.data_ptr_->volume;
        retval += atmos_holder.data_ptr_->temperature;
        retval += atmos_holder.data_ptr_->fire;
        return retval;
    }

}
