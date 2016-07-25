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

struct id_ptr_base
{
protected:
    mutable IMainObject* casted_;
    size_t id_;
};

template<class T>
class id_ptr_on : public id_ptr_base
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
        casted_ = nullptr;
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
        casted_ = nullptr;
        return *this;
    }
    template<class U>
    id_ptr_on& operator=(const id_ptr_on<U>& other)
    {
#if defined(KV_ID_PTR_CASTING_CACHE)
        if (other.id_ == 0)
        {
            id_ = 0;
            casted_ = nullptr;
            return *this;
        }
        if (other.casted_ != nullptr)
        {
            if (std::is_base_of<T, U>::value)
            {
                casted_ = other.casted_;
                id_ = other.id_;
                return *this;
            }
            T* retval = castTo<T>(other.casted_);
            if (retval)
            {
                casted_ = retval;
                id_ = other.id_;
                return *this;
            }
            id_ = 0;
            casted_ = nullptr;
            return *this;
        }
#endif // KV_ID_PTR_CASTING_CACHE
        *this = other.id_;
        return *this;
    }

    T* operator*() const
    {
        if (id_ == 0)
        {
            return nullptr;
        }
        if (casted_ == nullptr)
        {
            update();
        }
        return reinterpret_cast<T*>(casted_);
    }

    T* operator->() const
    {
        return operator*();
    }

    bool valid() const
    {
        update();
        return operator*() != nullptr;
    }
    operator void*() const
    {
#if defined(KV_ID_PTR_VALID_CACHE)
        if (casted_)
        {
            return reinterpret_cast<void*>(0x1);
        }
#endif // KV_ID_PTR_VALID_CACHE
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
    void update() const
    {
        IMainObject* local = GetFromIdTable(id_);
        if (local == nullptr)
        {
            casted_ = nullptr;
            return;
        }
        if (casted_)
        {
            return;
        }
        T* retval = castTo<T>(local);
        if (retval)
        {
            casted_ = retval;
        }
    }

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
    // Dynamic memory allocation is disabled
    static void* operator new(size_t);
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
