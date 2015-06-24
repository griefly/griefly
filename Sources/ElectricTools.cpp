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

void AtmosTool::AttackBy(id_ptr_on<Item> item)
{
    if (item.ret_id() == GetId())
    {
        id_ptr_on<CubeTile> ct = GetOwner()->GetOwner(); 

        std::stringstream ss;
        ss << "Energy: " << ct->GetAtmosHolder()->GetEnergy() << std::endl;
        ss << "Volume: " << ct->GetAtmosHolder()->GetVolume() << std::endl;
        ss << "Temperature: " << ct->GetAtmosHolder()->GetTemperature() << std::endl;
        ss << "Pressure: " << ct->GetAtmosHolder()->GetPressure() << std::endl;
        for (size_t i = 0; i < GASES_NUM; ++i)
        {
            ss << GASES_NAME[i] << ": " << ct->GetAtmosHolder()->GetGase(i) << std::endl;
        }
 
        Chat::GetChat()->PostTextFor(ss.str(), GetOwner());
    }
}
