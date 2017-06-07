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
