#pragma once

namespace kv
{
namespace internal
{
    class NoInitialization {} const no_initialization;
}
}

#define DECLARE_SAVEABLE(ThisClass, BaseClass)                             \
    friend void ::InitSettersForTypes();                                   \
    using ThisClassType = ThisClass;                                       \
    static kv::Object* _Z_KV_Creator()                                     \
    {                                                                      \
        return new ThisClassType();                                        \
    }                                                                      \
    static kv::Object* _Z_KV_VoidCreator()                                 \
    {                                                                      \
        return new ThisClassType(kv::internal::no_initialization);         \
    }                                                                      \
    void _Z_KV_Checker()                                                   \
    {                                                                      \
        static_assert(                                                     \
            std::is_same<decltype(this), ThisClassType*>::value,           \
            #ThisClass " and '*this' types mismatch!");                    \
        static_assert(                                                     \
            std::is_base_of<BaseClass, ThisClassType>::value,              \
            #ThisClass " does not have '" #BaseClass "' as base class!");  \
    }                                                                      \
    ThisClass(kv::internal::NoInitialization)                              \
        : BaseClass(kv::internal::no_initialization) { }                   \
    virtual void Save(FastSerializer& file) override;                      \
    virtual void Load(FastDeserializer& file) override;                    \
    virtual unsigned int HashMembers() override

#define REGISTER_CLASS_AS(type_id)         \
    virtual const QString& GetType() const \
    {                                      \
        return GetTypeStatic();            \
    }                                      \
    virtual int GetTypeIndex() const       \
    {                                      \
        return TYPE_INDEX;                 \
    }                                      \
    static int GetTypeIndexStatic()        \
    {                                      \
        return TYPE_INDEX;                 \
    }                                      \
    static const QString& GetTypeStatic(); \
    static int TYPE_INDEX

#define KV_SAVEABLE(name)                                                                   \
    name;                                                                                   \
    inline static void _Z_KV_SETTERS##name(kv::Object* ptr, FastDeserializer& deserializer) \
    {                                                                                       \
        ThisClassType* casted = static_cast<ThisClassType*>(ptr);                           \
        deserializer >> casted->name;                                                       \
    }                                                                                       \
    struct _Z_KV_SuppressAnnoyingExtraSemicolonWarning##name {}

#define KV_ON_LOAD_CALL(function) \
    struct _Z_KV_SuppressAnnoyingExtraSemicolonWarning##function {}

#define END_DECLARE(ThisClass)
