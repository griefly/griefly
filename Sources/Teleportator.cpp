#include "Teleportator.h"

#include "MapClass.h"

#include "MagicStrings.h"

void Teleportator::processGUImsg(const Message& msg)
{   
    SYSTEM_STREAM << "void Teleportator::processGUImsg(std::string& msg)\n";
    if (msg.text == Net::MAKE_NEW)
        CreateItem(msg.from, hash("ork"));
}

Teleportator::Teleportator()   
{
    SetSprite("icons/teleportator.png");
}

void Teleportator::CreateItem(unsigned int from_id, size_t hash)
{
    auto newmob = IMainItem::fabric->newItemOnMap<IMob>(hash, posx, posy);
    SYSTEM_STREAM << "NEW MOB CREATE HIS ID " << newmob.ret_id() << "\n";
    newmob->onMobControl = true;

    Message msg;
    msg.from = from_id;
    msg.to = newmob.ret_id();
    msg.type = Net::SYSTEM_TYPE;
    msg.text = Net::NEW_MAKED;

    IMainItem::mobMaster->net_client->Send(msg);
}