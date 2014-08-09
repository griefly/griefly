#pragma once

#include <stdlib.h>

#include <hash_map>
#include <strstream>
#include <fstream>

#include <SDL.h>

#if defined(__linux__)
#include <ext/hash_map>
namespace std { using namespace __gnu_cxx; }
#endif // linux

#include "Macroses.h"
#include "FastIsType.h"
#include "Idptr.h"

class IMainObject;
class MapMaster;
class Manager;




typedef IMainObject* (*item_creator)();
typedef unsigned int (*type_item)();

std::hash_map<unsigned int, item_creator>* itemList();

std::hash_map<unsigned int, item_creator>* itemListSaved();

template<class T>
class itemAdder 
{
public:
    static IMainObject* creator()
    {
        return new T;
    };
    static IMainObject* creatorSaved()
    {
        return new T(nouse);
    };
    itemAdder()
    {
        //T item;
        unsigned int key = T::T_ITEM_S();
        if (   itemList()->find(key) != itemList()->end()
            || itemListSaved()->find(key) != itemListSaved()->end())
        {
            SYSTEM_STREAM << "FATAL ERROR: " << key << " hash miss!" << std::endl;
            SDL_Delay(1000);
            abort();
        }
        (*itemList())[key] = &creator;
        (*itemListSaved())[key] = &creatorSaved;
    };
};

IMainObject* GetFromIdTable(size_t id);

template<typename T>
class id_ptr_on
{
public:
    id_ptr_on()
    {
        id = 0;
        item = nullptr;
    }
    template<class T2>
    bool operator==(const id_ptr_on<T2>& rval)
    {
        return ret_id() == rval.ret_id();
    }
    /* template<class R>
    operator id_ptr_on<R>()
    {
        id_ptr_on<R> retval;
        retval = ret_id();
        return retval;
    }
    template<class R>
    operator id_ptr_on<R>() const
    {
        id_ptr_on<R> retval;
        retval = ret_id();
        return retval;
    }*/
    T* operator*()
    {
        if(nullptr == GetFromIdTable(id))
            return nullptr;
        if (!item)
            item = castTo<T>(GetFromIdTable(id));
        return item;
    }
    const T* operator*() const
    {
        if(nullptr == GetFromIdTable(id))
            return nullptr;
        if (!item)
            item = castTo<T>(GetFromIdTable(id));
        return item;
    }
    T* operator->()
    {
        if(nullptr == GetFromIdTable(id))
            return nullptr;
        if (!item)
            item = castTo<T>(GetFromIdTable(id));
        return item;
    }
    const T* operator->() const
    {
        if(nullptr == GetFromIdTable(id))
            return nullptr;
        if (!item)
            item = castTo<T>(GetFromIdTable(id));
        return item;
    }
    id_ptr_on& operator=(size_t id_new)
    {
        id = id_new;
        item = nullptr;
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
    id_ptr_on(id_ptr_on<T2> value)
    {
        id = value.ret_id();
        item = nullptr;
    }

    ///operator size_t() { return id; }

    bool valid() const
    {
        return operator->() != nullptr; 
    }
    operator bool()
    {
        return valid();
    }
    id_ptr_on(size_t id_new)
    {
        id = id_new;
        item = nullptr;
    }
    size_t ret_id() const
    {
        return id;
    }
    T* ret_item()
    {
        if(nullptr == GetFromIdTable(id))
            return nullptr;
        if (!item)
            item = castTo<T>(GetFromIdTable(id)); 
        return item;
    }
private:
    mutable T* item; 
    size_t id;
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const id_ptr_on<T>& ptr)
{
    stream << ptr.ret_id();
    return stream;
}

template<typename T>
std::istream& operator>>(std::istream& stream, id_ptr_on<T>& ptr)
{
    size_t l;
    stream >> l;
    ptr = l;
    return stream;
}
