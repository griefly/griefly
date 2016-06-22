#pragma once

#include "../FastIsType.h"
#include "../Constheader.h"
#include "../Idptr.h"
#include "../Hashes.h"

#include "Interfaces.h"

#include <gtest/gtest.h>

class IOnMapBase;
class IMainObject
{
public:
    FRIEND_TEST(MainObjectDeathTest, Deaths);

    virtual ~IMainObject() { }

    void PlaySoundIfVisible(const std::string& name, size_t tile_id);
    void PlayMusic(const std::string& name, float volume = 100.0f);

    static const int THIS_COUNTER = __COUNTER__; 
    virtual bool Save(std::stringstream& file);
    virtual bool Load(std::stringstream& file);

    virtual unsigned int Hash()
    {
        return GetId();
    }

    virtual void Delete();
    IMainObject(size_t id) { id_ = id; how_often_ = 0; game_ = nullptr; }
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

    void SetId(size_t id);
    size_t GetId() const { return id_; }

    void SetFreq(int freq);
    int GetFreq() const { return how_often_; }
    void SetGame(IGame* game) { game_ = game; }
protected:
    IGame& GetGame();
    const IGame& GetGame() const;
    IObjectFactory& GetFactory();
    unsigned int GetRand();

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
private:
    size_t CreateImpl(const std::string& type, id_ptr_on<IOnMapBase> owner = 0);

    IGame* game_;
    size_t id_;
    int how_often_;
};
ADD_TO_TYPELIST(IMainObject);
