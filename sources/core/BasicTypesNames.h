#pragma once

#include <QString>

#include <core_headers/Mapgen.h>

namespace kv
{

// TODO: better namespaces

template<class T>
QString GetTypeName()
{
    return mapgen::key::type::UNKNOWN;
};

template<>
QString GetTypeName<bool>()
{
    return mapgen::key::type::BOOL;
}

template<>
QString GetTypeName<qint32>()
{
    return mapgen::key::type::INT32;
}

template<>
QString GetTypeName<quint32>()
{
    return mapgen::key::type::UINT32;
}

template<>
QString GetTypeName<QString>()
{
    return mapgen::key::type::STRING;
}

template<>
QString GetTypeName<QByteArray>()
{
    return mapgen::key::type::BYTEARRAY;
}

template<>
QString GetTypeName<qint64>()
{
    return mapgen::key::type::INT64;
}

} // namespace kv

