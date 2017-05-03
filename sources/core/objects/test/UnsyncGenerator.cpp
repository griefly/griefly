#include "UnsyncGenerator.h"

#include <QUuid>

#include "../../Game.h"

UnsyncGenerator::UnsyncGenerator()
{
    unsync_string_ = "default";
}

void UnsyncGenerator::PerformUnsync()
{
    qDebug() << "Unsync will be performed";
    unsync_string_ = QUuid::createUuid().toString();
}

void UnsyncGenerator::SetThisAsUnsyncGenerator()
{
    GetGame().SetUnsyncGenerator(GetId());
}
