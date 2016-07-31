#include <gtest/gtest.h>

#include "core/Idptr.h"

#include"core/objects/MainObject.h"

class TempTable
{
public:
    TempTable()
    {
        old_table_ = id_ptr_id_table;
        id_ptr_id_table = &table_;
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
        IdPtr<IMainObject> object(42);
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
}
