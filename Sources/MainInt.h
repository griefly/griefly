#pragma once

#include "FastIsType.h"
#include "constheader.h"
#include "Idptr.h"
#include "ASpritesClass.h"
#include "hashes.h"
#include "ItemFabric.h"

class IMob;
class MapMaster;
class CMonstr;
class CFirebullet;
class Manager;
class SmallItem;
class Liquid;
class IMainItem;
class IOnMapItem;

#define DECLARE_RAW_TYPE(thisclass, supclass) DECLARE_RAW_LOADED DECLARE_GET_TYPE_ITEM(thisclass, supclass)//load from raw, type

class IMainItem
{
public:
    static const int THIS_COUNTER = __COUNTER__; 
    virtual bool saveSelf(std::stringstream& file);                              
    virtual bool loadSelf(std::stringstream& file);                                                        

    virtual unsigned int hashSelf()
    {
        return GetId() + master.ret_id();
    }

    virtual void delThis();
    IMainItem(){ id_ = 0; master = 0; how_often_ = 0;};
    IMainItem(NotLoadItem) {id_ = 0; master = 0; how_often_ = 0;};
    static Manager* mobMaster;
    static MapMaster* map;
    static ItemFabric* fabric;
    virtual unsigned int T_ITEM()            
    {                                        
        static unsigned int result = 0;
        if(result) return result;            
        result = hash("main");                  
        return result;                       
    }                                        
    static unsigned int T_ITEM_S()            
    {                                         
        static unsigned int result = 0;       
        if(result) return result;             
        result = hash("main");                   
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
    virtual void process(){};

    void SetId(size_t id);
    size_t GetId() const { return id_; }

    void SetFreq(int freq);
    int GetFreq() const { return how_often_; }
public:
    id_ptr_on<IOnMapItem> master;
private:
    size_t id_;
    int how_often_;
};
ADD_TO_TYPELIST(IMainItem);