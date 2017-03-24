#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

namespace impl
{
    template<class T>
    struct Validator;
}

template<class T>
bool ValidateKey(const QJsonObject& object, const QString& key, T* value_to_set = nullptr)
{
    if (!object.contains(key))
    {
        return false;
    }
    const QJsonValue value = object.value(key);
    return impl::Validator<T>::ValidateValue(value, value_to_set);
}

template<class T>
bool ValidateValue(const QJsonValue& value, T* value_to_set = nullptr)
{
    return impl::Validator<T>::ValidateValue(value, value_to_set);
}

namespace impl
{
    class NonCreatable
    {
        NonCreatable() {}
    };

    template<>
    struct Validator<QString> : public NonCreatable
    {
        static bool ValidateValue(const QJsonValue& value, QString* value_to_set);
    };
    template<>
    struct Validator<double> : public NonCreatable
    {
        static bool ValidateValue(const QJsonValue& value, double* value_to_set);
    };
    template<>
    struct Validator<int> : public NonCreatable
    {
        static bool ValidateValue(const QJsonValue& value, int* value_to_set);
    };
    template<>
    struct Validator<bool> : public NonCreatable
    {
        static bool ValidateValue(const QJsonValue& value, bool* value_to_set);
    };
    template<>
    struct Validator<QJsonObject> : public NonCreatable
    {
        static bool ValidateValue(const QJsonValue& value, QJsonObject* value_to_set);
    };
    template<>
    struct Validator<QJsonArray> : public NonCreatable
    {
        static bool ValidateValue(const QJsonValue& value, QJsonArray* value_to_set);
    };
}
