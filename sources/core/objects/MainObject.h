#pragma once

#include "KvAbort.h"

#include "../FastIsType.h"
#include "../Constheader.h"
#include "../Idptr.h"
#include "../Hashes.h"
#include "../Macroses.h"

#include "Interfaces.h"

#ifdef _BUILD_TESTS
#include <gtest/gtest.h>
#endif // _BUILD_TESTS

class IOnMapBase;
class IMainObject
{
    friend class ObjectFactory;
#ifdef _BUILD_TESTS
    FRIEND_TEST(MainObjectDeathTest, Deaths);
    FRIEND_TEST(MainObject, SettersAndGettersAndCreateImpl);
    FRIEND_TEST(MainObject, Save);
    FRIEND_TEST(MainObject, Delete);
#endif // _BUILD_TESTS
public:
    virtual ~IMainObject() { }

    void PlayMusic(const QString& name, float volume = 100.0f);

    virtual void Save(FastSerializer& serializer);
    virtual void Load(FastDeserializer& deserializer);

    virtual unsigned int Hash()
    {
        unsigned int retval = 0;
        retval += hash(GetId());
        retval += hash(GetFreq());
        return retval;
    }

    virtual void Delete();
    IMainObject() { id_ = 0; how_often_ = 0; game_ = nullptr; }
    IMainObject(NotLoadItem) { id_ = 0; how_often_ = 0; game_ = nullptr; }
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

    static IMainObject* _Z_creator() { return new IMainObject(); }
    static IMainObject* _Z_creatorSaved() { return new IMainObject(nouse);}

    virtual void Process() { }

    quint32 GetId() const { return id_; }

    void SetFreq(int freq);
    int GetFreq() const { return how_often_; }

    GameInterface& GetGame();
    const GameInterface& GetGame() const;
    ObjectFactoryInterface& GetFactory();
    unsigned int GetRand();

    template<typename T>
    IdPtr<T> Create(const QString& type, IdPtr<IOnMapBase> owner = 0)
    {
        IdPtr<T> retval = CreateImpl(type, owner.Id());
        if (!retval.IsValid())
        {
            KvAbort(QString("Unable to cast object: %1").arg(type));
        }
        return retval;
    }
private:
    quint32 CreateImpl(const QString& type, quint32 owner = 0);

    GameInterface* game_;
    quint32 id_;
    int how_often_;
};
ADD_TO_TYPELIST(IMainObject);
