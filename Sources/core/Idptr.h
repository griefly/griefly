#pragma once

#include <stdlib.h>

#include <unordered_map>

#include "Macroses.h"
#include "FastIsType.h"

class IMainObject;

typedef IMainObject* (*item_creator)(size_t id);
typedef IMainObject* (*item_creator_saved)();
typedef unsigned int (*type_item)();

std::unordered_map<std::string, item_creator>* items_creators();

std::unordered_map<std::string, item_creator_saved>* items_void_creators();

extern std::vector<IMainObject*>* id_ptr_id_table;

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
    operator void*() const
    {
        if (valid())
        {
            return reinterpret_cast<void*>(0x1);
        }
        return nullptr;
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
    static IMainObject* GetFromIdTable(size_t id)
    {
        return (*id_ptr_id_table)[id];
    }

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
