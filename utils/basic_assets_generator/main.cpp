#include <QCoreApplication>

#include <CoreInterface.h>

#include <QDebug>
#include <QJsonObject>

namespace key
{

const QString TYPENAME("typename");
const QString SPRITE("sprite");
const QString SPRITE_STATE("sprite_state");

} // namespace key

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto& metadata = kv::GetCoreInstance().GetObjectsMetadata();
    for (const auto& object_metadata : metadata)
    {
        const QJsonObject asset
            {{key::TYPENAME, object_metadata.name},
             {key::SPRITE, object_metadata.default_view.base_frameset.sprite_name},
             {key::SPRITE_STATE, object_metadata.default_view.base_frameset.state}};
        qDebug() << asset;
    }

    return 0;
}
