#pragma once

#include <vector>

#include "Idptr.h"
#include "objects/OnMapObject.h"

class IOnMapBase;

class ObjectFactory : public ObjectFactoryInterface
{
public:
    ObjectFactory(GameInterface* game);
    virtual ~ObjectFactory();

    virtual QVector<ObjectInfo>& GetIdTable() override;

    virtual void ForeachProcess() override;

    virtual unsigned int Hash() override;

    virtual void BeginWorldCreation() override;
    virtual void FinishWorldCreation() override;
    virtual void MarkWorldAsCreated() override;

    virtual quint32 CreateImpl(const QString& type, quint32 owner = 0) override;
    virtual IMainObject* CreateVoid(const QString& hash, quint32 id_new) override;

    virtual void DeleteLater(quint32 id) override;
    virtual void ProcessDeletion() override;

    virtual void AddProcessingItem(quint32 item) override;
    virtual void Clear() override;

    virtual int GetId() override { return id_; }
    virtual void SetId(int id) override { id_ = id; }
private:
    void ClearProcessing();

    static IMainObject* NewVoidObject(const QString& type);
    static IMainObject* NewVoidObjectSaved(const QString& type);

    GameInterface* game_;

    QByteArray saved_map_;

    bool is_world_generating_;

    void UpdateProcessingItems();

    QVector<IMainObject*> ids_to_delete_;

    QVector<ObjectInfo> objects_table_;
    QVector<IdPtr<IMainObject>> process_table_;

    QVector<IdPtr<IMainObject>> add_to_process_;

    quint32 id_;
};
