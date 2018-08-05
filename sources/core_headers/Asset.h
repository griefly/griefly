#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QVector>

namespace kv
{

struct Asset
{
    QString type_name;
    QString asset_name;
    QString sprite;
    QString state;
    bool turf;

    struct VariableInfo
    {
        QString name;
        QString type;
        QJsonValue value;
    };
    QVector<VariableInfo> variables;

    static Asset FromJson(const QJsonObject& json);
    QJsonObject ToJsonForObjectCreation() const;
};

}
