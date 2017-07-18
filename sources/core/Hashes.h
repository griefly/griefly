#pragma once

#include <cstring>

#include <map>

#include <QString>

#include "Idptr.h"

namespace std
{
    template<>
    class hash<QString> {
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

template<class T>
unsigned int Hash(std::vector<IdPtr<T>>& content)
{
    unsigned int retval = 0;
    int i = 1;
    for (auto it = content.begin(); it != content.end(); ++it, ++i)
    {
        retval += it->Id() * i;
    }
    return retval;
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

}
