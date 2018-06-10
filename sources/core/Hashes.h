#pragma once

#include <QHash>
#include <QString>
#include <bitset>

namespace std
{
    template<>
    class hash<QString>
    {
    public:
        size_t operator()(const QString& value) const
        {
            return static_cast<size_t>(qHash(value));
        }
    };
}

namespace kv
{

inline unsigned int Hash(const QString& str)
{
    unsigned int len = str.length();
    unsigned int hash;
    unsigned int i;
    for(hash = i = 0; i < len; ++i)
    {
        hash += str[i].unicode();
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash + 1;
}

inline unsigned int Hash(unsigned int value)
{
    return value;
}

inline unsigned int Hash(int value)
{
    // The conversion is well-defined
    return static_cast<unsigned int>(value);
}

inline unsigned int Hash(bool value)
{
    return static_cast<unsigned int>(value);
}

inline unsigned int Hash(qint64 value)
{
    return static_cast<quint32>(value) + static_cast<quint32>(value >> 32);
}

inline unsigned int Hash(const std::bitset<4>& value)
{
    return value.to_ulong();
}

}
