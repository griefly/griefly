#pragma once

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

unsigned int Num2Hash(int hash);
int Hash2Num(unsigned int hash);

class Initer
{
public:
    Initer();
};