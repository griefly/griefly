#pragma once

#include <vector>
#include <memory>

#include "Idptr.h"
#include "objects/OnMapObject.h"

#include "FastStringstream.h"

class IOnMapBase;

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

    unsigned int Hash();

    void SaveMapHeader(std::stringstream& str);
    void SaveMap(const char* path);
    void SaveMap(std::stringstream& str);

    void LoadMapHeader(std::stringstream& savefile, size_t real_this_mob);
    void LoadMap(const char* path);
    void LoadMap(std::stringstream& str, size_t real_this_mob);
    
    void LoadFromMapGen(const std::string& name);

    void ClearMap();

    void BeginWorldCreation();
    void FinishWorldCreation();

    size_t CreateImpl(const std::string& type, id_ptr_on<IOnMapBase> owner = 0);

    template<typename T>
    id_ptr_on<T> Create(const std::string& type, id_ptr_on<IOnMapBase> owner = 0)
    {
        id_ptr_on<T> retval = CreateImpl(type, owner);
        if (!retval.valid())
        {
            qDebug() << "Unable to cast object: " << QString::fromStdString(type);
            abort();
        }
        return retval;
    }

    void DeleteLater(size_t id);
    void ProcessDeletion();

    void AddProcessingItem(id_ptr_on<IMainObject> item);

    void ClearProcessing();

    void SetPlayerId(size_t net_id, size_t real_id);
    size_t GetPlayerId(size_t net_id);
    size_t GetNetId(size_t real_id);

    FastStringstream* GetFastStream() { return &stream_; }
private:
    IMainObject* CreateVoid(const std::string& hash, size_t id_new);

    static IMainObject* NewVoidObject(const std::string& type, size_t id);
    static IMainObject* NewVoidObjectSaved(const std::string& type);

    Game* game_;

    FastStringstream stream_;
    QByteArray saved_map_;

    bool is_world_generating_;

    void UpdateProcessingItems();

    std::vector<IMainObject*> ids_to_delete_;

    std::vector<IMainObject*> objects_table_;
    std::vector<id_ptr_on<IMainObject>> process_table_;

    std::vector<id_ptr_on<IMainObject>> add_to_process_;

    unsigned int hash_last_;
    unsigned int hash_last_tick_;

    size_t id_;

    std::map<size_t, size_t> players_table_;

};
