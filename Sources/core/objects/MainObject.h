#pragma once

#include "../FastIsType.h"
#include "../Constheader.h"
#include "../Idptr.h"
#include "../Hashes.h"

class IMainObject
{
    friend class ObjectFactory;
public:
    void PlaySoundIfVisible(const std::string& name, size_t tile_id);
    void PlayMusic(const std::string& name, float volume = 100.0f);

    static const int THIS_COUNTER = __COUNTER__; 
    virtual bool saveSelf(std::stringstream& file);                              
    virtual bool loadSelf(std::stringstream& file);                                                        

    virtual unsigned int hashSelf()
    {
        return GetId();
    }

    virtual void Delete();
    IMainObject(size_t id){ id_ = id; how_often_ = 0; game_ = nullptr; }
    IMainObject(NotLoadItem) { id_ = 0; how_often_ = 0; game_ = nullptr; }
    virtual void AfterWorldCreation() { }
    virtual const std::string& T_ITEM()
    {                                        
        return T_ITEM_S();
    }                                        
    static const std::string& T_ITEM_S()
    {                                         
        static std::string result = "main";
        return result;
    }
    static int REAL_TYPE_ITEM;                
    virtual int RT_ITEM()                     
    {                                         
        return REAL_TYPE_ITEM;                
    }                                         
    __forceinline static int RT_ITEM_S()                    
    {                                         
        return REAL_TYPE_ITEM;                
    }       
    virtual void Process() { }

    void SetId(size_t id);
    size_t GetId() const { return id_; }

    void SetFreq(int freq);
    int GetFreq() const { return how_often_; }
protected:
    Game* game_;
private:
    size_t id_;
    int how_often_;
};
ADD_TO_TYPELIST(IMainObject);
