#pragma once

#include <vector>

#include "Idptr.h"
#include "objects/OnMapObject.h"

class IOnMapBase;

class ObjectFactory : public IObjectFactory
{
public:
    ObjectFactory(IGame* game);
    virtual ~ObjectFactory();

    virtual QVector<ObjectInfo>& GetIdTable() override;

    virtual void ForeachProcess() override;

    virtual unsigned int Hash() override;

    virtual void Save(FastSerializer& str) override;
    virtual void Load(FastDeserializer& str, quint32 real_this_mob) override;
    
    virtual void LoadFromMapGen(const QString& name) override;

    virtual void BeginWorldCreation() override;
    virtual void FinishWorldCreation() override;

    virtual quint32 CreateImpl(const QString& type, quint32 owner = 0) override;

    virtual void DeleteLater(quint32 id) override;
    virtual void ProcessDeletion() override;

    virtual void AddProcessingItem(quint32 item) override;
private:
    void Clear();
    void ClearProcessing();

    void SaveMapHeader(FastSerializer& str);
    void LoadMapHeader(FastDeserializer& savefile);

    IMainObject* CreateVoid(const QString& hash, quint32 id_new);

    static IMainObject* NewVoidObject(const QString& type, quint32 id);
    static IMainObject* NewVoidObjectSaved(const QString& type);

    IGame* game_;

    QByteArray saved_map_;

    bool is_world_generating_;

    void UpdateProcessingItems();

    QVector<IMainObject*> ids_to_delete_;

    QVector<ObjectInfo> objects_table_;
    QVector<IdPtr<IMainObject>> process_table_;

    QVector<IdPtr<IMainObject>> add_to_process_;

    quint32 id_;
};
