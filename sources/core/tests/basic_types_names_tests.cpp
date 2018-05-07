#include "BasicTypesNames.h"

#include <gtest/gtest.h>

namespace
{

class Foo
{
public:
    bool variable1;
    qint32 variable2;
    quint32 variable3;
    QString variable4;
    QByteArray variable5;
    qint64 variable6;
};

}

TEST(BasicTypesNames, Basics)
{
    EXPECT_EQ(kv::GetTypeName<decltype(Foo::variable1)>(), "bool");
    EXPECT_EQ(kv::GetTypeName<decltype(Foo::variable2)>(), "int32");
    EXPECT_EQ(kv::GetTypeName<decltype(Foo::variable3)>(), "uint32");
    EXPECT_EQ(kv::GetTypeName<decltype(Foo::variable4)>(), "string");
    EXPECT_EQ(kv::GetTypeName<decltype(Foo::variable5)>(), "bytearray");
    EXPECT_EQ(kv::GetTypeName<decltype(Foo::variable6)>(), "int64");
}
