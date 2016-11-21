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

    serializer << true;
    ASSERT_EQ(serializer.GetIndex(), 2);
    EXPECT_EQ(serializer.GetData()[0], '\x01');
    EXPECT_TRUE(serializer.GetData()[1]);

    serializer << false;
    ASSERT_EQ(serializer.GetIndex(), 4);
    EXPECT_EQ(serializer.GetData()[2], '\x01');
    EXPECT_FALSE(serializer.GetData()[3]);
}

TEST(FastSerializer, WriteInt32)
{
    FastSerializer serializer;

    {
        qint32 value = 0;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 5);
        EXPECT_EQ(serializer.GetData()[0], '\x02');
        for (int i = 0; i < 4; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i + 1], '\x00');
        }
    }

    {
        qint32 value = 1;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 10);
        EXPECT_EQ(serializer.GetData()[5], '\x02');
        EXPECT_EQ(serializer.GetData()[6], '\x01');
        for (int i = 7; i < 10; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        qint32 value = -1;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 15);
        EXPECT_EQ(serializer.GetData()[10], '\x02');
        for (int i = 11; i < 15; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\xFF');
        }
    }

    {
        qint32 value = 2047583047;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 20);
        EXPECT_EQ(serializer.GetData()[15], '\x02');
        EXPECT_EQ(serializer.GetData()[16], '\x47');
        EXPECT_EQ(serializer.GetData()[17], '\xA3');
        EXPECT_EQ(serializer.GetData()[18], '\x0B');
        EXPECT_EQ(serializer.GetData()[19], '\x7A');
    }

    {
        qint32 value = -2039583948;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 25);
        EXPECT_EQ(serializer.GetData()[20], '\x02');
        EXPECT_EQ(serializer.GetData()[21], '\x34');
        EXPECT_EQ(serializer.GetData()[22], '\x6B');
        EXPECT_EQ(serializer.GetData()[23], '\x6E');
        EXPECT_EQ(serializer.GetData()[24], '\x86');
    }
}

TEST(FastSerializer, WriteUint32)
{
    FastSerializer serializer;

    {
        quint32 value = 0;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 5);
        EXPECT_EQ(serializer.GetData()[0], '\x03');
        for (int i = 1; i < 5; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        quint32 value = 1;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 10);
        EXPECT_EQ(serializer.GetData()[5], '\x03');
        EXPECT_EQ(serializer.GetData()[6], '\x01');
        for (int i = 7; i < 10; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        quint32 value = 4294967295;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 15);
        EXPECT_EQ(serializer.GetData()[10], '\x03');
        for (int i = 11; i < 15; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\xFF');
        }
    }

    {
        quint32 value = 2047583048;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 20);
        EXPECT_EQ(serializer.GetData()[15], '\x03');
        EXPECT_EQ(serializer.GetData()[16], '\x48');
        EXPECT_EQ(serializer.GetData()[17], '\xA3');
        EXPECT_EQ(serializer.GetData()[18], '\x0B');
        EXPECT_EQ(serializer.GetData()[19], '\x7A');
    }
}

