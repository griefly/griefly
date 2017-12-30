#include "JsonValidator.h"

#include <gtest/gtest.h>

TEST(JsonValidator, ValidateKey)
{
    QJsonObject object;
    object.insert("key1", QString("string"));
    object.insert("key2", 42);
    object.insert("key3", true);

    {
        EXPECT_TRUE(ValidateKey<QString>(object, "key1"));
        EXPECT_FALSE(ValidateKey<bool>(object, "key1"));
        QString value;
        EXPECT_TRUE(ValidateKey(object, "key1", &value));
        EXPECT_EQ(value, "string");
    }
    {
        EXPECT_TRUE(ValidateKey<double>(object, "key2"));
        EXPECT_FALSE(ValidateKey<QString>(object, "key2"));
        double value;
        EXPECT_TRUE(ValidateKey(object, "key2", &value));
        EXPECT_EQ(value, 42.0);
    }
    {
        EXPECT_TRUE(ValidateKey<bool>(object, "key3"));
        EXPECT_FALSE(ValidateKey<QJsonArray>(object, "key3"));
        int value = 789;
        EXPECT_FALSE(ValidateKey(object, "key3", &value));
        EXPECT_EQ(value, 789);
    }
    {
        EXPECT_FALSE(ValidateKey<bool>(object, "does not exist"));
        QString value;
        EXPECT_FALSE(ValidateKey(object, "does not exist", &value));
    }
}

TEST(JsonValidator, ValidateValueString)
{
    QJsonValue value(QString("string"));

    EXPECT_FALSE(ValidateValue<bool>(value));
    EXPECT_FALSE(ValidateValue<int>(value));
    EXPECT_FALSE(ValidateValue<double>(value));
    EXPECT_TRUE(ValidateValue<QString>(value));
    EXPECT_FALSE(ValidateValue<QJsonObject>(value));
    EXPECT_FALSE(ValidateValue<QJsonArray>(value));

    bool bool_value;
    int int_value;
    double double_value;
    QString string_value;
    QJsonObject object_value;
    QJsonArray array_value;

    EXPECT_FALSE(ValidateValue(value, &bool_value));
    EXPECT_FALSE(ValidateValue(value, &int_value));
    EXPECT_FALSE(ValidateValue(value, &double_value));
    EXPECT_TRUE(ValidateValue(value, &string_value));
    EXPECT_EQ(string_value, "string");
    EXPECT_FALSE(ValidateValue(value, &object_value));
    EXPECT_FALSE(ValidateValue(value, &array_value));
}

TEST(JsonValidator, ValidateValueObject)
{
    QJsonObject object;
    object.insert("test", 42);
    QJsonValue value(object);

    EXPECT_FALSE(ValidateValue<bool>(value));
    EXPECT_FALSE(ValidateValue<int>(value));
    EXPECT_FALSE(ValidateValue<double>(value));
    EXPECT_FALSE(ValidateValue<QString>(value));
    EXPECT_TRUE(ValidateValue<QJsonObject>(value));
    EXPECT_FALSE(ValidateValue<QJsonArray>(value));

    bool bool_value;
    int int_value;
    double double_value;
    QString string_value;
    QJsonObject object_value;
    QJsonArray array_value;

    EXPECT_FALSE(ValidateValue(value, &bool_value));
    EXPECT_FALSE(ValidateValue(value, &int_value));
    EXPECT_FALSE(ValidateValue(value, &double_value));
    EXPECT_FALSE(ValidateValue(value, &string_value));
    EXPECT_TRUE(ValidateValue(value, &object_value));
    EXPECT_EQ(object_value, object);
    EXPECT_FALSE(ValidateValue(value, &array_value));
}

TEST(JsonValidator, ValidateValueArray)
{
    QJsonArray array;
    array.append(1);
    array.append(2);
    array.append(3);
    QJsonValue value(array);

    EXPECT_FALSE(ValidateValue<bool>(value));
    EXPECT_FALSE(ValidateValue<int>(value));
    EXPECT_FALSE(ValidateValue<double>(value));
    EXPECT_FALSE(ValidateValue<QString>(value));
    EXPECT_FALSE(ValidateValue<QJsonObject>(value));
    EXPECT_TRUE(ValidateValue<QJsonArray>(value));

    bool bool_value;
    int int_value;
    double double_value;
    QString string_value;
    QJsonObject object_value;
    QJsonArray array_value;

    EXPECT_FALSE(ValidateValue(value, &bool_value));
    EXPECT_FALSE(ValidateValue(value, &int_value));
    EXPECT_FALSE(ValidateValue(value, &double_value));
    EXPECT_FALSE(ValidateValue(value, &string_value));
    EXPECT_FALSE(ValidateValue(value, &object_value));
    EXPECT_TRUE(ValidateValue(value, &array_value));
    EXPECT_EQ(array_value, array);
}

