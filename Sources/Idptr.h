#pragma once
#include <hash_map>
#include <strstream>
#include <fstream>

#include "Macroses.h"

class IMainItem;
class MapMaster;
class Manager;




typedef IMainItem* (*item_creator)();
typedef unsigned int (*type_item)();

std::hash_map<unsigned int, item_creator>* itemList();

std::hash_map<unsigned int, item_creator>* itemListSaved();

template<class T>
class itemAdder 
{
public:
    static IMainItem* creator()
    {
        return new T;
    };
    static IMainItem* creatorSaved()
    {
        return new T(nouse);
    };
    itemAdder()
    {
        //T item;
        unsigned int key = T::T_ITEM_S();
        (*itemList())[key] = &creator;
        (*itemListSaved())[key] = &creatorSaved;
    };
};

class id_ptr
{
public:
    size_t id;
    IMainItem* operator*();
    IMainItem* operator->();
    id_ptr operator=(size_t id_new);
};

template<typename T>
class id_ptr_on
{
public:
    id_ptr_on()
    {
        id = 0;
        item = nullptr;
    }
    T* operator*()
    {
        if(0 == IMainItem::fabric->idTable()[id])
            return item = 0;
        //if(!item)
        //    item = castTo<T>(IMainItem::map->idTable[id]);
        return item = castTo<T>(IMainItem::fabric->idTable()[id]);
    }
    T* operator->()
    {
        if(0 == IMainItem::fabric->idTable()[id])
            return item = 0;
        //if(!item)
        //    item = castTo<T>(IMainItem::map->idTable[id]);
        return item = castTo<T>(IMainItem::fabric->idTable()[id]);
    }
    id_ptr_on& operator=(size_t id_new)
    {
        id = id_new;
        return *this;
    }
    id_ptr_on& operator=(id_ptr id_new)
    {
        id = id_new.id;
        return *this;
    }
    template<typename T2>
    id_ptr_on& operator=(id_ptr_on<T2> value)
    {
        item = castTo<T>(value.ret_item());
        id = value.ret_id();
        return *this;
    }
    template<typename T2>
    id_ptr_on(id_ptr_on<T2>& value)
    {
        //item = castTo<T>(value.ret_item());
        id = value.ret_id();
    }
    operator bool()
    {
        return operator->() != nullptr; 
    }
    id_ptr_on(size_t id_new)
    {
        id = id_new;
    }
    size_t ret_id()
    {
        return id;
    }
    T* ret_item()
    {
        item = castTo<T>(IMainItem::fabric->idTable()[id]); 
        return item;
    }
private:
    T* item; 
    size_t id;
};

template<typename T>
std::iostream& operator<<(std::iostream& stream, id_ptr_on<T> ptr)
{
    stream << ptr.ret_id();
    return stream;
}

template<typename T>
std::iostream& operator>>(std::iostream& stream, id_ptr_on<T>& ptr)
{
    size_t l;
    stream >> l;
    ptr = l;
    return stream;
}