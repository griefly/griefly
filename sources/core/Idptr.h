#pragma once

#include <cstdlib>

#include <iostream>

#include <QDebug>

#include "FastIsType.h"
#include "KvAbort.h"
#include "FastSerializer.h"

namespace kv
{
    class Object;
}

struct ObjectInfo
{
    ObjectInfo()
        : object(nullptr)
    {
        // Nothing
    }
    kv::Object* object;
};

extern QVector<ObjectInfo>* id_ptr_id_table;

struct IdPtrBase
{
protected:
    mutable kv::Object* casted_;
    quint32 id_;
};

template<class T>
class IdPtr : public IdPtrBase
{
    template<class U>
    friend FastDeserializer& operator<<(FastDeserializer& stream, const IdPtr<U>& ptr);
    template<class U>
    friend FastSerializer& operator>>(FastSerializer& stream, IdPtr<U>& ptr);
    template<class U>
    friend class IdPtr;
public:
    IdPtr()
    {
        id_ = 0;
        casted_ = nullptr;
    }
    IdPtr(quint32 id)
    {
        *this = id;
    }
    IdPtr(const IdPtr& other)
    {
        *this = other;
    }

    template<class U>
    IdPtr(const IdPtr<U>& other)
    {
        *this = other;
    }
    template<class U>
    bool operator==(const IdPtr<U>& other) const
    {
        return id_ == other.id_;
    }
    template<class U>
    bool operator!=(const IdPtr<U>& other) const
    {
        return !operator==(other);
    }

    IdPtr& operator=(quint32 id)
    {
        id_ = id;
        casted_ = nullptr;
        return *this;
    }
    IdPtr& operator=(const IdPtr& other)
    {
        id_ = other.id_;
        casted_ = other.casted_;
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

    T& operator*() const
    {
        if (id_ == 0)
        {
            KvAbort("Unable to dereference IdPtr with 0 id");
        }
        if (casted_ == nullptr)
        {
            Update();
        }
        return *static_cast<T*>(casted_);
    }

    T* operator->() const
    {
        return &operator*();
    }

    bool IsValid() const
    {
        Update();
        return casted_ != nullptr;
    }
    explicit operator bool() const
    {
#if defined(KV_ID_PTR_VALID_CACHE)
        if (casted_)
        {
            return true;
        }
#endif // KV_ID_PTR_VALID_CACHE
        if (IsValid())
        {
            return true;
        }
        return false;
    }
    quint32 Id() const
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

        kv::Object* local = GetFromIdTable(id_);
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

    static kv::Object* GetFromIdTable(quint32 id)
    {
        if (static_cast<int>(id) >= id_ptr_id_table->size())
        {
            KvAbort(QString("Id table lookup fail, id: %1, size: %2")
                .arg(id).arg(id_ptr_id_table->size()));
        }
        return (*id_ptr_id_table)[id].object;
    }
};

template<typename T>
FastSerializer& operator<<(FastSerializer& stream, const IdPtr<T>& ptr)
{
    stream << ptr.Id();
    return stream;
}

template<typename T>
FastDeserializer& operator>>(FastDeserializer& stream, IdPtr<T>& ptr)
{
    quint32 id;
    stream >> id;
    ptr = id;
    return stream;
}
