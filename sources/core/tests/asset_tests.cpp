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
