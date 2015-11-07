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
    id_ptr_on<T> Create(const std::string& type, id_ptr_on<IOnMapBase> owner = 0)
    {
        T* item = castTo<T>(NewVoidObject(type, id_));
        if (item == 0)
        {
            qDebug() << "Creation type mismatch: " << QString::fromStdString(type);
            abort();
        }

        if (id_ >= idTable_.size())
        {
            idTable_.resize(id_ * 2);
        }
        idTable_[id_] = item;
        ++id_;
        if (owner.valid() && !owner->AddItem(item->GetId()))
        {
            qDebug() << "AddItem failed";
            abort();
        }

        if (!is_world_generating_)
        {
            item->AfterWorldCreation();
        }
        return item->GetId();
    }
    
    template<typename T>
    id_ptr_on<T> CreateVoid(const std::string& hash, size_t id_new)
    {
        T* item = castTo<T>(NewVoidObjectSaved(hash));
        if (id_new >= idTable_.size())
        {
            idTable_.resize(id_new * 2);
        }

        if (id_new >= id_)
        {
            id_ = id_new + 1;
        }
        idTable_[id_new] = item;
        item->SetId(id_new);
        return item->GetId();
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
