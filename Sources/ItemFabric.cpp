#include "MainInt.h"
#include "OnMapInt.h"

#include "ItemFabric.h"
#include "MapClass.h"
//#include "NetClass.h"

ItemFabric::ItemFabric()
{
    idTable_.resize(100);
    id_ = 1;
}

void ItemFabric::Sync()
{
    if (MAIN_TICK % HASH_OFTEN == 0)
        hash_last_ = hash_all();
}

void ItemFabric::foreachProcess()
{
    size_t table_size = idTable_.size();
    for (int i = 1; i < table_size; ++i)
        if (idTable_[i] != nullptr)
            idTable_[i]->process();
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
    savefile << MAIN_TICK << std::endl;
    savefile << id_ << std::endl;
    savefile << IMainItem::map->mobi->thisMob.ret_id() << std::endl;
    saveMap(savefile);
    rfile << savefile.str();
    rfile.close();
}
void ItemFabric::saveMap(std::stringstream& savefile)
{
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
    savefile >> MAIN_TICK;
    savefile >> id_;
    size_t loc;
    savefile >> loc;
    IMainItem::map->mobi->thisMob = loc;
    
    idTable_.resize(id_ + 1);

    loadMap(savefile);
    IMainItem::map->mobi->changeMob(IMainItem::map->mobi->thisMob);
    //savefile.close();
}

void ItemFabric::loadMap(std::stringstream& savefile)
{
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
    for (int i = 1; i < table_size; ++i)
        if (idTable_[i] != nullptr)
            idTable_[i]->delThis();
};

unsigned int ItemFabric::hash_all()
{
    unsigned int h = 0;
    size_t table_size = idTable_.size();
    for (int i = 1; i < table_size; ++i)
        if (idTable_[i] != nullptr)
            h += idTable_[i]->hashSelf();
    return h;
}