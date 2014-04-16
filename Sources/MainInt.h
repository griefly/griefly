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
        return id + master.ret_id();
    }

    virtual void delThis();
    IMainItem(){ id = 0; master = 0; how_often = 0;};
    IMainItem(NotLoadItem){};
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
public:
    size_t id;
    int how_often;
    id_ptr_on<IOnMapItem> master;
};
ADD_TO_TYPELIST(IMainItem);