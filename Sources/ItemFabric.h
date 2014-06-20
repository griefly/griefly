#pragma once

#include <vector>

#include "Idptr.h"
#include "OnMapBaseInt.h"

class ItemFabric
{
public:
    ItemFabric();
    size_t get_id() 
    { 
        return id_;
    }

    unsigned int get_hash_last()
    {
        return hash_last_;
    }

    std::vector<IMainItem*>& idTable()
    {
        return idTable_;
    }

    void Sync();
    void foreachProcess();

    static IMainItem* newVoidItem(unsigned int type);
    static IMainItem* newVoidItemSaved(unsigned int type);

    unsigned int hash_all();

    void saveMapHeader(std::stringstream& str);
    void saveMap(const char* path);
    void saveMap(std::stringstream& str, bool zip);

    void loadMapHeader(std::stringstream& savefile, size_t real_this_mob = 0);
    void loadMap(const char* path);
    void loadMap(std::stringstream& str, bool zip, size_t real_this_mob = 0);
    
    void clearMap();

    template<typename T>
    id_ptr_on<T> newItemOnMap(unsigned int hash, id_ptr_on<IOnMapBase> owner, size_t id_new = 0)
    {
        
        static_assert(std::tr1::is_same<IOnMapItem, T>::value || std::tr1::is_base_of<IOnMapItem, T>::value, "Error: MapMaster::newItemOnMap - type isn't derivied from IOnMapItem");
        T* item;
        item = castTo<T>(newVoidItem(hash));//TODO: FIX IT!(LOOK DOWN)
        if(item == 0)
        {
            SYSTEM_STREAM << "\nERROR! ERROR!\n";
            SDL_Delay(1000);
        }
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
        item->SetId(id_new);
        idTable_[id_new] = item;

        owner->AddItem(item->GetId());

        return item->GetId();
    }
    
    template<typename T, typename TMaster>
    id_ptr_on<T> newItemSaved(TMaster master, unsigned int hash, size_t id_new = 0)
    {
        T* item;
        item = castTo<T>(newVoidItemSaved(hash));
        if(std::max(id_new, id_) >= idTable_.size()) 
            idTable_.resize(std::max(id_new, id_) * 2);
        if(id_new == 0)
            id_new = id_++;
        else if(id_new >= id_)
            id_ = id_new + 1;
        idTable_[id_new] = item;
        item->SetId(id_new);
        item->master = master;
        id_ptr_on<T> ret_val;
        ret_val = item->GetId();
        return ret_val;
    }
    
    template<typename T, typename TMaster>
    id_ptr_on<T> newItem(TMaster master, unsigned int hash, size_t id_new = 0)
    {
        T* item;
        item = castTo<T>(newVoidItem(hash));
        if(std::max(id_new, id_) >= idTable_.size()) 
            idTable_.resize(std::max(id_new, id_) * 2);
        if(id_new == 0)
            id_new = id_++;
        else if(id_new >= id_)
            id_ = id_new + 1;
        idTable_[id_new] = item;
        item->SetId(id_new);
        item->master = master;
        id_ptr_on<T> ret_val;
        ret_val = item->GetId();
        return ret_val;
    }

    void AddProcessingItem(id_ptr_on<IMainItem> item);
    void RemoveProcessingItem(id_ptr_on<IMainItem> item);

    void SetPlayerId(size_t net_id, size_t real_id);
    size_t GetPlayerId(size_t net_id);
private:
    void UpdateProcessingItems();

    std::vector<IMainItem*> idTable_;
    std::vector<id_ptr_on<IMainItem>> process_table_;

    std::vector<id_ptr_on<IMainItem>> add_to_process_;
    std::vector<id_ptr_on<IMainItem>> remove_from_process_;

    unsigned int hash_last_;
    size_t id_;

    std::map<size_t, size_t> players_table_;

};

ItemFabric* GetItemFabric();
void SetItemFabric(ItemFabric* item_fabric);