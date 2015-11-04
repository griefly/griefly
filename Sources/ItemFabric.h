#pragma once

#include <vector>
#include <memory>

#include "Idptr.h"
#include "OnMapBase.h"
#include "OnMapObject.h"

class ObjectFactory
{
public:
    ObjectFactory();

    unsigned int GetLastHash()
    {
        return hash_last_;
    }
    unsigned int GetLastHashTick()
    {
        return hash_last_tick_;
    }

    std::vector<IMainObject*>& GetIdTable()
    {
        return idTable_;
    }

    void Sync();
    void ForeachProcess();

    static IMainObject* NewVoidObject(const std::string& type, size_t id);
    static IMainObject* NewVoidObjectSaved(const std::string& type);

    unsigned int Hash();

    void SaveMapHeader(std::stringstream& str);
    void SaveMap(const char* path);
    void SaveMap(std::stringstream& str, bool zip);

    void LoadMapHeader(std::stringstream& savefile, size_t real_this_mob = 0);
    void LoadMap(const char* path);
    void LoadMap(std::stringstream& str, bool zip, size_t real_this_mob = 0);
    
    void ClearMap();

    void BeginWorldCreation();
    void FinishWorldCreation();

    template<typename T>
    id_ptr_on<T> newItemOnMap(const std::string& hash, id_ptr_on<IOnMapBase> owner, size_t id_new = 0)
    {
        
        static_assert(std::is_same<IOnMapObject, T>::value || std::is_base_of<IOnMapObject, T>::value, "Error: MapMaster::newItemOnMap - type isn't derivied from IOnMapObject");
        T* item;
        if(std::max(id_new, id_) >= idTable_.size())
        {
            SYSTEM_STREAM << "RESIZE MAIN TABLE\n";
            idTable_.resize(std::max(id_new, id_) * 2);
        }
        if(id_new == 0)
            id_new = id_++;
        else if(id_new >= id_)
        {
            SYSTEM_STREAM << id_new << " " << id_ << "id_ptr_on<T> newItemOn\n";
            id_ = id_new + 1;
        }
        item = castTo<T>(NewVoidObject(hash, id_new));//TODO: FIX IT!(LOOK DOWN)
        if(item == 0)
        {
            SYSTEM_STREAM << "\nERROR! ERROR!\n";
            SDL_Delay(1000);
        }

        //item->SetId(id_new);
        idTable_[id_new] = item;

        owner->AddItem(item->GetId());

        if (!is_world_generating_)
        {
            item->AfterWorldCreation();
        }

        return item->GetId();
    }
    
    template<typename T>
    id_ptr_on<T> newItemSaved(const std::string& hash, size_t id_new = 0)
    {
        T* item;
        item = castTo<T>(NewVoidObjectSaved(hash));
        if(std::max(id_new, id_) >= idTable_.size()) 
            idTable_.resize(std::max(id_new, id_) * 2);
        if(id_new == 0)
            id_new = id_++;
        else if(id_new >= id_)
            id_ = id_new + 1;
        idTable_[id_new] = item;
        item->SetId(id_new);
//        item->master = master;
        id_ptr_on<T> ret_val;
        ret_val = item->GetId();
        return ret_val;
    }
    
    template<typename T>
    id_ptr_on<T> newItem(const std::string& hash, size_t id_new = 0)
    {
        T* item;
        if(std::max(id_new, id_) >= idTable_.size())
            idTable_.resize(std::max(id_new, id_) * 2);
        if(id_new == 0)
            id_new = id_++;
        else if(id_new >= id_)
            id_ = id_new + 1;
        item = castTo<T>(NewVoidObject(hash, id_new));
        idTable_[id_new] = item;
//        item->SetId(id_new);
//        item->master = master;
        id_ptr_on<T> ret_val;
        ret_val = item->GetId();
        if (!is_world_generating_)
        {
            item->AfterWorldCreation();
        }
        return ret_val;
    }

    void AddProcessingItem(id_ptr_on<IMainObject> item);

    void ClearProcessing();

    void SetPlayerId(size_t net_id, size_t real_id);
    size_t GetPlayerId(size_t net_id);
    size_t GetNetId(size_t real_id);
private:
    bool is_world_generating_;

    void UpdateProcessingItems();

    std::vector<IMainObject*> idTable_;
    std::vector<id_ptr_on<IMainObject>> process_table_;

    std::vector<id_ptr_on<IMainObject>> add_to_process_;

    unsigned int hash_last_;
    unsigned int hash_last_tick_;

    size_t id_;

    std::map<size_t, size_t> players_table_;

};

ObjectFactory& GetFactory();
void SetFactory(ObjectFactory* item_fabric);
