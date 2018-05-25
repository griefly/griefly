#include <QCoreApplication>

#include <CoreInterface.h>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto& metadata = kv::GetCoreInstance().GetObjectsMetadata();

    return 0;
}
