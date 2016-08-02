#include <gtest/gtest.h>

#include "core/Idptr.h"

#include "core/objects/MainObject.h"
#include "core/objects/UnsyncGenerator.h"

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
    std::vector<ObjectInfo> table_;
    std::vector<ObjectInfo>* old_table_;
};

TEST(IdPtrDeathTest, Death)
{
    TempTable table;

    ASSERT_DEATH(
    {
        IdPtr<IMainObject> object(101);
        object.IsValid();
    }, "Id table lookup fail");
}

TEST(IdPtr, Constructors)
{
    TempTable table;

    IdPtr<IMainObject> object(40);
    ASSERT_EQ(object.Id(), 40);

    object = 10;
    ASSERT_EQ(object.Id(), 10);

    IdPtr<IMainObject> empty;
    ASSERT_EQ(empty.Id(), 0);

    empty = object;
    ASSERT_EQ(empty.Id(), 10);

    IdPtr<IMainObject> from_other(empty);
    ASSERT_EQ(from_other.Id(), 10);

    IdPtr<UnsyncGenerator> gen_from_other(from_other);
    ASSERT_EQ(from_other.Id(), 10);

    from_other = 33;
    gen_from_other = from_other;
    ASSERT_EQ(from_other.Id(), 33);
}

TEST(IdPtr, EqualOperator)
{
    TempTable table;

    IdPtr<IMainObject> object(23);
    ASSERT_TRUE(object == object);

    IdPtr<IMainObject> other(22);
    ASSERT_FALSE(object == other);
    ASSERT_FALSE(other == object);

    IdPtr<IMainObject> object2(23);
    ASSERT_TRUE(object == object2);
    ASSERT_TRUE(object2 == object);
}

TEST(IdPtr, Dereference)
{
    TempTable table;

    IMainObject object(42);
    (*id_ptr_id_table)[42].object = &object;

    IdPtr<IMainObject> ptr;
    ASSERT_EQ(ptr.operator*(), nullptr);

    ptr = 10;
    ASSERT_EQ(ptr.operator*(), nullptr);

    ptr = 42;
    ASSERT_EQ(ptr.operator*(), &object);
    // Cache
    ASSERT_EQ(ptr.operator*(), &object);

    ASSERT_EQ(ptr.operator*(), ptr.operator->());
}







