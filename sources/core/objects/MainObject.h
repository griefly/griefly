#pragma once

#include "KVAbort.h"

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
public:
#ifdef _BUILD_TESTS
    FRIEND_TEST(MainObjectDeathTest, Deaths);
    FRIEND_TEST(MainObject, SettersAndGettersAndCreateImpl);
#endif // _BUILD_TESTS
    virtual ~IMainObject() { }

    void PlaySoundIfVisible(const QString& name, quint32 tile_id);
    void PlayMusic(const QString& name, float volume = 100.0f);

    static const int THIS_COUNTER = __COUNTER__; 
    virtual bool Save(FastSerializer& serializer);
    virtual bool Load(FastDeserializer& deserializer);

    virtual unsigned int Hash()
    {
        return GetId() + GetFreq();
    }

    virtual void Delete();
    IMainObject(quint32 id) { id_ = id; how_often_ = 0; game_ = nullptr; }
    IMainObject(NotLoadItem) { id_ = 0; how_often_ = 0; game_ = nullptr; }
    virtual void AfterWorldCreation() { }
    virtual const QString& GetType() const
    {
        return GetTypeStatic();
    }
    static const QString& GetTypeStatic()
    {
        static QString type = "main";
        return type;
    }
    static int TYPE_INDEX;
    virtual int GetTypeIndex() const
    {
        return TYPE_INDEX;
    }
    static int GetTypeIndexStatic()
    {
        return TYPE_INDEX;
    }

    static IMainObject* _Z_creator(quint32 id) { return new IMainObject(id); }
    static IMainObject* _Z_creatorSaved() { return new IMainObject(nouse);}

    virtual void Process() { }

    void SetId(quint32 id);
    quint32 GetId() const { return id_; }

    void SetFreq(int freq);
    int GetFreq() const { return how_often_; }
    void SetGame(IGame* game) { game_ = game; }

    IGame& GetGame();
    const IGame& GetGame() const;
    IObjectFactory& GetFactory();
    unsigned int GetRand();

    template<typename T>
    IdPtr<T> Create(const QString& type, IdPtr<IOnMapBase> owner = 0)
    {
        IdPtr<T> retval = CreateImpl(type, owner.Id());
        if (!retval.IsValid())
        {
            qDebug() << "Unable to cast object: " << type;
            KvAbort();
        }
        return retval;
    }
private:
    quint32 CreateImpl(const QString& type, quint32 owner = 0);

    IGame* game_;
    quint32 id_;
    int how_often_;
};
ADD_TO_TYPELIST(IMainObject);