TEST(FastSerializer, WriteString)
{
    FastSerializer serializer;

    {
        QString value;
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 6);
        EXPECT_EQ(serializer.GetData()[0], '\x04');
        EXPECT_EQ(serializer.GetData()[1], '\x02');
        for (int i = 2; i < 6; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i], '\x00');
        }
    }

    {
        QString value("Hello world!");
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 36);
        EXPECT_EQ(serializer.GetData()[6], '\x04');
        EXPECT_EQ(serializer.GetData()[7], '\x02');
        EXPECT_EQ(serializer.GetData()[8], '\x0C');
        EXPECT_EQ(serializer.GetData()[9], '\x00');
        EXPECT_EQ(serializer.GetData()[10], '\x00');
        EXPECT_EQ(serializer.GetData()[11], '\x00');
        EXPECT_EQ(serializer.GetData()[12], 'H');
        EXPECT_EQ(serializer.GetData()[13], '\x00');
        EXPECT_EQ(serializer.GetData()[14], 'e');
        EXPECT_EQ(serializer.GetData()[15], '\x00');
        EXPECT_EQ(serializer.GetData()[22], ' ');
        EXPECT_EQ(serializer.GetData()[23], '\x00');
        EXPECT_EQ(serializer.GetData()[34], '!');
        EXPECT_EQ(serializer.GetData()[35], '\x00');
    }

    {
        QString value("Привет, мир!");
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 66);
        EXPECT_EQ(serializer.GetData()[36], '\x04');
        EXPECT_EQ(serializer.GetData()[37], '\x02');
        EXPECT_EQ(serializer.GetData()[38], '\x0C');
        EXPECT_EQ(serializer.GetData()[39], '\x00');
        EXPECT_EQ(serializer.GetData()[40], '\x00');
        EXPECT_EQ(serializer.GetData()[41], '\x00');
        EXPECT_EQ(serializer.GetData()[54], ',');
        EXPECT_EQ(serializer.GetData()[55], '\x00');
        EXPECT_EQ(serializer.GetData()[56], ' ');
        EXPECT_EQ(serializer.GetData()[57], '\x00');
        EXPECT_EQ(serializer.GetData()[58], '\x3C');
        EXPECT_EQ(serializer.GetData()[59], '\x04');
        EXPECT_EQ(serializer.GetData()[64], '!');
        EXPECT_EQ(serializer.GetData()[65], '\x00');
    }
}

TEST(FastSerializer, WriteByteArray)
{
    FastSerializer serializer;

    {
        const int DATA_SIZE = 7;
        const char DATA[DATA_SIZE + 1] = "fff\0fff";
        QByteArray value(DATA, DATA_SIZE);
        serializer << value;
        ASSERT_EQ(serializer.GetIndex(), 13);
        EXPECT_EQ(serializer.GetData()[0], '\x05');
        EXPECT_EQ(serializer.GetData()[1], '\x02');
        EXPECT_EQ(serializer.GetData()[2], '\x07');
        EXPECT_EQ(serializer.GetData()[3], '\x00');
        EXPECT_EQ(serializer.GetData()[4], '\x00');
        EXPECT_EQ(serializer.GetData()[5], '\x00');
        for (int i = 0; i < DATA_SIZE; ++i)
        {
            EXPECT_EQ(serializer.GetData()[i + 6], DATA[i]);
        }
    }
}

TEST(FastSerializer, ResetIndex)
{
    FastSerializer serializer;

    ASSERT_EQ(serializer.GetIndex(), 0);
    serializer << 42;
    ASSERT_EQ(serializer.GetIndex(), 5);
    serializer.ResetIndex();
    ASSERT_EQ(serializer.GetIndex(), 0);
}

TEST(FastSerializer, BigData)
{
    FastSerializer serializer;

    ASSERT_EQ(serializer.GetIndex(), 0);

    const int BIG_DATA_SIZE = FastSerializer::DEFAULT_SIZE;
    for (int i = 0; i < BIG_DATA_SIZE; ++i)
    {
        serializer << true;
    }
    ASSERT_EQ(serializer.GetIndex(), BIG_DATA_SIZE * 2);
}

TEST(FastDeserializerDeathTest, EnsureSizeFail)
{
    FastDeserializer deserializer("\x01\x00\x01\x00", 4);
    ASSERT_DEATH(
    {
        bool value;
        deserializer >> value;
        deserializer >> value;
        deserializer >> value;
    }, "FastDeserializer: EnsureSize fail!");
}

TEST(FastDeserializer, Constructor)
{
    {
        FastDeserializer deserializer(nullptr, 0);
        EXPECT_TRUE(deserializer.IsEnd());
    }
    {
        FastDeserializer deserializer(nullptr, 1);
        EXPECT_FALSE(deserializer.IsEnd());
    }
}

TEST(FastDeserializer, ReadBool)
{
    const char* const DATA = "\0134gkfo\0\01";
    const int DATA_SIZE = 11;

    FastDeserializer deserializer(DATA, DATA_SIZE);

    for (int i = 0; i < DATA_SIZE; ++i)
    {
        ASSERT_FALSE(deserializer.IsEnd());
        bool value;
        deserializer >> value;
        if (DATA[i] == '\0')
        {
            EXPECT_FALSE(value);
        }
        else
        {
            EXPECT_TRUE(value);
        }
    }

    ASSERT_TRUE(deserializer.IsEnd());
}

