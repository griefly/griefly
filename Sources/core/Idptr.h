#pragma once

#include <stdlib.h>

#include <unordered_map>
//#include <sstream>
#include <fstream>

#include "Macroses.h"
#include "FastIsType.h"

class IMainObject;
class MapMaster;
class Game;




typedef IMainObject* (*item_creator)(size_t id);
typedef IMainObject* (*item_creator_saved)();
typedef unsigned int (*type_item)();

std::unordered_map<std::string, item_creator>* itemList();

std::unordered_map<std::string, item_creator_saved>* itemListSaved();

IMainObject* GetFromIdTable(size_t id);

template<typename T>
class id_ptr_on
{
public:
    id_ptr_on()
    {
        id = 0;
    }
    template<class T2>
    bool operator==(const id_ptr_on<T2>& rval)
    {
        return ret_id() == rval.ret_id();
    }

    T* operator*()
    {
        if (id == 0)
        {
            return nullptr;
        }
        IMainObject* local = GetFromIdTable(id);
        if (nullptr == local)
        {
            return nullptr;
        }
        return castTo<T>(local);
    }
    const T* operator*() const
    {
        if (id == 0)
        {
            return nullptr;
        }
        IMainObject* local = GetFromIdTable(id);
        if (nullptr == local)
        {
            return nullptr;
        }
        return castTo<T>(local);
    }
    T* operator->()
    {
        return operator*();
    }
    const T* operator->() const
    {
        return operator*();
    }
    id_ptr_on& operator=(size_t id_new)
    {
        id = id_new;
        return *this;
    }
    template<typename T2>
    id_ptr_on& operator=(id_ptr_on<T2> value)
    {
        id = value.ret_id();
        return *this;
    }
    template<typename T2>
    id_ptr_on(id_ptr_on<T2> value)
    {
        id = value.ret_id();
    }

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
    }
    size_t ret_id() const
    {
        return id;
    }
private:
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
