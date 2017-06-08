#include <gtest/gtest.h>

#include "core/Hashes.h"
#include "core/SaveableOperators.h"

TEST(SaveableOperators, MapOperatorsEmpty)
{
    FastSerializer serializer(1);
    QMap<qint32, qint32> map;
    EXPECT_EQ(hash(map), 0);
    serializer << map;

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    QMap<qint32, qint32> other_map;
    deserializer >> other_map;

    EXPECT_TRUE(other_map.isEmpty());
    EXPECT_EQ(hash(other_map), hash(map));
}

TEST(SaveableOperators, MapOperators)
{
    FastSerializer serializer(1);

    QMap<QString, qint32> map;
    quint32 empty_hash = hash(map);

    map.insert("key", 4242);
    map.insert("test", -80);

    quint32 non_empty_hash = hash(map);
    EXPECT_NE(empty_hash, non_empty_hash);
    EXPECT_EQ(non_empty_hash, 687226892);

    serializer << map;

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    QMap<QString, qint32> other_map;
    deserializer >> other_map;

    EXPECT_EQ(other_map, map);
    EXPECT_EQ(hash(other_map), non_empty_hash);
}

TEST(SaveableOperators, PairOperators)
{
    FastSerializer serializer(1);
    QPair<qint32, qint32> pair{11, 12};
    EXPECT_EQ(hash(pair), 23);
    serializer << pair;

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    QPair<qint32, qint32> other_pair;
    deserializer >> other_pair;

    EXPECT_EQ(other_pair.first, 11);
    EXPECT_EQ(other_pair.second, 12);
    EXPECT_EQ(hash(other_pair), hash(pair));
}

TEST(SaveableOperators, VectorOperatorsEmpty)
{
    FastSerializer serializer(1);
    QVector<qint32> vector;
    EXPECT_EQ(hash(vector), 0);
    serializer << vector;

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    QVector<qint32> other_vector;
    deserializer >> other_vector;

    EXPECT_TRUE(other_vector.isEmpty());
    EXPECT_EQ(hash(other_vector), hash(vector));
}

TEST(SaveableOperators, VectorOperators)
{
    FastSerializer serializer(1);
    QVector<qint32> vector{7, 2, 3, 4, 5, 43};
    EXPECT_EQ(hash(vector), 319);
    serializer << vector;

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    QVector<qint32> other_vector;
    deserializer >> other_vector;

    ASSERT_EQ(other_vector.size(), 6);
    EXPECT_EQ(other_vector[0], 7);
    EXPECT_EQ(other_vector[1], 2);
    EXPECT_EQ(other_vector[2], 3);
    EXPECT_EQ(other_vector[3], 4);
    EXPECT_EQ(other_vector[4], 5);
    EXPECT_EQ(other_vector[5], 43);
    EXPECT_EQ(hash(other_vector), hash(vector));
}
