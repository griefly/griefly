#pragma once

#include <stdlib.h>

#include <unordered_map>
//#include <sstream>
#include <fstream>

#include <SDL.h>

#include "Macroses.h"
#include "FastIsType.h"

class IMainObject;
class MapMaster;
class Manager;




typedef IMainObject* (*item_creator)(size_t id);
typedef IMainObject* (*item_creator_saved)();
typedef unsigned int (*type_item)();

std::unordered_map<std::string, item_creator>* itemList();

std::unordered_map<std::string, item_creator_saved>* itemListSaved();

template<class T>
class itemAdder 
{
public:
    static IMainObject* creator(size_t id)
    {
        return new T(id);
    };
    static IMainObject* creatorSaved()
    {
        return new T(nouse);
    };
    itemAdder()
    {
        //T item;
        std::string key = T::T_ITEM_S();
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
        if (id == 0)
            return nullptr;
        if (nullptr == GetFromIdTable(id))
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

template<class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<id_ptr_on<T>>& to_write)
{
    file << static_cast<size_t>(to_write.size()) << " ";
    for (auto it = to_write.begin(); it != to_write.end(); ++it)
    {
        file << *it;
    }
    return file;
}

template<class T>
std::istream& operator>>(std::istream& stream, std::vector<id_ptr_on<T>>& to_read)
{
    size_t size;
    file >> size;

    char c;
    file.get(c);
    if (c != ' ')
    {
        SYSTEM_STREAM << "Error: corrupted string info" << std::endl;
        return file;
    }

    qDebug() << "Size: " << size;

    to_read.resize(size);
    for (size_t i = 0; i < size; ++i)
    {
        size_t new_item;
        file >> new_item;
        qDebug() << "new item: " << new_item;
        to_read[i] = new_item;
    }

    return file;
}
