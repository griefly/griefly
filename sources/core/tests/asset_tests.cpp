#include <gtest/gtest.h>

#include <QJsonArray>

#include "core_headers/Asset.h"

TEST(Asset, FromJson)
{
    const kv::Asset asset = kv::Asset::FromJson(
        QJsonObject{{"is_turf", true},
                    {"sprite", "sprite"},
                    {"sprite_state", "state"},
                    {"typename", "typename"},
                    {"asset_name", "asset"},
                    {"variables", QJsonArray{
                        QJsonObject{
                            {"name", "name"},
                            {"type", "type"},
                            {"value", QJsonValue("value")}
                            }
                        }}
                    });

    EXPECT_EQ(asset.turf, true);
    EXPECT_EQ(asset.sprite, "sprite");
    EXPECT_EQ(asset.state, "state");
    EXPECT_EQ(asset.type_name, "typename");
    EXPECT_EQ(asset.asset_name, "asset");
    ASSERT_EQ(asset.variables.size(), 1);
    EXPECT_EQ(asset.variables[0].name, "name");
    EXPECT_EQ(asset.variables[0].type, "type");
    EXPECT_EQ(asset.variables[0].value, QJsonValue("value"));
}

TEST(Asset, ToJson)
{
    kv::Asset asset;
    asset.turf = true;
    asset.sprite = "sprite";
    asset.state = "state";
    asset.type_name = "typename";
    asset.asset_name = "asset";
    asset.variables.push_back({"name", "type", QJsonValue("value")});

    const QJsonObject asset_json = asset.ToJsonForObjectCreation();
    const QJsonObject test_json{
        {"is_turf", true},
        {"sprite_name", "sprite"},
        {"state", "state"},
        {"type", "typename"},
        {"asset_name", "asset"},
        {"variables", QJsonArray{
            QJsonObject{
                {"name", "name"},
                {"type", "type"},
                {"value", QJsonValue("value")}
                }
            }}
        };
    EXPECT_EQ(asset_json, test_json);
}
