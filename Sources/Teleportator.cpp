#include "Teleportator.h"

#include "MobInt.h"
#include "MapClass.h"
#include "ItemFabric.h"
#include "MagicStrings.h"

void Teleportator::processGUImsg(const Message& msg)
{   
    SYSTEM_STREAM << "void Teleportator::processGUImsg(std::string& msg)\n";
    if (msg.text == Net::MAKE_NEW)
        CreateItem(msg.from, hash("Human"));
}

Teleportator::Teleportator(size_t id) : IMessageReceiver(id)
{
    SetSprite("icons/teleportator.png");
}

void Teleportator::CreateItem(unsigned int from_id, size_t hash)
{
    auto newmob = GetItemFabric()->newItemOnMap<IMob>(hash, owner);
    SYSTEM_STREAM << "NEW MOB CREATE HIS ID " << newmob.ret_id() << "\n";
    newmob->onMobControl = true;
    
    GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());

    Message msg;
    msg.from = from_id;
    msg.to = newmob.ret_id();
    msg.type = Net::SYSTEM_TYPE;
    msg.text = Net::NEW_MAKED;

    NetClient::GetNetClient()->Send(msg);
}
