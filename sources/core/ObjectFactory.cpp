#include "ObjectFactory.h"

#include "KvAbort.h"

#include "objects/Object.h"
#include "objects/MaterialObject.h"
#include "objects/PhysicsEngine.h"
#include "Map.h"
#include "SynchronizedRandom.h"
#include "AutogenMetadata.h"
#include "WorldLoaderSaver.h"

#include "objects/GlobalObjectsHolder.h"

ObjectFactory::ObjectFactory(GameInterface* game)
{
    objects_table_.resize(100);
    id_ = 1;
    is_world_generating_ = true;
    game_ = game;
    id_ptr_id_table = &objects_table_;
}

ObjectFactory::~ObjectFactory()
{
    ProcessDeletion();
    for (auto& info : objects_table_)
    {
        if (info.object)
        {
            delete info.object;
        }
    }
}

std::vector<ObjectInfo>& ObjectFactory::GetIdTable()
{
    return objects_table_;
}

const std::vector<ObjectInfo>& ObjectFactory::GetIdTable() const
{
    return objects_table_;
}

kv::Object* ObjectFactory::NewVoidObject(const QString& type)
{
    auto creator = GetItemsCreators()->find(type);
    if (creator == GetItemsCreators()->end())
    {
        kv::Abort(QString("Unable to find creator for type: %1").arg(type));
    }
    return creator->second();
}

kv::Object* ObjectFactory::NewVoidObjectSaved(const QString& type)
{
    auto creator = GetVoidItemsCreators()->find(type);
    if (creator == GetVoidItemsCreators()->end())
    {
        kv::Abort(QString("Unable to find void creator for type: %1").arg(type));
    }
    return creator->second();
}

void ObjectFactory::Clear()
{
    const quint32 table_size = static_cast<quint32>(objects_table_.size());
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            delete objects_table_[i].object;
        }
    }
    if (table_size != objects_table_.size())
    {
        qDebug() << "WARNING: table_size != idTable_.size()!";
    }

    ids_to_delete_.clear();

    id_ = 1;
}

void ObjectFactory::BeginWorldCreation()
{
    is_world_generating_ = true;
}

void ObjectFactory::FinishWorldCreation()
{
    MarkWorldAsCreated();
    const quint32 table_size = static_cast<quint32>(objects_table_.size());
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            objects_table_[i].object->AfterWorldCreation();
        }
    }
}

void ObjectFactory::MarkWorldAsCreated()
{
    is_world_generating_ = false;
}

quint32 ObjectFactory::CreateImpl(const QString &type, quint32 owner_id)
{
    kv::Object* item = NewVoidObject(type);
    kv::internal::GetObjectGame(item) = game_;

    if (id_ >= objects_table_.size())
    {
        objects_table_.resize(id_ * 2);
    }
    objects_table_[id_].object = item;

    kv::internal::GetObjectId(item) = id_;
    item->SetFreq(item->GetFreq());

    quint32 retval = id_;
    ++id_;
    IdPtr<kv::MapObject> owner = owner_id;
    if (owner.IsValid())
    {
        if (CastTo<kv::Turf>(item) != nullptr)
        {
            owner->SetTurf(item->GetId());
        }
        else if (!owner->AddObject(item->GetId()))
        {
            kv::Abort("AddItem failed");
        }
    }

    if (item->ToHearer())
    {
        game_->GetGlobals()->hearers.append(retval);
    }

    if (!is_world_generating_)
    {
        item->AfterWorldCreation();
    }
    return retval;
}

kv::Object* ObjectFactory::CreateVoid(const QString &hash, quint32 id_new)
{
    kv::Object* item = NewVoidObjectSaved(hash);
    kv::internal::GetObjectGame(item) = game_;
    if (id_new >= objects_table_.size())
    {
        objects_table_.resize(id_new * 2);
    }

    if (id_new >= id_)
    {
        id_ = id_new + 1;
    }
    objects_table_[id_new].object = item;
    kv::internal::GetObjectId(item) = id_new;
    item->SetFreq(item->GetFreq());
    return item;
}

quint32 ObjectFactory::CreateAssetImpl(const kv::Asset& asset, quint32 owner_id)
{
    auto object = kv::world::LoadObject(game_, asset.ToJsonForObjectCreation());
    IdPtr<kv::MapObject> owner = owner_id;
    if (owner.IsValid())
    {
        if (CastTo<kv::Turf>(object.operator->()) != nullptr)
        {
            owner->SetTurf(object->GetId());
        }
        else if (!owner->AddObject(object->GetId()))
        {
            kv::Abort("AddItem failed");
        }
    }
    return object->GetId();
}

void ObjectFactory::DeleteLater(quint32 id)
{
    ids_to_delete_.push_back(objects_table_[id].object);
    objects_table_[id].object = nullptr;
}

void ObjectFactory::ProcessDeletion()
{
    for (auto it = ids_to_delete_.begin(); it != ids_to_delete_.end(); ++it)
    {
        delete *it;
    }
    ids_to_delete_.clear();
}

quint32 ObjectFactory::Hash() const
{
    unsigned int h = 0;
    const quint32 table_size = static_cast<quint32>(objects_table_.size());
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            // TODO (?): i to hash
            h += objects_table_[i].object->HashMembers();
        }
    }

    return h;
}
