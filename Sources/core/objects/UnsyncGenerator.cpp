#include "UnsyncGenerator.h"

#include <QUuid>

UnsyncGenerator::UnsyncGenerator(size_t id) : IMainObject(id)
{
    unsync_string_ = "default";
}

void UnsyncGenerator::PerformUnsync()
{
    qDebug() << "Unsync will be performed";
    unsync_string_ = QUuid::createUuid().toString().toStdString();
}

void UnsyncGenerator::SetThisAsUnsyncGenerator()
{
    SetUnsyncGenerator(GetId());
}

id_ptr_on<UnsyncGenerator> unsync_generator = 0;

void SetUnsyncGenerator(id_ptr_on<UnsyncGenerator> generator)
{
    unsync_generator = generator;
}

id_ptr_on<UnsyncGenerator> GetUnsyncGenerator()
{
    return unsync_generator;
}
