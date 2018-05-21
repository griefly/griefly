#pragma once

#include <QString>

#include <core_headers/Mapgen.h>

namespace kv
{

namespace impl
{

template<class T, class Unused = void>
struct TypeName;

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

template<class T>
struct TypeName<T, std::enable_if_t<std::is_enum<T>::value, void>>
{
    static QString Get()
    {
        return TypeName<std::underlying_type_t<T>>::Get();
    }
    TypeName() = delete;
};

template<class T>
// TODO: better solution with real default value
struct TypeName<T, std::enable_if_t<!std::is_enum<T>::value, void>>
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

} // namespace kv

