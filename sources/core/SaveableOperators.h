#pragma once

#include <QMap>
#include <QVector>

#include "Hashes.h"
#include "core_headers/FastSerializer.h"

namespace kv
{

template<class TKey, class TValue>
inline FastSerializer& operator<<(FastSerializer& file, const QMap<TKey, TValue>& map)
{
    file << static_cast<quint32>(map.size());
    // Not range-based because for Qt map it will be only values without keys
    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        file << it.key();
        file << it.value();
    }
    return file;
}

template<class TKey, class TValue>
inline FastDeserializer& operator>>(FastDeserializer& file, QMap<TKey, TValue>& map)
{
    quint32 size;
    file >> size;
    for (quint32 i = 0; i < size; ++i)
    {
        TKey key;
        file >> key;
        TValue value;
        file >> value;
        map.insert(key, value);
    }
    return file;
}

template<class TFirst, class TSecond>
inline FastSerializer& operator<<(FastSerializer& file, const QPair<TFirst, TSecond>& pair)
{
    file << pair.first;
    file << pair.second;
    return file;
}

template<class TFirst, class TSecond>
inline FastDeserializer& operator>>(FastDeserializer& file, QPair<TFirst, TSecond>& pair)
{
    file >> pair.first;
    file >> pair.second;
    return file;
}

template<class TValue>
inline FastSerializer& operator<<(FastSerializer& file, const QVector<TValue>& vector)
{
    file << vector.size();
    for (const auto& value : vector)
    {
        file << value;
    }
    return file;
}

template<class TValue>
inline FastDeserializer& operator>>(FastDeserializer& file, QVector<TValue>& vector)
{
    int size;
    file >> size;
    vector.resize(size);
    for (int i = 0; i < size; ++i)
    {
        file >> vector[i];
    }
    return file;
}

}

template<class TValue>
inline unsigned int Hash(const QVector<TValue>& vector)
{
    using kv::Hash;

    unsigned int retval = 0;
    for (int i = 0; i < vector.size(); ++i)
    {
        retval += (i + 1) * Hash(vector[i]);
    }
    return retval;
}

template<class TFirst, class TSecond>
inline unsigned int Hash(const QPair<TFirst, TSecond>& pair)
{
    using kv::Hash;

    unsigned int retval = 0;
    retval += Hash(pair.first);
    retval += Hash(pair.second);
    return retval;
}

template<class TKey, class TValue>
inline unsigned int Hash(const QMap<TKey, TValue>& map)
{
    using kv::Hash;

    unsigned int retval = 0;
    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        retval += Hash(it.key());
        retval += Hash(it.value());
    }
    return retval;
}
