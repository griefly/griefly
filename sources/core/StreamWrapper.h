#pragma once

#include <string>
#include <map>
#include <vector>

#include <QDebug>

#include "Constheader.h"
#include "Idptr.h"

template<class T>
inline kv::FastSerializer& WrapWriteMessage(kv::FastSerializer& file, T& to_write)
{
    file << to_write;
    return file;
}

template<class KeyType, class ValueType>
inline kv::FastSerializer& WrapWriteMessage(
        kv::FastSerializer& file,
        std::map<KeyType, ValueType>& to_write)
{
    file << static_cast<quint32>(to_write.size());
    for (auto it = to_write.begin(); it != to_write.end(); ++it)
    {
        WrapWriteMessage(file, it->first);
        WrapWriteMessage(file, it->second);
    }
    return file;
}

template<class T>
inline kv::FastDeserializer& WrapReadMessage(kv::FastDeserializer& file, T& to_read)
{
    file >> to_read;
    return file;
}

template<class KeyType, class ValueType>
inline kv::FastDeserializer& WrapReadMessage(
        kv::FastDeserializer& file,
        std::map<KeyType, ValueType>& to_read)
{
    quint32 size;
    file >> size;

    if (size == 0)
    {
        return file;
    }
    for (quint32 i = 0; i < size; ++i)
    {
        KeyType key;
        WrapReadMessage(file, key);
        ValueType value;
        WrapReadMessage(file, value);
        to_read[key] = value;
    }
    return file;
}
