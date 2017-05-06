#include <gtest/gtest.h>

#include "AutogenMetadata.h"

#include "core/FastIsType.h"

#include "core/objects/MainObject.h"
#include "core/objects/test/UnsyncGenerator.h"
#include "core/objects/DrawInt.h"

TEST(FastIsType, TwoArgs)
{
    int main_type = kv::Object::GetTypeIndexStatic();
    int unsync_type = UnsyncGenerator::GetTypeIndexStatic();
    int draw_type = IDraw::GetTypeIndexStatic();

    ASSERT_TRUE(FastIsType(main_type, main_type));
    ASSERT_TRUE(FastIsType(main_type, unsync_type));
    ASSERT_TRUE(FastIsType(unsync_type, unsync_type));
    ASSERT_TRUE(FastIsType(draw_type, draw_type));
    ASSERT_TRUE(FastIsType(main_type, draw_type));

    ASSERT_FALSE(FastIsType(unsync_type, main_type));
    ASSERT_FALSE(FastIsType(unsync_type, draw_type));
    ASSERT_FALSE(FastIsType(draw_type, unsync_type));
    ASSERT_FALSE(FastIsType(draw_type, main_type));
}

TEST(FastIsType, OneArg)
{
    int main_type = kv::Object::GetTypeIndexStatic();
    int unsync_type = UnsyncGenerator::GetTypeIndexStatic();
    int draw_type = IDraw::GetTypeIndexStatic();

    ASSERT_TRUE(FastIsType<kv::Object>(main_type));
    ASSERT_TRUE(FastIsType<kv::Object>(unsync_type));
    ASSERT_TRUE(FastIsType<UnsyncGenerator>(unsync_type));
    ASSERT_TRUE(FastIsType<IDraw>(draw_type));
    ASSERT_TRUE(FastIsType<kv::Object>(draw_type));

    ASSERT_FALSE(FastIsType<UnsyncGenerator>(main_type));
    ASSERT_FALSE(FastIsType<UnsyncGenerator>(draw_type));
    ASSERT_FALSE(FastIsType<IDraw>(unsync_type));
    ASSERT_FALSE(FastIsType<IDraw>(main_type));
}

TEST(FastIsType, CastTo)
{
    kv::Object main_type(nouse);
    UnsyncGenerator unsync_type(nouse);
    IDraw draw_type(nouse);

    IDraw* null_type = nullptr;

    ASSERT_FALSE(CastTo<kv::Object>(null_type));

    ASSERT_TRUE(CastTo<kv::Object>(&main_type));
    ASSERT_TRUE(CastTo<kv::Object>(&unsync_type));
    ASSERT_TRUE(CastTo<UnsyncGenerator>(&unsync_type));
    ASSERT_TRUE(CastTo<IDraw>(&draw_type));
    ASSERT_TRUE(CastTo<kv::Object>(&draw_type));

    ASSERT_FALSE(CastTo<UnsyncGenerator>(&main_type));
    ASSERT_FALSE(CastTo<UnsyncGenerator>(&draw_type));
    ASSERT_FALSE(CastTo<IDraw>(&unsync_type));
    ASSERT_FALSE(CastTo<IDraw>(&main_type));
}
