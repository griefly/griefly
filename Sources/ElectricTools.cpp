#include "ElectricTools.h"

#include <sstream>

#include "Chat.h"
#include "AtmosHolder.h"
#include "TileInt.h"

AtmosTool::AtmosTool(size_t id) : Item(id)
{
    SetSprite("icons/device.dmi");
    SetState("atmos");
    //SetState("cutters");
    name = "Atmos tool";
}

std::string AtmosTool::GetInfo(AtmosHolder &holder)
{
    std::stringstream ss;
    ss << "Energy: " << holder.GetEnergy() << std::endl;
    ss << "Volume: " << holder.GetVolume() << std::endl;
    ss << "Temperature: " << holder.GetTemperature() << std::endl;
    ss << "Pressure: " << holder.GetPressure() << std::endl;
    for (size_t i = 0; i < GASES_NUM; ++i)
    {
        ss << GASES_NAME[i] << ": " << holder.GetGase(i) << std::endl;
    }
    return ss.str();
}

void AtmosTool::AttackBy(id_ptr_on<Item> item)
{
    if (item.ret_id() == GetId())
    {
        if (id_ptr_on<CubeTile> ct = GetOwner()->GetOwner())
        {
            Chat::GetChat()->PostTextFor(GetInfo(*ct->GetAtmosHolder()), GetOwner());
        }
    }
}
