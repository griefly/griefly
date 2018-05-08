#pragma once

#include <QString>

namespace kv
{

template<class T>
QString GetTypeName()
{
    return QStringLiteral("unknown");
};

template<>
QString GetTypeName<bool>()
{
    return QStringLiteral("bool");
}

template<>
QString GetTypeName<qint32>()
{
    return QStringLiteral("int32");
}

template<>
QString GetTypeName<quint32>()
{
    return QStringLiteral("uint32");
}

template<>
QString GetTypeName<QString>()
{
    return QStringLiteral("string");
}

template<>
QString GetTypeName<QByteArray>()
{
    return QStringLiteral("bytearray");
}

template<>
QString GetTypeName<qint64>()
{
    return QStringLiteral("int64");
}


} // namespace kv

