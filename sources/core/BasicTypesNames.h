#pragma once

#include <QString>

#include <core_headers/Mapgen.h>

namespace kv
{

namespace impl
{

template<class T>
struct TypeName
{
    static QString Get()
    {
        return mapgen::key::type::UNKNOWN;
    }
    TypeName() = delete;
};

} // namespace impl

template<class T>
QString GetTypeName()
{
    return impl::TypeName<T>::Get();
};

namespace impl
{

template<>
struct TypeName<bool>
{
    static QString Get()
    {
        return mapgen::key::type::BOOL;
    }
    TypeName() = delete;
};

template<>
struct TypeName<qint32>
{
    static QString Get()
    {
        return mapgen::key::type::INT32;
    }
    TypeName() = delete;
};

template<>
struct TypeName<quint32>
{
    static QString Get()
    {
        return mapgen::key::type::UINT32;
    }
    TypeName() = delete;
};

template<>
struct TypeName<QString>
{
    static QString Get()
    {
        return mapgen::key::type::STRING;
    }
    TypeName() = delete;
};

template<>
struct TypeName<QByteArray>
{
    static QString Get()
    {
        return mapgen::key::type::BYTEARRAY;
    }
    TypeName() = delete;
};

template<>
struct TypeName<qint64>
{
    static QString Get()
    {
        return mapgen::key::type::INT64;
    }
    TypeName() = delete;
};

/*template<class T>
std::enable_if<std::is_enum<T>::value, QString> GetTypeName<T>()
{

}*/

} // namespace impl

} // namespace kv

