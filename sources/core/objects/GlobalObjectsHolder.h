#pragma once

#include "Object.h"

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
inline unsigned int hash(const QPair<TFirst, TSecond>& pair)
{
    unsigned int retval = 0;
    retval += hash(pair.first);
    retval += hash(pair.second);
    return retval;
}

namespace kv
{
class UnsyncGenerator;

class GlobalObjectsHolder : public Object
{
public:
    DECLARE_SAVEABLE(GlobalObjectsHolder, Object);
    REGISTER_CLASS_AS(GlobalObjectsHolder);
    GlobalObjectsHolder();

    QMap<quint32, QPair<QString, int>> KV_SAVEABLE(musics_for_mobs);
    IdPtr<UnsyncGenerator> KV_SAVEABLE(unsync_generator);
    QMap<quint32, quint32> KV_SAVEABLE(players_table);
};
END_DECLARE(GlobalObjectsHolder);

}

