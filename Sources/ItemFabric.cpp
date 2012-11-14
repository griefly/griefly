#include "MainInt.h"
#include "OnMapInt.h"

#include "ItemFabric.h"
#include "MapClass.h"
#include "sync_random.h"

#include "MagicStrings.h"

#include "Debug.h"

ItemFabric::ItemFabric()
{
    idTable_.resize(100);
    id_ = 1;
}

void ItemFabric::Sync()
{
    if (MAIN_TICK % HASH_OFTEN == 0)
    {
        hash_last_ = hash_all();

        // Debug::UnsyncDebug().Save();

        Message msg;

        msg.type = Net::SYSTEM_TYPE;
        msg.text = Net::HASH;
        msg.from = hash_last_;

        IMainItem::mobMaster->net_client->Send(msg);
    }
}

void ItemFabric::foreachProcess()
{
    size_t table_size = idTable_.size();
    for (size_t i = 1; i < table_size; ++i)
        if (idTable_[i] != nullptr)
            idTable_[i]->process();
}

void ItemFabric::saveMapHeader(std::stringstream& savefile)
{
    savefile << MAIN_TICK << std::endl;
    savefile << id_ << std::endl;
    savefile << IMainItem::map->mobi->thisMob.ret_id() << std::endl;

    // Random save
    savefile << random_helpers::get_seed() << std::endl;
    savefile << random_helpers::get_calls_counter() << std::endl;

    savefile << IMainItem::mobMaster->GetCreator() << std::endl;
}

void ItemFabric::loadMapHeader(std::stringstream& savefile, size_t real_this_mob)
{
    savefile >> MAIN_TICK;
    savefile >> id_;
    size_t loc;
    savefile >> loc;
    if (real_this_mob == 0)
        IMainItem::map->mobi->thisMob = loc;
    else
        IMainItem::map->mobi->thisMob = real_this_mob;
    
    unsigned int new_seed;
    unsigned int new_calls_counter;
    savefile >> new_seed;
    savefile >> new_calls_counter;
    random_helpers::set_rand(new_seed, new_calls_counter);

    size_t new_creator;
    savefile >> new_creator;
    IMainItem::mobMaster->SetCreator(new_creator);

    idTable_.resize(id_ + 1);
}

void ItemFabric::saveMap(const char* path)
{
    std::fstream rfile;
    rfile.open(path, std::ios_base::out | std::ios_base::trunc);
    if(rfile.fail()) 
    {
        SYSTEM_STREAM << "Error open " << path << std::endl; 
        return;
    }
    std::stringstream savefile;
    saveMap(savefile);
    rfile << savefile.str();
    rfile.close();
}
void ItemFabric::saveMap(std::stringstream& savefile)
{
    saveMapHeader(savefile);
    auto it = ++idTable_.begin();
    while(it != idTable_.end())
        if(*it) 
        {
            (*it++)->saveSelf(savefile);
            savefile << std::endl;
        }
        else 
            ++it;
    savefile << "0 ~";
}

void ItemFabric::loadMap(const char* path)
{
    std::stringstream savefile;
    std::fstream rfile;
    rfile.open(path, std::ios_base::in);

    rfile.seekg (0, std::ios::end);
    std::streamoff length = rfile.tellg();
    rfile.seekg (0, std::ios::beg);
    char* buff = new char[(unsigned int)(length + 1)];
    buff[length + 1] = '\0';

    rfile.read(buff, length);
    rfile.close();
    savefile << buff;
    delete[] buff;
    //
    loadMap(savefile);
}

void ItemFabric::loadMap(std::stringstream& savefile, size_t real_this_mob)
{
    clearMap();
    loadMapHeader(savefile, real_this_mob);
    int j = 0;
    while(!savefile.eof())
    {
        j++;
        if(savefile.fail())
        {
            SYSTEM_STREAM << "Error! " << j << "\n";
            SDL_Delay(10000);
        }
        unsigned int type;
        savefile >> type;
        if(type == 0) 
            break;

        size_t id_loc;
        savefile >> id_loc;
 
        size_t id_master;
        savefile >> id_master;
        
        id_ptr_on<IMainItem> i;
        i = newItemSaved<IMainItem>(id_master, type, id_loc);
        i->loadSelf(savefile);
    }
    SYSTEM_STREAM << "\n NUM OF ELEMENTS CREATED: " << j << "\n";
    IMainItem::map->mobi->changeMob(IMainItem::map->mobi->thisMob);
}

IMainItem* ItemFabric::newVoidItem(unsigned int type)
{
    static Initer init;
    return (*itemList())[type]();
};

IMainItem* ItemFabric::newVoidItemSaved(unsigned int type)
{
    static Initer init;
    return (*itemListSaved())[type]();
};

void ItemFabric::clearMap()
{
    size_t table_size = idTable_.size();
    for (size_t i = 1; i < table_size; ++i)
        if (idTable_[i] != nullptr)
            idTable_[i]->delThis();
};

unsigned int ItemFabric::hash_all()
{
    unsigned int h = 0;
    size_t table_size = idTable_.size();
    for (size_t i = 1; i < table_size; ++i)
        if (idTable_[i] != nullptr)
            h += idTable_[i]->hashSelf();
    return h;
}