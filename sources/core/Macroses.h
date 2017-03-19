#pragma once

#include "StreamWrapper.h"

#define ADD_TO_TYPELIST(classname)                                              \

class NotLoadItem {} const nouse;

template<int>
struct FakeParamClass {};

#define DECLARE_SAVED(thisclass, masterclass)  \
    friend void InitSettersForTypes();\
    typedef thisclass ThisClassType; \
    static IMainObject* _Z_creator(quint32 id) { return new ThisClassType(id); }\
    static IMainObject* _Z_creatorSaved() { return new ThisClassType(nouse);} \
    void _Z_checker()\
    {\
        static_assert(\
               std::is_same<decltype(this), ThisClassType*>::value \
            && std::is_base_of<masterclass, ThisClassType>::value, #thisclass " error DECLARE_SAVED");\
    }\
    virtual void Save(FastSerializer& file) override;                \
    virtual void Load(FastDeserializer& file) override;                 \
    virtual unsigned int Hash() override;                 \
    thisclass (NotLoadItem) : masterclass(nouse) {}

#define KV_SAVEBLE(name) \
    name; \
    inline static void _Z_KV_SETTERS##name (IMainObject* ptr, FastDeserializer& str) \
    { \
        ThisClassType* casted = static_cast<ThisClassType*>(ptr); \
        WrapReadMessage(str, casted->name); \
    }

#define KV_ON_LOAD_CALL(function)


#define DECLARE_GET_TYPE_ITEM(str)         \
    virtual const QString& GetType() const \
    {                                      \
        return GetTypeStatic();            \
    }                                      \
    static const QString& GetTypeStatic()  \
    {                                      \
        static QString type = #str;        \
        return type;                       \
    }                                      \
    static int TYPE_INDEX;                 \
    virtual int GetTypeIndex() const       \
    {                                      \
        return TYPE_INDEX;                 \
    }                                      \
    static int GetTypeIndexStatic()        \
    {                                      \
        return TYPE_INDEX;                 \
    }
