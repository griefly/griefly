#pragma once

#include <vector>

#include "Idptr.h"
#include "objects/OnMapObject.h"

class IOnMapBase;

class ObjectFactory : public IObjectFactory
{
public:
    ObjectFactory(IGame* game);

    virtual unsigned int GetLastHash() override;
    virtual std::vector<IMainObject*>& GetIdTable() override;

    virtual void ForeachProcess() override;

    virtual unsigned int Hash() override;

    virtual void Save(std::stringstream& str) override;
    virtual void Load(std::stringstream& str, size_t real_this_mob) override;
    
    virtual void LoadFromMapGen(const std::string& name) override;

    virtual void BeginWorldCreation() override;
    virtual void FinishWorldCreation() override;

    virtual size_t CreateImpl(const std::string& type, id_ptr_on<IOnMapBase> owner = 0) override;

    virtual void DeleteLater(size_t id) override;
    virtual void ProcessDeletion() override;

    virtual void AddProcessingItem(id_ptr_on<IMainObject> item) override;

    virtual void ClearProcessing() override;

    virtual void SetPlayerId(size_t net_id, size_t real_id) override;
    virtual size_t GetPlayerId(size_t net_id) override;
    virtual size_t GetNetId(size_t real_id) override;
private:
    void Clear();

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
