#pragma once

#include <vector>
#include <memory>

#include "Idptr.h"
#include "objects/OnMapBase.h"
#include "objects/OnMapObject.h"

#include "FastStringstream.h"

class ObjectFactory
{
public:
    ObjectFactory(Game* game);

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
        return objects_table_;
    }

    void ForeachProcess();

    static IMainObject* NewVoidObject(const std::string& type, size_t id);
    static IMainObject* NewVoidObjectSaved(const std::string& type);

    unsigned int Hash();

    void SaveMapHeader(std::stringstream& str);
    void SaveMap(const char* path);
    void SaveMap(std::stringstream& str);

    void LoadMapHeader(std::stringstream& savefile, size_t real_this_mob);
    void LoadMap(const char* path);
    void LoadMap(std::stringstream& str, size_t real_this_mob);
    
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

        item->game_ = game_;

        if (id_ >= objects_table_.size())
        {
            objects_table_.resize(id_ * 2);
        }
        objects_table_[id_] = item;
        ++id_;
        bool is_turf = std::is_base_of<ITurf, T>::value || std::is_same<ITurf, T>::value;
        if (owner.valid() && is_turf)
        {
            qDebug() << "is_turf == true";
            owner->SetTurf(item->GetId());
        }
        else if (owner.valid() && !owner->AddItem(item->GetId()))
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
        item->game_ = game_;
        if (id_new >= objects_table_.size())
        {
            objects_table_.resize(id_new * 2);
        }

        if (id_new >= id_)
        {
            id_ = id_new + 1;
        }
        objects_table_[id_new] = item;
        item->SetId(id_new);
        return item->GetId();
    }

    void AddProcessingItem(id_ptr_on<IMainObject> item);

    void ClearProcessing();

    void SetPlayerId(size_t net_id, size_t real_id);
    size_t GetPlayerId(size_t net_id);
    size_t GetNetId(size_t real_id);

    FastStringstream* GetFastStream() { return &stream_; }
private:
    Game* game_;

    FastStringstream stream_;
    QByteArray saved_map_;

    bool is_world_generating_;

    void UpdateProcessingItems();

    std::vector<IMainObject*> objects_table_;
    std::vector<id_ptr_on<IMainObject>> process_table_;

    std::vector<id_ptr_on<IMainObject>> add_to_process_;

    unsigned int hash_last_;
    unsigned int hash_last_tick_;

    size_t id_;

    std::map<size_t, size_t> players_table_;

};

ObjectFactory& GetFactory();
void SetFactory(ObjectFactory* item_fabric);
