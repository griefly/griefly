#include "JsonValidator.h"

#include <limits>

#include <cmath>

namespace
{
    template<class TValue, class TChecker, class TGetter>
    bool ValidateValueImpl(
        const QJsonValue& value,
        TValue* value_to_set,
        TChecker checker,
        TGetter getter,
        const TValue& default_value)
    {
        if (!(value.*checker)())
        {
            return false;
        }
        if (value_to_set)
        {
            *value_to_set = (value.*getter)(default_value);
        }
        return true;
    }
}

bool impl::Validator<QString>::ValidateValue(
    const QJsonValue& value, QString* value_to_set)
{
    return ValidateValueImpl(
        value, value_to_set, &QJsonValue::isString,
        static_cast<QString(QJsonValue::*)(const QString&) const>(&QJsonValue::toString),
        QString());
}

bool impl::Validator<double>::ValidateValue(
    const QJsonValue& value, double* value_to_set)
{
    return ValidateValueImpl(
        value, value_to_set, &QJsonValue::isDouble, &QJsonValue::toDouble, 0.0);
}

bool impl::Validator<int>::ValidateValue(
    const QJsonValue& value, int* value_to_set)
{
    double temp_value;
    const bool retval = impl::Validator<double>::ValidateValue(value, &temp_value);
    if (!retval)
    {
        return false;
    }
    if (   temp_value >= std::numeric_limits<int>::max()
        || temp_value <= std::numeric_limits<int>::min())
    {
        return false;
    }

    double dummy;
    if (std::modf(temp_value, &dummy) != 0.0)
    {
        return false;
    }

    *value_to_set = static_cast<int>(temp_value);
    return true;
}

bool impl::Validator<bool>::ValidateValue(
    const QJsonValue& value, bool* value_to_set)
{
    return ValidateValueImpl(
        value, value_to_set, &QJsonValue::isBool, &QJsonValue::toBool, false);
}

bool impl::Validator<QJsonObject>::ValidateValue(
    const QJsonValue& value, QJsonObject* value_to_set)
{
    return ValidateValueImpl(
        value, value_to_set, &QJsonValue::isObject,
        static_cast<QJsonObject(QJsonValue::*)(const QJsonObject&) const>(&QJsonValue::toObject),
        QJsonObject());
}

bool impl::Validator<QJsonArray>::ValidateValue(
    const QJsonValue& value, QJsonArray* value_to_set)
{
    return ValidateValueImpl(
        value, value_to_set, &QJsonValue::isObject,
        static_cast<QJsonArray(QJsonValue::*)(const QJsonArray&) const>(&QJsonValue::toArray),
        QJsonArray());
}
