#include <QCoreApplication>

#include <CoreInterface.h>
#include <Mapgen.h>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
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

    QCommandLineOption assets_directory_option
        ({"d", "directory"}, "Directory where assets will be created", "directory");

    QCommandLineParser parser;
    parser.addOption(assets_directory_option);
    parser.addHelpOption();

    if (!parser.parse(app.arguments()))
    {
        qDebug() << "Unable to parse params!";
        return -1;
    }

    if (!parser.isSet(assets_directory_option))
    {
        qDebug() << "Directory option is missing, try '--help'!";
        return -2;
    }

    const QString assets_directory = parser.value(assets_directory_option).append("/");

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

        const QJsonDocument document(asset);
        const QByteArray data = document.toJson(QJsonDocument::Indented);

        QFile file(assets_directory + object_metadata.name + ".json");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qDebug() << "Unable to open file:" << file.fileName();
            return -3;
        }
        file.write(data);
        qDebug() << file.fileName() << "has been created";
    }

    return 0;
}