TEST(FastDeserializer, ReadInt32)
{
    const char* const DATA =
        "\x00\x00\x00\x00"
        "\x01\x00\x00\x00"
        "\xFF\x00\x00\x00"
        "\x47\xA3\x0B\x7A"
        "\xFF\xFF\xFF\xFF"
        "\xFF\x1F\xFF\xFF";
    const int DATA_SIZE = 24;

    FastDeserializer deserializer(DATA, DATA_SIZE);
    qint32 value;

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 0);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 1);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 255);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 2047583047);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, -1);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, -57345);

    ASSERT_TRUE(deserializer.IsEnd());
}

TEST(FastDeserializer, ReadUint32)
{
    const char* const DATA =
        "\x00\x00\x00\x00"
        "\x01\x00\x00\x00"
        "\xFF\x00\x00\x00"
        "\x47\xA3\x0B\x7A"
        "\xFF\xFF\xFF\xFF"
        "\xFF\x1F\xFF\xFF";
    const int DATA_SIZE = 24;

    FastDeserializer deserializer(DATA, DATA_SIZE);
    quint32 value;

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 0);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 1);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 255);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 2047583047);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 4294967295);

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, 4294909951);

    ASSERT_TRUE(deserializer.IsEnd());
}

TEST(FastDeserializer, ReadString)
{
    const char* const DATA =
        "\x00\x00\x00\x00"
        "\x03\x00\x00\x00"
        "\x48\x00\x65\x00\x20\x00"
        "\x05\x00\x00\x00"
        "\x1F\x04\x20\x00\x20\x00\x3C\x04\x20\x00";
    const int DATA_SIZE = 28;

    FastDeserializer deserializer(DATA, DATA_SIZE);
    QString value;

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, "");

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, "He ");

    ASSERT_FALSE(deserializer.IsEnd());
    deserializer >> value;
    EXPECT_EQ(value, "П  м ");

    ASSERT_TRUE(deserializer.IsEnd());
}

TEST(FastDeserializer, ReadByteArray)
{
    const char* const DATA = "\x0A\x00\x00\x00\x1F\x04\x20\x00\x20\x00\x3C\x04\x20\x00";
    const int DATA_SIZE = 14;

    FastDeserializer deserializer(DATA, DATA_SIZE);
    ASSERT_FALSE(deserializer.IsEnd());
    QByteArray value;
    deserializer >> value;

    ASSERT_EQ(value.size(), DATA_SIZE - 4);

    for (int i = 0; i < DATA_SIZE - 4; ++i)
    {
        EXPECT_EQ(value[i], DATA[i + 4]);
    }

    ASSERT_TRUE(deserializer.IsEnd());
}

TEST(FastSerializeDeserialize, VariousValues)
{
    {
        FastSerializer serializer;
        serializer << true;
        serializer << false;
        serializer << 42;
        serializer << -14329;
        serializer << QString("One two three");
        serializer << QString("Ехал грека через реку");
        serializer << 4294967294u;

        FastDeserializer deserializer(
            serializer.GetData(),
            serializer.GetIndex());

        qDebug() << serializer.GetIndex();

        {
            bool value;

            deserializer >> value;
            EXPECT_EQ(value, true);
            deserializer >> value;
            EXPECT_EQ(value, false);
        }
        {
            int value;

            deserializer >> value;
            EXPECT_EQ(value, 42);
            deserializer >> value;
            EXPECT_EQ(value, -14329);
        }
        {
            QString value;

            deserializer >> value;
            EXPECT_EQ(value, "One two three");
            deserializer >> value;
            EXPECT_EQ(value, "Ехал грека через реку");
        }
        {
            unsigned int value;

            deserializer >> value;
            EXPECT_EQ(value, 4294967294u);
        }

        ASSERT_TRUE(deserializer.IsEnd());
    }
}
