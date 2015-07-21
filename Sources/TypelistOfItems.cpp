#define KV_REAL_TYPE_SECTION

#include <assert.h>

#include <bitset>

#include "TypelistOfItems.h"
#include "FastIsType.h"

#include "MapClass.h"


std::bitset<ItemsTLLength> FastDC[ItemsTLLength];

template<typename Loc, typename T>
class ForEachInit;

template<typename Loc, typename T, typename U>
class ForEachInit<Loc, Loki::Typelist<T, U>>
{
public:
    static void InitEach()
    {
        FastDC[Loki::TL::IndexOf<ItemsTypelist, Loc>::value][Loki::TL::IndexOf<ItemsTypelist, T>::value] = 
            std::is_base_of<Loc, T>::value || std::is_same<Loc, T>::value;
        ForEachInit<Loc, U>::InitEach();
    };
};

template<typename Loc>
class ForEachInit<Loc, Loki::NullType>
{
public:
    static void InitEach() {};
};

template<typename Typelist>
class InitializeDCData;

template<>
class InitializeDCData<Loki::NullType>
{
public:
    static void Init() {}
};

template<typename T, typename U>
class InitializeDCData<Loki::Typelist<T, U>>
{
public:
    static void Init() 
    {
        itemAdder<T> loc;
        ForEachInit<T, ItemsTypelist>::InitEach();
        InitializeDCData<U>::Init();
    };
};

bool IsInited()
{
    static bool is_inited = false;
    if (is_inited == false)
    {
        is_inited = true;
        return false;
    }
    return true;
}

Initer::Initer()
{
    if (!IsInited())
        InitializeDCData<ItemsTypelist>::Init();
}

template<typename T>
class InitRealTypes;

template<typename T, typename U>
class InitRealTypes<Loki::Typelist<T, U>>
{
public:
    static void Init(int num)
    {
        T::REAL_TYPE_ITEM = ItemsTLLength - num;
        InitRealTypes<U>::Init(num - 1);
    }
};

template<>
class InitRealTypes<Loki::NullType>
{
public:
    static void Init(int num) {}
};

class IniteRT
{
public:
    IniteRT()
    {
        InitRealTypes<ItemsTypelist>::Init(ItemsTLLength);
    }
};

static Initer init4;
static IniteRT init2;

bool FastIsType(int typeto, int typefrom)
{
    static Initer init0;
    static IniteRT init1;
    return FastDC[typeto][typefrom];
};

std::string Num2HashData[ItemsTLLength];

template<typename T>
class InitNum2HashData;

template<typename T, typename U>
class InitNum2HashData<Loki::Typelist<T, U>>
{
public:
    static void Init(int num)
    {
        Num2HashData[ItemsTLLength - num] = T::T_ITEM_S();
        InitNum2HashData<U>::Init(num - 1);
    }
};

template<>
class InitNum2HashData<Loki::NullType>
{
public:
    static void Init(int num) {}
};

class IniterNum2Hash
{
public:
    IniterNum2Hash()
    {
        InitNum2HashData<ItemsTypelist>::Init(ItemsTLLength);
    }
};

const std::string& Num2Hash(int num)
{
    static IniterNum2Hash init;
    assert(num >= 0);
    assert(num < ItemsTLLength);
    return Num2HashData[num]; 
};
