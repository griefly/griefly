#ifndef MACROSES_H
#define MACROSES_H

#ifndef OPEN_STATIC_VARS
#define OPEN_STATIC_VARS false
#endif //OPEN_STATIC_VARS

#ifndef BR_CLOSE_IF_OPEN 
#define BR_CLOSE_IF_OPEN
#endif //BR_CLOSE_IF_OPEN 

#ifndef BR_CLOSE_IF_NO_OPEN 
#define BR_CLOSE_IF_NO_OPEN }
#endif //BR_CLOSE_IF_NO_OPEN

#define CONTAC(a, b) a##b

#if defined(__linux__)
#define __forceinline __attribute__((always_inline))
#endif

#include <Typelist.h>

namespace Private
{
    template<int count>
    class TypelistMaker;

    template<>
    class TypelistMaker<__COUNTER__>
    {
    public:
        typedef Loki::NullType typelist;
    };
}
template<bool State, class T1, class T2>
class type_if_else;

template<class T1, class T2>
class type_if_else<true, T1, T2>
{
public:
    typedef T1 value;
};

template<class T1, class T2>
class type_if_else<false, T1, T2>
{
public:
    typedef T2 value;
};

#define ADD_TO_TYPELIST(classname)                                              \
namespace {                                                               \
struct CONTAC(UnusedClass_, classname) {static int REAL_TYPE_ITEM;};   BR_CLOSE_IF_OPEN          \
     int type_if_else<OPEN_STATIC_VARS, classname, CONTAC(UnusedClass_, classname)>::value::REAL_TYPE_ITEM = -1; BR_CLOSE_IF_NO_OPEN \
namespace Private                                                                 \
{                                                                                 \
    const int CONTAC(now_count_, classname) = __COUNTER__;                        \
    template<>                                                                    \
    class TypelistMaker<CONTAC(now_count_, classname)>                            \
    {                                                                             \
    public:                                                                       \
        typedef Loki::Typelist<classname, TypelistMaker<classname :: THIS_COUNTER - 1>::typelist> typelist;\
    };                                                                            \
}


class NotLoadItem {} const nouse;

template<int>
struct FakeParamClass {};

#define DECLARE_SAVED(thisclass, masterclass)  \
        static const int THIS_COUNTER = __COUNTER__;           \
template<int num> __forceinline void KV_SAVE_FUNC(std::stringstream& file, FakeParamClass<num>){};     \
template<int num> __forceinline void KV_LOAD_FUNC(std::stringstream& file, FakeParamClass<num>){};     \
template<int num> __forceinline unsigned int KV_HASH_FUNC(unsigned int hash, FakeParamClass<num>){return hash;};     \
        virtual bool saveSelf(std::stringstream& file)                 \
        {                                                           \
        static_assert(std::is_same<decltype(this), thisclass*>::value && std::is_base_of<masterclass, thisclass>::value, #thisclass " error DECLARE_SAVED");\
            masterclass ::saveSelf(file);                           \
            KV_SAVE_FUNC(file, FakeParamClass<THIS_COUNTER + 1>());    \
            return true;                                            \
        };                                                          \
        virtual bool loadSelf(std::stringstream& file)                 \
        {                                                           \
            masterclass ::loadSelf(file);                           \
            KV_LOAD_FUNC(file, FakeParamClass<THIS_COUNTER + 1>()); \
            return true;                                            \
        };                                                          \
        virtual unsigned int hashSelf()                 \
        {                                                           \
            return                                                  \
            KV_HASH_FUNC(masterclass ::hashSelf(), FakeParamClass<THIS_COUNTER + 1>());\
        }; \
        thisclass (NotLoadItem) : masterclass(nouse) {}

#define KV_SAVEBLE(name) name; struct Z_Impl##name { static const int name##counter = __COUNTER__;}; __forceinline void KV_SAVE_FUNC(std::stringstream& file, FakeParamClass<Z_Impl##name :: name##counter>){file << " "; file << name; file << " "; KV_SAVE_FUNC(file, FakeParamClass<Z_Impl##name :: name##counter + 1>());} __forceinline void KV_LOAD_FUNC(std::stringstream& file, FakeParamClass<Z_Impl##name :: name##counter>){file >> name; KV_LOAD_FUNC(file, FakeParamClass<Z_Impl##name :: name##counter + 1>());} __forceinline unsigned int KV_HASH_FUNC(unsigned int h, FakeParamClass<Z_Impl##name :: name##counter>){return hash(name) + KV_HASH_FUNC(h, FakeParamClass<Z_Impl##name :: name##counter + 1>());};

#define KV_ON_LOAD(name, value) name; struct Z_Impl##name { static const int name##counter = __COUNTER__;}; __forceinline void KV_SAVE_FUNC(std::stringstream& file, FakeParamClass<Z_Impl##name :: name##counter>){KV_SAVE_FUNC(file, FakeParamClass<Z_Impl##name ::name##counter + 1>());} __forceinline void KV_LOAD_FUNC(std::stringstream& file, FakeParamClass<Z_Impl##name ::name##counter>){name = value; KV_LOAD_FUNC(file, FakeParamClass<Z_Impl##name :: name##counter + 1>());} __forceinline unsigned int KV_HASH_FUNC(unsigned int h, FakeParamClass<Z_Impl##name :: name##counter>){return KV_HASH_FUNC(h, FakeParamClass<Z_Impl##name :: name##counter + 1>());};

#define KV_ON_LOAD_CALL(function) struct Z_Impl##function { static const int function##counter = __COUNTER__;}; __forceinline void KV_SAVE_FUNC(std::stringstream& file, FakeParamClass<Z_Impl##function :: function##counter>){KV_SAVE_FUNC(file, FakeParamClass<Z_Impl##function :: function##counter + 1>());} __forceinline void KV_LOAD_FUNC(std::stringstream& file, FakeParamClass<Z_Impl##function ::function##counter>){ function(); KV_LOAD_FUNC(file, FakeParamClass<Z_Impl##function :: function##counter + 1>());} __forceinline unsigned int KV_HASH_FUNC(unsigned int h, FakeParamClass<Z_Impl##function :: function##counter>){return KV_HASH_FUNC(h, FakeParamClass<Z_Impl##function :: function##counter + 1>());};

#define DECLARE_REAL_TYPE_ITEM \
    static int RealType;       \
    virtual int RT_ITEM()      \
    {                          \
        return RealType;       \
    }                          \
    int RT_ITEM_S()            \
    {                          \
        return RealType;       \
    }

#define DECLARE_EASY_ITEM(str)         \
    virtual unsigned int T_ITEM()      \
    {                                  \
        static unsigned int result = hash(str);\
        return result;                 \
    }

#define DECLARE_GET_TYPE_ITEM(str)            \
    virtual unsigned int T_ITEM()             \
    {                                         \
        static unsigned int result = hash(#str);\
        return result;                        \
    }                                         \
    static unsigned int T_ITEM_S()            \
    {                                         \
        static unsigned int result = hash(#str);\
        return result;                        \
    }                                         \
    static int REAL_TYPE_ITEM;                \
    virtual int RT_ITEM()                     \
    {                                         \
        return REAL_TYPE_ITEM;                \
    }                                         \
    static int RT_ITEM_S()                    \
    {                                         \
        return REAL_TYPE_ITEM;                \
    }                                         
#endif //MACROSES_H
