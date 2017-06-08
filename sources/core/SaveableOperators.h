#pragma once

#include <QMap>
#include <QVector>

#include "FastSerializer.h"

template<class TKey, class TValue>
inline FastSerializer& operator<<(FastSerializer& file, const QMap<TKey, TValue>& map)
{
    file << map.size();
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
    int size;
    file >> size;
    for (int i = 0; i < size; ++i)
    {
        TKey key;
        file >> key;
        TValue value;
        file >> value;
        map.insert(key, value);
    }
    return file;
}

template<class TKey, class TValue>
inline unsigned int hash(const QMap<TKey, TValue>& map)
{
    unsigned int retval = 0;
    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        retval += hash(it.key());
        retval += hash(it.value());
    }
    return retval;
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

template<class TFirst, class TSecond>
inline unsigned int hash(const QPair<TFirst, TSecond>& pair)
{
    unsigned int retval = 0;
    retval += hash(pair.first);
    retval += hash(pair.second);
    return retval;
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

template<class TValue>
inline unsigned int hash(const QVector<TValue>& vector)
{
    unsigned int retval = 0;
    for (int i = 0; i < vector.size(); ++i)
    {
        retval += (i + 1) * hash(vector[i]);
    }
    return retval;
}
