#pragma once

#include <vector>
#include <memory>

#include "Idptr.h"
#include "objects/OnMapObject.h"

class IOnMapBase;

class ObjectFactory
{
public:
    ObjectFactory(IGame* game);

    unsigned int GetLastHash();
    unsigned int GetLastHashTick();

    std::vector<IMainObject*>& GetIdTable();

    void ForeachProcess();

    unsigned int Hash();

    void Save(std::stringstream& str);
    void Load(std::stringstream& str, size_t real_this_mob);
    
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
private:
    void SaveMapHeader(std::stringstream& str);
    void LoadMapHeader(std::stringstream& savefile, size_t real_this_mob);

    IMainObject* CreateVoid(const std::string& hash, size_t id_new);

    static IMainObject* NewVoidObject(const std::string& type, size_t id);
    static IMainObject* NewVoidObjectSaved(const std::string& type);

    IGame* game_;

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
