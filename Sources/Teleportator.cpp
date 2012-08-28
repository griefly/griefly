#include "Teleportator.h"

#include "MapClass.h"

void Teleportator::processGUImsg(std::string& msg)
{   
    SYSTEM_STREAM << "void Teleportator::processGUImsg(std::string& msg)\n";
    if (msg == "NEWMOB")
        CreateItem(hash("ork"));
}

Teleportator::Teleportator()   
{
    SetSprite("icons/teleportator.png");
}

void Teleportator::CreateItem(size_t hash)
{
    auto newmob = IMainItem::fabric->newItemOnMap<IMob>(hash, posx, posy);
    SYSTEM_STREAM << "NEW MOB CREATE HIS ID " << newmob.ret_id() << "\n";
    newmob->onMobControl = true;
    if (IMainItem::mobMaster->GetMode() == SERVER)
        wait_queue_.Put(newmob.ret_id());
}

size_t Teleportator::WaitCreateItem()
{
    return wait_queue_.Get();
}