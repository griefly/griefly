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

    void PlaySoundIfVisible(const std::string& name, quint32 tile_id);
    void PlayMusic(const std::string& name, float volume = 100.0f);

    static const int THIS_COUNTER = __COUNTER__; 
    virtual bool Save(std::stringstream& file);
    virtual bool Load(std::stringstream& file);

    virtual unsigned int Hash()
    {
        return GetId();
    }

    virtual void Delete();
    IMainObject(quint32 id) { id_ = id; how_often_ = 0; game_ = nullptr; }
    IMainObject(NotLoadItem) { id_ = 0; how_often_ = 0; game_ = nullptr; }
    virtual void AfterWorldCreation() { }
    virtual const std::string& T_ITEM() const
    {
        return T_ITEM_S();
    }
    static const std::string& T_ITEM_S()
    {                                         
        static std::string result = "main";
        return result;
    }
    static int REAL_TYPE_ITEM;
    virtual int RT_ITEM() const
    {                                         
        return REAL_TYPE_ITEM;
    }                                         
    static int RT_ITEM_S()
    {                                         
        return REAL_TYPE_ITEM;
    }       
    virtual void Process() { }

    void SetId(quint32 id);
    quint32 GetId() const { return id_; }

    void SetFreq(int freq);
    int GetFreq() const { return how_often_; }
    void SetGame(IGame* game) { game_ = game; }
protected:
    IGame& GetGame();
    const IGame& GetGame() const;
    IObjectFactory& GetFactory();
    unsigned int GetRand();

    template<typename T>
    IdPtr<T> Create(const std::string& type, IdPtr<IOnMapBase> owner = 0)
    {
        IdPtr<T> retval = CreateImpl(type, owner.Id());
        if (!retval.IsValid())
        {
            qDebug() << "Unable to cast object: " << QString::fromStdString(type);
            KvAbort();
        }
        return retval;
    }
private:
    quint32 CreateImpl(const std::string& type, quint32 owner = 0);

    IGame* game_;
    quint32 id_;
    int how_often_;
};
ADD_TO_TYPELIST(IMainObject);
