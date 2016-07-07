#pragma once

#include <stdlib.h>

#include <iostream>

#include <QDebug>

#include "FastIsType.h"
#include "KVAbort.h"

class IMainObject;

struct ObjectInfo
{
    ObjectInfo()
        : object(nullptr)
    {
        // Nothing
    }
    IMainObject* object;
};

extern std::vector<ObjectInfo>* id_ptr_id_table;

template<class T>
class id_ptr_on
{
    template<class U>
    friend std::ostream& operator<<(std::ostream& stream, const id_ptr_on<U>& ptr);
    template<class U>
    friend std::istream& operator>>(std::istream& stream, id_ptr_on<U>& ptr);
    template<class U>
    friend class id_ptr_on;
public:
    id_ptr_on()
    {
        id_ = 0;
        casted_ = false;
    }
    id_ptr_on(size_t id)
    {
        *this = id;
    }
    template<class U>
    id_ptr_on(const id_ptr_on<U>& other)
    {
        *this = other;
    }
    template<class U>
    bool operator==(const id_ptr_on<U>& other)
    {
        return id_ == other.id_;
    }

    id_ptr_on& operator=(size_t id)
    {
        id_ = id;
        casted_ = false;
        return *this;
    }
    template<class U>
    id_ptr_on& operator=(const id_ptr_on<U>& other)
    {
        *this = other.id_;
        return *this;
    }

    T* operator*() const
    {
        if (id_ == 0)
        {
            return nullptr;
        }
        IMainObject* local = GetFromIdTable(id_);
        if (local == nullptr)
        {
            return nullptr;
        }
        if (casted_)
        {
            return reinterpret_cast<T*>(local);
        }
        T* retval = castTo<T>(local);
        if (retval)
        {
            casted_ = true;
        }
        return retval;
    }

    T* operator->() const
    {
        return operator*();
    }

    bool valid() const
    {
        return operator*() != nullptr;
    }
    operator void*() const
    {
        if (valid())
        {
            return reinterpret_cast<void*>(0x1);
        }
        return nullptr;
    }
    size_t ret_id() const
    {
        return id_;
    }
private:
    static IMainObject* GetFromIdTable(size_t id)
    {
        if (id >= id_ptr_id_table->size())
        {
            qDebug() << "Id table lookup fail, id: "
                     << id << ", size: " << id_ptr_id_table->size();
            kv_abort();
        }
        return (*id_ptr_id_table)[id].object;
    }
    mutable bool casted_;
    size_t id_;
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const id_ptr_on<T>& ptr)
{
    stream << ptr.id_;
    return stream;
}

template<typename T>
std::istream& operator>>(std::istream& stream, id_ptr_on<T>& ptr)
{
    stream >> ptr.id_;
    return stream;
}
