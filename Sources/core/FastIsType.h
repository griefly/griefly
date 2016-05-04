#pragma once

#include <string>

// TODO: inline
bool FastIsType(int typeto, int typefrom);

template<typename Typeto>
inline bool FastIsType(int typefrom)
{
    return FastIsType(Typeto::RT_ITEM_S(), typefrom);
}

template<typename Typeto, typename TypeObjectFrom>
inline Typeto* castTo(TypeObjectFrom* ptr)
{
    if (ptr == nullptr)
    {
        return nullptr;
    }
    if (FastIsType<Typeto>(ptr->RT_ITEM()))
    {
        return reinterpret_cast<Typeto*>(ptr);
    }
    return nullptr;
}
