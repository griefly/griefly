#include <QCoreApplication>

#include <CoreInterface.h>
#include <Mapgen.h>

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

namespace key
{

const QString TYPENAME("typename");
const QString SPRITE("sprite");
const QString SPRITE_STATE("sprite_state");
const QString IS_TURF("is_turf");
const QString TYPE("type");
const QString NAME("name");
const QString VARIABLES("variables");

} // namespace key

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto& metadata = kv::GetCoreInstance().GetObjectsMetadata();
    for (const auto& object_metadata : metadata)
    {
        QJsonArray variables;
        for (const auto& variable : object_metadata.variables)
        {
            if (variable.type == mapgen::key::type::UNKNOWN)
            {
                continue;
            }
            variables.append(QJsonObject{{key::NAME, variable.name}, {key::TYPE, variable.type}});
        }

        const QJsonObject asset
            {{key::TYPENAME, object_metadata.name},
             {key::SPRITE, object_metadata.default_view.base_frameset.sprite_name},
             {key::SPRITE_STATE, object_metadata.default_view.base_frameset.state},
             {key::IS_TURF, object_metadata.turf},
             {key::VARIABLES, variables}};
        qDebug() << asset;
    }

    return 0;
}
