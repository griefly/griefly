#include <gtest/gtest.h>

#include "core/Idptr.h"

#include "core/objects/Object.h"
#include "core/objects/test/UnsyncGenerator.h"

class TempTable
{
public:
    TempTable()
    {
        old_table_ = id_ptr_id_table;
        id_ptr_id_table = &table_;
        table_.resize(100);
    }
    ~TempTable()
    {
        id_ptr_id_table = old_table_;
    }

private:
    QVector<ObjectInfo> table_;
    QVector<ObjectInfo>* old_table_;
};

TEST(IdPtrDeathTest, Death)
{
    TempTable table;

    ASSERT_DEATH(
    {
        IdPtr<kv::Object> object(101);
        object.IsValid();
    }, "Id table lookup fail");
}

class IdPtrTest : public ::testing::Test
{
private:
    TempTable table_;
};

TEST_F(IdPtrTest, Constructors)
{
    IdPtr<kv::Object> object(40);
    ASSERT_EQ(object.Id(), 40);

    object = 10;
    ASSERT_EQ(object.Id(), 10);

    IdPtr<kv::Object> empty;
    ASSERT_EQ(empty.Id(), 0);

    empty = object;
    ASSERT_EQ(empty.Id(), 10);

    IdPtr<kv::Object> from_other(empty);
    ASSERT_EQ(from_other.Id(), 10);

    IdPtr<kv::UnsyncGenerator> gen_from_other(from_other);
    ASSERT_EQ(from_other.Id(), 10);

    from_other = 33;
    gen_from_other = from_other;
    ASSERT_EQ(from_other.Id(), 33);
}

TEST_F(IdPtrTest, EqualOperator)
{
    IdPtr<kv::Object> object(23);
    ASSERT_TRUE(object == object);
    ASSERT_FALSE(object != object);

    IdPtr<kv::Object> other(22);
    ASSERT_FALSE(object == other);
    ASSERT_FALSE(other == object);
    ASSERT_TRUE(object != other);
    ASSERT_TRUE(other != object);

    IdPtr<kv::Object> object2(23);
    ASSERT_TRUE(object == object2);
    ASSERT_TRUE(object2 == object);
    ASSERT_FALSE(object != object2);
    ASSERT_FALSE(object2 != object);
}

TEST_F(IdPtrTest, Dereference)
{
    kv::Object object;
    (*id_ptr_id_table)[42].object = &object;

    IdPtr<kv::Object> ptr;
    ASSERT_EQ(ptr.operator*(), nullptr);

    ptr = 10;
    ASSERT_EQ(ptr.operator*(), nullptr);

    ptr = 42;
    ASSERT_EQ(ptr.operator*(), &object);
    // Cache
    ASSERT_EQ(ptr.operator*(), &object);

    ASSERT_EQ(ptr.operator*(), ptr.operator->());
}

TEST_F(IdPtrTest, Validating)
{
    kv::Object object;
    (*id_ptr_id_table)[42].object = &object;

    IdPtr<kv::Object> ptr;
    ASSERT_FALSE(ptr.IsValid());
    ASSERT_FALSE(ptr);

    ptr = 10;
    ASSERT_FALSE(ptr.IsValid());

    ptr = 42;
    ASSERT_TRUE(ptr.IsValid());
    ASSERT_TRUE(ptr);

    (*id_ptr_id_table)[42].object = nullptr;
    ASSERT_FALSE(ptr.IsValid());
}

TEST_F(IdPtrTest, SaveAndLoad)
{
    IdPtr<kv::Object> ptr(93);
    FastSerializer serializer(1);
    serializer << ptr;
    EXPECT_EQ(
        QByteArray(serializer.GetData(), serializer.GetIndex()),
        QByteArray("\x03\x5D\x00\x00\x00", 5));

    IdPtr<kv::Object> ptr2;
    EXPECT_FALSE(ptr2 == ptr);

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());

    deserializer >> ptr2;
    EXPECT_TRUE(ptr2 == ptr);
}