TEST(JsonValidator, ValidateValueBool)
{
    QJsonValue value(true);

    EXPECT_TRUE(ValidateValue<bool>(value));
    EXPECT_FALSE(ValidateValue<int>(value));
    EXPECT_FALSE(ValidateValue<double>(value));
    EXPECT_FALSE(ValidateValue<QString>(value));
    EXPECT_FALSE(ValidateValue<QJsonObject>(value));
    EXPECT_FALSE(ValidateValue<QJsonArray>(value));

    bool bool_value;
    int int_value;
    double double_value;
    QString string_value;
    QJsonObject object_value;
    QJsonArray array_value;

    EXPECT_TRUE(ValidateValue(value, &bool_value));
    EXPECT_EQ(bool_value, true);
    EXPECT_FALSE(ValidateValue(value, &int_value));
    EXPECT_FALSE(ValidateValue(value, &double_value));
    EXPECT_FALSE(ValidateValue(value, &string_value));
    EXPECT_FALSE(ValidateValue(value, &object_value));
    EXPECT_FALSE(ValidateValue(value, &array_value));
}

TEST(JsonValidator, ValidateValueDouble)
{
    QJsonValue value(42.43289);

    EXPECT_FALSE(ValidateValue<bool>(value));
    EXPECT_FALSE(ValidateValue<int>(value));
    EXPECT_TRUE(ValidateValue<double>(value));
    EXPECT_FALSE(ValidateValue<QString>(value));
    EXPECT_FALSE(ValidateValue<QJsonObject>(value));
    EXPECT_FALSE(ValidateValue<QJsonArray>(value));

    bool bool_value;
    int int_value;
    double double_value;
    QString string_value;
    QJsonObject object_value;
    QJsonArray array_value;

    EXPECT_FALSE(ValidateValue(value, &bool_value));
    EXPECT_FALSE(ValidateValue(value, &int_value));
    EXPECT_TRUE(ValidateValue(value, &double_value));
    EXPECT_EQ(double_value, 42.43289);
    EXPECT_FALSE(ValidateValue(value, &string_value));
    EXPECT_FALSE(ValidateValue(value, &object_value));
    EXPECT_FALSE(ValidateValue(value, &array_value));
}

TEST(JsonValidator, ValidateValueInt)
{
    {
        QJsonValue value(777);

        EXPECT_FALSE(ValidateValue<bool>(value));
        EXPECT_TRUE(ValidateValue<int>(value));
        EXPECT_TRUE(ValidateValue<double>(value));
        EXPECT_FALSE(ValidateValue<QString>(value));
        EXPECT_FALSE(ValidateValue<QJsonObject>(value));
        EXPECT_FALSE(ValidateValue<QJsonArray>(value));

        bool bool_value;
        int int_value;
        double double_value;
        QString string_value;
        QJsonObject object_value;
        QJsonArray array_value;

        EXPECT_FALSE(ValidateValue(value, &bool_value));
        EXPECT_TRUE(ValidateValue(value, &int_value));
        EXPECT_EQ(int_value, 777);
        EXPECT_TRUE(ValidateValue(value, &double_value));
        EXPECT_EQ(double_value, 777.0);
        EXPECT_FALSE(ValidateValue(value, &string_value));
        EXPECT_FALSE(ValidateValue(value, &object_value));
        EXPECT_FALSE(ValidateValue(value, &array_value));
    }
    {
        QJsonValue value(777.000000);
        EXPECT_TRUE(ValidateValue<int>(value));
        EXPECT_TRUE(ValidateValue<double>(value));
    }
    {
        QJsonValue value(1e15);
        EXPECT_FALSE(ValidateValue<int>(value));
        EXPECT_TRUE(ValidateValue<double>(value));
    }
    {
        QJsonValue value(-1e15);
        EXPECT_FALSE(ValidateValue<int>(value));
        EXPECT_TRUE(ValidateValue<double>(value));
    }
    {
        QJsonValue value(static_cast<double>(std::numeric_limits<int>::max()));
        EXPECT_TRUE(ValidateValue<int>(value));
        EXPECT_TRUE(ValidateValue<double>(value));
    }
    {
        QJsonValue value(static_cast<double>(std::numeric_limits<int>::min()));
        EXPECT_TRUE(ValidateValue<int>(value));
        EXPECT_TRUE(ValidateValue<double>(value));
    }
    {
        QJsonValue value(static_cast<double>(std::numeric_limits<int>::max()) + 1.0);
        EXPECT_FALSE(ValidateValue<int>(value));
        EXPECT_TRUE(ValidateValue<double>(value));
    }
    {
        QJsonValue value(static_cast<double>(std::numeric_limits<int>::min()) - 1.0);
        EXPECT_FALSE(ValidateValue<int>(value));
        EXPECT_TRUE(ValidateValue<double>(value));
    }
}
