#include <gtest/gtest.h>

#include "AutogenMetadata.h"

#include "core/FastIsType.h"

#include "core/objects/MainObject.h"
#include "core/objects/test/UnsyncGenerator.h"
#include "core/objects/OnMapBase.h"

using namespace kv;

TEST(FastIsType, TwoArgs)
{
    int main_type = Object::GetTypeIndexStatic();
    int unsync_type = UnsyncGenerator::GetTypeIndexStatic();
    int base_type = MapObject::GetTypeIndexStatic();

    ASSERT_TRUE(FastIsType(main_type, main_type));
    ASSERT_TRUE(FastIsType(main_type, unsync_type));
    ASSERT_TRUE(FastIsType(unsync_type, unsync_type));
    ASSERT_TRUE(FastIsType(base_type, base_type));
    ASSERT_TRUE(FastIsType(main_type, base_type));

    ASSERT_FALSE(FastIsType(unsync_type, main_type));
    ASSERT_FALSE(FastIsType(unsync_type, base_type));
    ASSERT_FALSE(FastIsType(base_type, unsync_type));
    ASSERT_FALSE(FastIsType(base_type, main_type));
}

TEST(FastIsType, OneArg)
{
    int main_type = Object::GetTypeIndexStatic();
    int unsync_type = UnsyncGenerator::GetTypeIndexStatic();
    int base_type = MapObject::GetTypeIndexStatic();

    ASSERT_TRUE(FastIsType<Object>(main_type));
    ASSERT_TRUE(FastIsType<Object>(unsync_type));
    ASSERT_TRUE(FastIsType<UnsyncGenerator>(unsync_type));
    ASSERT_TRUE(FastIsType<MapObject>(base_type));
    ASSERT_TRUE(FastIsType<Object>(base_type));

    ASSERT_FALSE(FastIsType<UnsyncGenerator>(main_type));
    ASSERT_FALSE(FastIsType<UnsyncGenerator>(base_type));
    ASSERT_FALSE(FastIsType<MapObject>(unsync_type));
    ASSERT_FALSE(FastIsType<MapObject>(main_type));
}

TEST(FastIsType, CastTo)
{
    kv::Object main_type(kv::internal::no_initialization);
    UnsyncGenerator unsync_type(kv::internal::no_initialization);
    MapObject base_type(kv::internal::no_initialization);

    MapObject* null_type = nullptr;

    ASSERT_FALSE(CastTo<Object>(null_type));

    ASSERT_TRUE(CastTo<Object>(&main_type));
    ASSERT_TRUE(CastTo<Object>(&unsync_type));
    ASSERT_TRUE(CastTo<UnsyncGenerator>(&unsync_type));
    ASSERT_TRUE(CastTo<MapObject>(&base_type));
    ASSERT_TRUE(CastTo<Object>(&base_type));

    ASSERT_FALSE(CastTo<UnsyncGenerator>(&main_type));
    ASSERT_FALSE(CastTo<UnsyncGenerator>(&base_type));
    ASSERT_FALSE(CastTo<MapObject>(&unsync_type));
    ASSERT_FALSE(CastTo<MapObject>(&main_type));
}
