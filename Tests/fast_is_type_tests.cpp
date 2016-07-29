#include <gtest/gtest.h>

#include "AutogenMetadata.h"

#include "core/FastIsType.h"

#include "core/objects/MainObject.h"
#include "core/objects/UnsyncGenerator.h"
#include "core/objects/DrawInt.h"

TEST(FastIsTypeTest, TwoArgs)
{
    int main_type = IMainObject::RT_ITEM_S();
    int unsync_type = UnsyncGenerator::RT_ITEM_S();
    int draw_type = IDraw::RT_ITEM_S();

    std::cout << main_type << "," << unsync_type << "," << draw_type
              << "," << cast_table << std::endl;

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
