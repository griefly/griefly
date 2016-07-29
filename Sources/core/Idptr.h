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

struct IdPtrBase
{
protected:
    mutable IMainObject* casted_;
    size_t id_;
};

template<class T>
class IdPtr : public IdPtrBase
{
    template<class U>
    friend std::ostream& operator<<(std::ostream& stream, const IdPtr<U>& ptr);
    template<class U>
    friend std::istream& operator>>(std::istream& stream, IdPtr<U>& ptr);
    template<class U>
    friend class IdPtr;
public:
    IdPtr()
    {
        id_ = 0;
        casted_ = nullptr;
    }
    IdPtr(size_t id)
    {
        *this = id;
    }
    template<class U>
    IdPtr(const IdPtr<U>& other)
    {
        *this = other;
    }
    template<class U>
    bool operator==(const IdPtr<U>& other)
    {
        return id_ == other.id_;
    }

    IdPtr& operator=(size_t id)
    {
        id_ = id;
        casted_ = nullptr;
        return *this;
    }
    template<class U>
    IdPtr& operator=(const IdPtr<U>& other)
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
            Update();
        }
        return reinterpret_cast<T*>(casted_);
    }

    T* operator->() const
    {
        return operator*();
    }

    bool IsValid() const
    {
        Update();
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
        if (IsValid())
        {
            return reinterpret_cast<void*>(0x1);
        }
        return nullptr;
    }
    size_t Id() const
    {
        return id_;
    }
private:
    void Update() const
    {
        if (id_ == 0)
        {
            casted_ = nullptr;
            return;
        }

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
        T* retval = CastTo<T>(local);
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
std::ostream& operator<<(std::ostream& stream, const IdPtr<T>& ptr)
{
    stream << ptr.id_;
    return stream;
}

template<typename T>
std::istream& operator>>(std::istream& stream, IdPtr<T>& ptr)
{
    stream >> ptr.id_;
    return stream;
}
