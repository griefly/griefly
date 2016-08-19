#include <gtest/gtest.h>

#include "representation/Metadata.h"

TEST(ImageMetadata, Constructor)
{
    ImageMetadata metadata;
    ASSERT_FALSE(metadata.Valid());
    ASSERT_FALSE(metadata.IsValidState("state"));
}

TEST(ImageMetadata, Init)
{
    ImageMetadata metadata;
    metadata.Init("icons/human.dmi", 0, 0);

    ASSERT_TRUE(metadata.Valid());
    ASSERT_EQ(metadata.GetW(), 32);
    ASSERT_EQ(metadata.GetH(), 32);
    ASSERT_TRUE(metadata.IsValidState("asian1_m_s"));
    ASSERT_FALSE(metadata.IsValidState("nonvalidstate"));

    const ImageMetadata::SpriteMetadata& sprite
        = metadata.GetSpriteMetadata("asian1_m_s");
    ASSERT_EQ(sprite.dirs, 4);
    ASSERT_EQ(sprite.rewind, false);
    ASSERT_EQ(sprite.loop, -1);
    for (int i = 0; i < 3; ++i)
    {
        ASSERT_EQ(sprite.hotspot[i], -1);
    }
    ASSERT_EQ(sprite.first_frame_pos, 41);
    ASSERT_EQ(sprite.frames_data.size(), 1);
    ASSERT_EQ(sprite.frames_data[0].delay, 0);
    ASSERT_EQ(sprite.frames_sequence.size(), 1);
    ASSERT_EQ(sprite.frames_sequence[0], 0);
}
