#pragma once

#include <QVector>
#include <QBitArray>

#include <kvengine_export.h>

#if defined(__linux__) || defined(__APPLE__)
#define __forceinline __attribute__((always_inline)) inline
#endif

namespace kv
{
    class Object;
}

extern KVENGINE_EXPORT std::vector<QBitArray>* cast_table;

__forceinline bool FastIsType(int typeto, int typefrom)
{
    return (*cast_table)[typefrom].testBit(typeto);
}

void InitCastTable();

template<typename Typeto>
inline bool FastIsType(int typefrom)
{
    return FastIsType(Typeto::GetTypeIndexStatic(), typefrom);
}

template<typename Typeto, typename TypeObjectFrom>
inline Typeto* CastTo(TypeObjectFrom* ptr)
{
    if (ptr == nullptr)
    {
        return nullptr;
    }
    if (FastIsType<Typeto>(ptr->GetTypeIndex()))
    {
        return static_cast<Typeto*>(static_cast<kv::Object*>(ptr));
    }
    return nullptr;
}
