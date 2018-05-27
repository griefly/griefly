#include <QCoreApplication>

#include <CoreInterface.h>

#include <QDebug>
#include <QJsonObject>

namespace key
{

const QString TYPENAME("typename");

} // namespace key

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto& metadata = kv::GetCoreInstance().GetObjectsMetadata();
    for (const auto& object_metadata : metadata)
    {
        const QJsonObject asset
            {{key::TYPENAME, object_metadata.name}};
        qDebug() << asset;
    }

    return 0;
}
