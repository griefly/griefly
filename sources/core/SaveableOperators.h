#pragma once

#include <QMap>
#include <QVector>
#include <bitset>

#include "Hashes.h"
#include "FastSerializer.h"

namespace kv
{

template<long unsigned int N>
inline unsigned int Hash(std::bitset<N>& value)
{
    return value.to_ulong();
}

template<long unsigned int N>
inline FastDeserializer& operator>>(FastDeserializer& file, std::bitset<N>& bset)
{
    qint32 number;
    file >> number;
    bset = std::bitset<N>(number);
    return file;
}

template<long unsigned int N>
inline FastSerializer& operator<<(FastSerializer& file, const std::bitset<N>& bset)
{
    file << static_cast<qint32>(bset.to_ulong());
    return file;
}

template<class T>
std::enable_if_t<std::is_enum<T>::value, kv::FastSerializer&>
operator<<(kv::FastSerializer& file, const T& dir)
{
    file << static_cast<std::underlying_type_t<T>>(dir);
    return file;
}

template<class T>
std::enable_if_t<std::is_enum<T>::value, kv::FastDeserializer&>
operator>>(kv::FastDeserializer& file, T& dir)
{
    std::underlying_type_t<T> temp;
    file >> temp;
    dir = static_cast<T>(temp);
    return file;
}

template<class T>
std::enable_if_t<std::is_enum<T>::value, unsigned int> Hash(T value)
{
    return kv::Hash(static_cast<std::underlying_type_t<T>>(value));
}

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
