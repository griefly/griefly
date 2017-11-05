#pragma once

#include "core_headers/KvAbort.h"
#include "core/KvGlobals.h"
#include "core/Hashes.h"

#include "../FastIsType.h"
#include "../Idptr.h"
#include "../KvMacros.h"

#include "Interfaces.h"

#ifdef _BUILD_TESTS
#include <gtest/gtest.h>
#endif // _BUILD_TESTS

void InitSettersForTypes();

namespace kv
{

class MapObject;
class Hearer;

namespace internal
{
    GameInterface*& GetObjectGame(Object* object);
    quint32& GetObjectId(Object* object);
    quint32 CreateImpl(Object* object, const QString& type, quint32 owner = 0);
}

class Object
{
    friend GameInterface*& internal::GetObjectGame(Object* object);
    friend quint32& internal::GetObjectId(Object* object);
    friend quint32 internal::CreateImpl(Object* object, const QString& type, quint32 owner);
public:
    Object(const Object& other) = delete;
    Object& operator=(const Object& other) = delete;

    Object() { id_ = 0; how_often_ = 0; game_ = nullptr; }
    Object(kv::internal::NoInitialization) { id_ = 0; how_often_ = 0; game_ = nullptr; }

    virtual ~Object() { }

    void PlayMusic(const QString& name, int volume = 100);

    virtual void Save(FastSerializer& serializer);
    virtual void Load(FastDeserializer& deserializer);

    virtual unsigned int HashMembers()
    {
        unsigned int retval = 0;
        retval += Hash(GetId());
        retval += Hash(GetFreq());
        return retval;
    }

    virtual void Delete();
    virtual void AfterWorldCreation() { }
    virtual const QString& GetType() const
    {
        return GetTypeStatic();
    }
    static const QString& GetTypeStatic();
    static int TYPE_INDEX;
    virtual int GetTypeIndex() const
    {
        return TYPE_INDEX;
    }
    static int GetTypeIndexStatic()
    {
        return TYPE_INDEX;
    }

    static Object* _Z_KV_Creator() { return new Object(); }
    static Object* _Z_KV_VoidCreator() { return new Object(kv::internal::no_initialization);}

    virtual void Process() { }

    quint32 GetId() const { return id_; }

    void SetFreq(int freq);
    int GetFreq() const { return how_often_; }

    GameInterface& GetGame();
    const GameInterface& GetGame() const;
    ObjectFactoryInterface& GetFactory();
    quint32 GenerateRandom();

    virtual Hearer* ToHearer() { return nullptr; }
    void PostHtmlFor(const QString& html, IdPtr<Object> mob);
    void PostVisible(const QString& html, const Position& place);

    qint32 GetGameTick() const;

    template<typename T>
    IdPtr<T> Create(const QString& type, IdPtr<MapObject> owner = 0)
    {
        IdPtr<T> retval = CreateImpl(type, owner.Id());
        if (!retval.IsValid())
        {
            kv::Abort(QString("Unable to cast object: %1").arg(type));
        }
        return retval;
    }
private:
    quint32 CreateImpl(const QString& type, quint32 owner = 0);

    GameInterface* game_;
    quint32 id_;
    int how_often_;
};

namespace internal
{
    inline GameInterface*& GetObjectGame(Object* object)
    {
        return object->game_;
    }
    inline quint32& GetObjectId(Object* object)
    {
        return object->id_;
    }
    inline quint32 CreateImpl(Object* object, const QString& type, quint32 owner)
    {
        return object->CreateImpl(type, owner);
    }
}
}
