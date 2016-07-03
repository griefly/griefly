#pragma once

#include <stdlib.h>

#include <iostream>

#include <QDebug>

#include "FastIsType.h"
#include "KVAbort.h"

class IMainObject;

extern std::vector<IMainObject*>* id_ptr_id_table;

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
        return castTo<T>(local);
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
        return (*id_ptr_id_table)[id];
    }

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
