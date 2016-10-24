#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "representation/Metadata.h"

using ::testing::internal::CaptureStderr;
using ::testing::internal::GetCapturedStderr;
using ::testing::HasSubstr;

TEST(ImageMetadata, Constructor)
{
    ImageMetadata metadata;
    ASSERT_FALSE(metadata.Valid());
    ASSERT_FALSE(metadata.IsValidState("state"));
}

TEST(ImageMetadataDeathTest, Death)
{
    ImageMetadata metadata;
    ASSERT_DEATH(
    {
        metadata.Init("icons/test/empty_file.png", 0, 0);
    }, "Fail to read png signature");
}

TEST(ImageMetadata, Init)
{
    ImageMetadata metadata;

    {
        CaptureStderr();
        metadata.Init("I do not exist", 0, 0);
        std::string output = GetCapturedStderr();
        ASSERT_THAT(output, HasSubstr("Fail to open file"));
    }

    ASSERT_FALSE(metadata.Valid());

    {
        CaptureStderr();
        metadata.Init("icons/test/test1.dmi", 0, 0);
        std::string output = GetCapturedStderr();
        ASSERT_THAT(output, HasSubstr("Begin to init metadata for"));
        ASSERT_THAT(output, HasSubstr("Read width:  32"));
        ASSERT_THAT(output, HasSubstr("state1"));
        ASSERT_THAT(output, HasSubstr("Begin make sequence"));
    }

    ASSERT_TRUE(metadata.Valid());
    ASSERT_EQ(metadata.GetW(), 32);
    ASSERT_EQ(metadata.GetH(), 32);
    ASSERT_TRUE(metadata.IsValidState("state2"));
    ASSERT_FALSE(metadata.IsValidState("nonvalidstate"));

    const ImageMetadata::SpriteMetadata& sprite
        = metadata.GetSpriteMetadata("state2");
    ASSERT_EQ(sprite.dirs, 4);
    ASSERT_EQ(sprite.rewind, false);
    ASSERT_EQ(sprite.loop, -1);
    for (int i = 0; i < 3; ++i)
    {
        ASSERT_EQ(sprite.hotspot[i], -1);
    }
    ASSERT_EQ(sprite.first_frame_pos, 1);
    ASSERT_EQ(sprite.frames_data.size(), 1);
    ASSERT_EQ(sprite.frames_data[0].delay, 0);
    ASSERT_EQ(sprite.frames_sequence.size(), 1);
    ASSERT_EQ(sprite.frames_sequence[0], 0);
}

TEST(ImageMetadata, InitWithoutMetadata)
{
    {
        ImageMetadata metadata;

        CaptureStderr();
        metadata.Init("icons/test/no_metadata.png", 0, 0);
        std::string output = GetCapturedStderr();
        ASSERT_THAT(output, HasSubstr("Fail metadata load, try without it"));

        ASSERT_TRUE(metadata.Valid());
        ASSERT_TRUE(metadata.IsValidState(""));

        const ImageMetadata::SpriteMetadata& sprite
            = metadata.GetSpriteMetadata("");
        ASSERT_EQ(sprite.dirs, 1);
        ASSERT_EQ(sprite.first_frame_pos, 0);
        ASSERT_EQ(sprite.frames_data.size(), 1);
        ASSERT_EQ(sprite.frames_data[0].delay, 0);
        ASSERT_EQ(sprite.frames_sequence.size(), 1);
        ASSERT_EQ(sprite.frames_sequence[0], 0);
    }
    {
        ImageMetadata metadata;

        CaptureStderr();
        metadata.Init("icons/test/not_png.jpg", 0, 0);
        std::string output = GetCapturedStderr();
        ASSERT_THAT(output, HasSubstr("Fail metadata load, try without it"));
        ASSERT_THAT(output, HasSubstr("Data is not valid PNG-data"));

        ASSERT_TRUE(metadata.Valid());
        ASSERT_TRUE(metadata.IsValidState(""));

        const ImageMetadata::SpriteMetadata& sprite
            = metadata.GetSpriteMetadata("");
        ASSERT_EQ(sprite.dirs, 1);
        ASSERT_EQ(sprite.first_frame_pos, 0);
        ASSERT_EQ(sprite.frames_data.size(), 1);
        ASSERT_EQ(sprite.frames_data[0].delay, 0);
        ASSERT_EQ(sprite.frames_sequence.size(), 1);
        ASSERT_EQ(sprite.frames_sequence[0], 0);
    }
}

