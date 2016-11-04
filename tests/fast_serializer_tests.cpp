#include <gtest/gtest.h>

#include "FastSerializer.h"

#include <QFile>

TEST(FastSerializer, Constructor)
{
    FastSerializer serializer;
    ASSERT_EQ(serializer.GetIndex(), 0);
    ASSERT_TRUE(serializer.GetData());
}

TEST(FastSerializer, WriteBool)
{
    FastSerializer serializer;

    serializer.Write(true);
    ASSERT_EQ(serializer.GetIndex(), 1);
    EXPECT_TRUE(serializer.GetData()[0]);

    serializer.Write(false);
    ASSERT_EQ(serializer.GetIndex(), 2);
    EXPECT_FALSE(serializer.GetData()[1]);
}

TEST(FastSerializer, WriteInt32)
{
    FastSerializer serializer;

    {
        qint32 value = 0;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 4);
        for (int i = 0; i < 4; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        qint32 value = 1;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 8);
        EXPECT_EQ(serializer.GetData()[4], '\x01');
        for (int i = 5; i < 8; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        qint32 value = -1;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 12);
        for (int i = 8; i < 12; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\xFF');
        }
    }

    {
        qint32 value = 2047583047;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 16);
        EXPECT_EQ(serializer.GetData()[12], '\x47');
        EXPECT_EQ(serializer.GetData()[13], '\xA3');
        EXPECT_EQ(serializer.GetData()[14], '\x0B');
        EXPECT_EQ(serializer.GetData()[15], '\x7A');
    }

    {
        qint32 value = -2039583948;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 20);
        EXPECT_EQ(serializer.GetData()[16], '\x34');
        EXPECT_EQ(serializer.GetData()[17], '\x6B');
        EXPECT_EQ(serializer.GetData()[18], '\x6E');
        EXPECT_EQ(serializer.GetData()[19], '\x86');
    }
}

TEST(FastSerializer, WriteUint32)
{
    FastSerializer serializer;

    {
        quint32 value = 0;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 4);
        for (int i = 0; i < 4; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        quint32 value = 1;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 8);
        EXPECT_EQ(serializer.GetData()[4], '\x01');
        for (int i = 5; i < 8; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        quint32 value = 4294967295;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 12);
        for (int i = 8; i < 12; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\xFF');
        }
    }

    {
        quint32 value = 2047583048;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 16);
        EXPECT_EQ(serializer.GetData()[12], '\x48');
        EXPECT_EQ(serializer.GetData()[13], '\xA3');
        EXPECT_EQ(serializer.GetData()[14], '\x0B');
        EXPECT_EQ(serializer.GetData()[15], '\x7A');
    }
}

TEST(FastSerializer, WriteString)
{
    FastSerializer serializer;

    {
        QString value;
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 4);
        for (int i = 0; i < 4; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        QString value("Hello world!");
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 32);
        EXPECT_EQ(serializer.GetData()[4], '\x0C');
        EXPECT_EQ(serializer.GetData()[5], '\x00');
        EXPECT_EQ(serializer.GetData()[6], '\x00');
        EXPECT_EQ(serializer.GetData()[7], '\x00');
        EXPECT_EQ(serializer.GetData()[8], 'H');
        EXPECT_EQ(serializer.GetData()[9], '\x00');
        EXPECT_EQ(serializer.GetData()[10], 'e');
        EXPECT_EQ(serializer.GetData()[11], '\x00');
        EXPECT_EQ(serializer.GetData()[18], ' ');
        EXPECT_EQ(serializer.GetData()[19], '\x00');
        EXPECT_EQ(serializer.GetData()[30], '!');
        EXPECT_EQ(serializer.GetData()[31], '\x00');
    }

    {
        // Cyrillic
        // Some issues with cyrillic text in code, so load it from a file
        QFile file("test/text/cyrillic.txt");
        ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
        QString value = file.readLine();
        serializer.Write(value);
        ASSERT_EQ(serializer.GetIndex(), 60);
        EXPECT_EQ(serializer.GetData()[32], '\x0C');
        EXPECT_EQ(serializer.GetData()[33], '\x00');
        EXPECT_EQ(serializer.GetData()[34], '\x00');
        EXPECT_EQ(serializer.GetData()[35], '\x00');
        EXPECT_EQ(serializer.GetData()[36], '\x1F');
        EXPECT_EQ(serializer.GetData()[37], '\x04');
        EXPECT_EQ(serializer.GetData()[48], ',');
        EXPECT_EQ(serializer.GetData()[49], '\x00');
        EXPECT_EQ(serializer.GetData()[50], ' ');
        EXPECT_EQ(serializer.GetData()[51], '\x00');
        EXPECT_EQ(serializer.GetData()[52], '\x3C');
        EXPECT_EQ(serializer.GetData()[53], '\x04');
        EXPECT_EQ(serializer.GetData()[58], '!');
        EXPECT_EQ(serializer.GetData()[59], '\x00');
    }
}

TEST(FastSerializer, ResetIndex)
{
    FastSerializer serializer;

    ASSERT_EQ(serializer.GetIndex(), 0);
    serializer.Write(42);
    ASSERT_EQ(serializer.GetIndex(), 4);
    serializer.ResetIndex();
    ASSERT_EQ(serializer.GetIndex(), 0);
}

TEST(FastSerializer, BigData)
{
    FastSerializer serializer;

    ASSERT_EQ(serializer.GetIndex(), 0);

    const int BIG_DATA_SIZE = FastSerializer::DEFAULT_SIZE * 2;
    for (int i = 0; i < BIG_DATA_SIZE; ++i)
    {
        serializer.Write(true);
    }
    ASSERT_EQ(serializer.GetIndex(), BIG_DATA_SIZE);
}

