#pragma once

#include <vector>

#include "Idptr.h"
#include "objects/MaterialObject.h"

class ObjectFactory : public ObjectFactoryInterface
{
public:
    ObjectFactory(GameInterface* game);
    virtual ~ObjectFactory();

    virtual std::vector<ObjectInfo>& GetIdTable() override;
    virtual const std::vector<ObjectInfo>& GetIdTable() const override;

    virtual quint32 Hash() const override;

    virtual void BeginWorldCreation() override;
    virtual void FinishWorldCreation() override;
    virtual void MarkWorldAsCreated() override;

    virtual quint32 CreateImpl(const QString& type, quint32 owner = 0) override;
    virtual kv::Object* CreateVoid(const QString& hash, quint32 id_new) override;

    virtual void DeleteLater(quint32 id) override;
    virtual void ProcessDeletion() override;

    virtual void Clear() override;

    virtual int GetId() const override { return id_; }
    virtual void SetId(int id) override { id_ = id; }
private:
    static kv::Object* NewVoidObject(const QString& type);
    static kv::Object* NewVoidObjectSaved(const QString& type);

    GameInterface* game_;

    QByteArray saved_map_;

    bool is_world_generating_;

    QVector<kv::Object*> ids_to_delete_;

    std::vector<ObjectInfo> objects_table_;

    quint32 id_;
};
