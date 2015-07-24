#pragma once

#include <string>

bool FastIsType(int typeto, int typefrom);

template<typename Typeto>
inline bool FastIsType(int typefrom)
{
    return FastIsType(Typeto::RT_ITEM_S(), typefrom);
}

template<typename Typeto, typename TypeObjectFrom>
inline Typeto* castTo(TypeObjectFrom* ptr)
{
    /*return dynamic_cast<Typeto*>(ptr);/**/
    /**/if(ptr == nullptr)
        return nullptr;
    if(FastIsType<Typeto>(ptr->RT_ITEM()))
        return reinterpret_cast<Typeto*>(ptr);
    return nullptr;/**/
}

const std::string& Num2Hash(int hash);

/*class Initer
{
public:
    Initer();
};*/
