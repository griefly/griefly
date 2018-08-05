#include "core_headers/Asset.h"

#include <QJsonArray>

#include "core_headers/Mapgen.h"

namespace
{

namespace key
{

const QString IS_TURF("is_turf");
const QString SPRITE("sprite");
const QString SPRITE_STATE("sprite_state");
const QString ASSET_NAME("asset_name");
const QString TYPENAME("typename");
const QString VARIABLES("variables");
const QString NAME("name");
const QString TYPE("type");
const QString VALUE("value");

}

} // namespace

kv::Asset kv::Asset::FromJson(const QJsonObject& json)
{
    // TODO: proper validation
    Asset asset;
    asset.turf = json[key::IS_TURF].toBool();
    asset.sprite = json[key::SPRITE].toString();
    asset.state = json[key::SPRITE_STATE].toString();
    asset.type_name = json[key::TYPENAME].toString();
    asset.asset_name = json[key::ASSET_NAME].toString();

    const QJsonArray variables = json[key::VARIABLES].toArray();
    for (const QJsonValue& value : variables)
    {
        const QJsonObject object = value.toObject();
        Asset::VariableInfo info;
        info.name = object[key::NAME].toString();
        info.type = object[key::TYPE].toString();
        info.value = object[key::VALUE];
        asset.variables.append(info);
    }
    return asset;
}

QJsonObject kv::Asset::ToJsonForObjectCreation() const
{
    QJsonObject json;
    json.insert(key::IS_TURF, turf);
    json.insert(mapgen::key::SPRITE, sprite);
    json.insert(mapgen::key::STATE, state);
    json.insert(mapgen::key::TYPE, type_name);
    json.insert(key::ASSET_NAME, asset_name);

    QJsonArray array;
    for (auto& variable : variables)
    {
        array.push_back(QJsonObject{
            {key::NAME, variable.name},
            {key::TYPE, variable.type},
            {key::VALUE, variable.value}});
    }
    json.insert(key::VARIABLES, array);

    return json;
}
