#pragma once

#include <hash_map>
#include <strstream>
#include <fstream>

#if defined(__linux__)
#include <ext/hash_map>
namespace std { using namespace __gnu_cxx; }
#endif // linux

#include "Macroses.h"
#include "FastIsType.h"
#include "Idptr.h"

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

IMainItem* GetFromIdTable(size_t id);

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
    T* operator*()
    {
        if(0 == GetFromIdTable(id))
            return item = 0;
        return item = castTo<T>(GetFromIdTable(id));
    }
    const T* operator*() const
    {
        if(0 == GetFromIdTable(id))
            return 0;
        return castTo<T>(GetFromIdTable(id));
    }
    T* operator->()
    {
        if(0 == GetFromIdTable(id))
            return item = 0;
        return item = castTo<T>(GetFromIdTable(id));
    }
    const T* operator->() const
    {
        if(0 == GetFromIdTable(id))
            return 0;
        return castTo<T>(GetFromIdTable(id));
    }
    id_ptr_on& operator=(size_t id_new)
    {
        id = id_new;
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
        id = value.ret_id();
    }
    bool valid() const
    {
        return operator->() != nullptr; 
    }
    id_ptr_on(size_t id_new)
    {
        id = id_new;
    }
    size_t ret_id() const
    {
        return id;
    }
    T* ret_item()
    {
        item = castTo<T>(GetFromIdTable(id)); 
        return item;
    }
private:
    T* item; 
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
