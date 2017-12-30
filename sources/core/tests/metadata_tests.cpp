#include <gtest/gtest.h>
#include <gmock/gmock.h>

// TODO: test in representation
/*#include "representation/Metadata.h"

using ::testing::internal::CaptureStderr;
using ::testing::internal::GetCapturedStderr;
using ::testing::HasSubstr;

TEST(ImageMetadata, Constructor)
{
    ImageMetadata metadata;
    ASSERT_FALSE(metadata.Valid());
    EXPECT_FALSE(metadata.IsValidState("state"));
}

TEST(ImageMetadata, Init)
{
    ImageMetadata metadata;
    {
        CaptureStderr();
        metadata.Init("I do not exist", 0, 0);
        std::string output = GetCapturedStderr();
        EXPECT_THAT(output, HasSubstr("Unable to open metadata file"));
    }

    EXPECT_TRUE(metadata.Valid());

    {
        CaptureStderr();
        metadata.Init("test/icons/test1.dmi", 0, 0);
        std::string output = GetCapturedStderr();
        EXPECT_THAT(output, HasSubstr("Begin to init metadata for"));
        EXPECT_THAT(output, HasSubstr("state1"));
        EXPECT_THAT(output, HasSubstr("Begin make sequence"));
    }

    EXPECT_TRUE(metadata.Valid());
    EXPECT_EQ(metadata.GetW(), 32);
    EXPECT_EQ(metadata.GetH(), 32);
    EXPECT_TRUE(metadata.IsValidState("state with spaces"));
    EXPECT_FALSE(metadata.IsValidState("nonvalidstate"));

    ASSERT_TRUE(metadata.IsValidState("state2"));
    const ImageMetadata::SpriteMetadata& sprite
        = metadata.GetSpriteMetadata("state2");
    EXPECT_EQ(sprite.dirs, 4);
    EXPECT_EQ(sprite.rewind, 0);
    EXPECT_EQ(sprite.loop, -1);
    for (int i = 0; i < 3; ++i)
    {
        ASSERT_EQ(sprite.hotspot[i], -1);
    }
    EXPECT_EQ(sprite.first_frame_pos, 1);
    ASSERT_EQ(sprite.frames_data.size(), 1);
    EXPECT_EQ(sprite.frames_data[0].delay, 0);
    ASSERT_EQ(sprite.frames_sequence.size(), 1);
    EXPECT_EQ(sprite.frames_sequence[0], 0);
}

TEST(ImageMetadata, InitAnimated)
{
    ImageMetadata metadata;
    {
        CaptureStderr();
        metadata.Init("test/icons/test1.dmi", 0, 0);
        std::string output = GetCapturedStderr();
        EXPECT_THAT(output, HasSubstr("Begin to init metadata for"));
        EXPECT_THAT(output, HasSubstr("animated"));
        EXPECT_THAT(output, HasSubstr("Begin make sequence"));
    }

    EXPECT_TRUE(metadata.Valid());
    EXPECT_EQ(metadata.GetW(), 32);
    EXPECT_EQ(metadata.GetH(), 32);
    ASSERT_TRUE(metadata.IsValidState("animated"));

    const ImageMetadata::SpriteMetadata& sprite
        = metadata.GetSpriteMetadata("animated");
    EXPECT_EQ(sprite.dirs, 1);
    EXPECT_EQ(sprite.rewind, 1);
    EXPECT_EQ(sprite.loop, 4);
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_EQ(sprite.hotspot[i], -1);
    }
    EXPECT_EQ(sprite.first_frame_pos, 6);
    ASSERT_EQ(sprite.frames_data.size(), 4);
    EXPECT_EQ(sprite.frames_data[0].delay, 1);
    EXPECT_EQ(sprite.frames_data[1].delay, 3);
    EXPECT_EQ(sprite.frames_data[2].delay, 2);
    EXPECT_EQ(sprite.frames_data[3].delay, 10);
    ASSERT_EQ(sprite.frames_sequence.size(), 25);
    EXPECT_EQ(sprite.frames_sequence[0], 0);
    EXPECT_EQ(sprite.frames_sequence[4], 2);
    EXPECT_EQ(sprite.frames_sequence[24], -1);
    EXPECT_EQ(sprite.frames_sequence[13], 1);
    EXPECT_EQ(sprite.frames_sequence[15], 3);
}

TEST(ImageMetadata, InitHotspot)
{
    ImageMetadata metadata;
    {
        CaptureStderr();
        metadata.Init("test/icons/hotspot.dmi", 0, 0);
        std::string output = GetCapturedStderr();
        EXPECT_THAT(output, HasSubstr("hotspot"));
    }
    EXPECT_TRUE(metadata.Valid());
    EXPECT_EQ(metadata.GetW(), 32);
    EXPECT_EQ(metadata.GetH(), 32);
    ASSERT_TRUE(metadata.IsValidState("hotspot"));

    const ImageMetadata::SpriteMetadata& sprite
        = metadata.GetSpriteMetadata("hotspot");
    EXPECT_EQ(sprite.dirs, 1);
    EXPECT_EQ(sprite.rewind, 0);
    EXPECT_EQ(sprite.loop, -1);
    EXPECT_EQ(sprite.hotspot[0], 13);
    EXPECT_EQ(sprite.hotspot[1], 9);
    EXPECT_EQ(sprite.hotspot[2], 1);
    EXPECT_EQ(sprite.first_frame_pos, 0);
    ASSERT_EQ(sprite.frames_data.size(), 1);
    EXPECT_EQ(sprite.frames_data[0].delay, 0);
    ASSERT_EQ(sprite.frames_sequence.size(), 1);
    EXPECT_EQ(sprite.frames_sequence[0], 0);
}

TEST(ImageMetadata, InitWithoutMetadata)
{
    {
        ImageMetadata metadata;

        CaptureStderr();
        metadata.Init("test/icons/no_metadata.png", 0, 0);
        std::string output = GetCapturedStderr();
        EXPECT_THAT(output, HasSubstr("Unable to open metadata file"));

        EXPECT_TRUE(metadata.Valid());
        EXPECT_TRUE(metadata.IsValidState(""));

        const ImageMetadata::SpriteMetadata& sprite
            = metadata.GetSpriteMetadata("");
        EXPECT_EQ(sprite.dirs, 1);
        EXPECT_EQ(sprite.first_frame_pos, 0);
        ASSERT_EQ(sprite.frames_data.size(), 1);
        EXPECT_EQ(sprite.frames_data[0].delay, 0);
        ASSERT_EQ(sprite.frames_sequence.size(), 1);
        EXPECT_EQ(sprite.frames_sequence[0], 0);
    }
    {
        ImageMetadata metadata;

        CaptureStderr();
        metadata.Init("test/icons/not_png.jpg", 0, 0);
        std::string output = GetCapturedStderr();
        EXPECT_THAT(output, HasSubstr("Unable to open metadata file"));

        EXPECT_TRUE(metadata.Valid());
        EXPECT_TRUE(metadata.IsValidState(""));

        const ImageMetadata::SpriteMetadata& sprite
            = metadata.GetSpriteMetadata("");
        EXPECT_EQ(sprite.dirs, 1);
        EXPECT_EQ(sprite.first_frame_pos, 0);
        ASSERT_EQ(sprite.frames_data.size(), 1);
        EXPECT_EQ(sprite.frames_data[0].delay, 0);
        ASSERT_EQ(sprite.frames_sequence.size(), 1);
        EXPECT_EQ(sprite.frames_sequence[0], 0);
    }
}*/
