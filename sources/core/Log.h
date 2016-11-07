#pragma once
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QUuid>
#include "KVAbort.h"
#include <QDateTime> 

void InformationToStream(const QString type, const QMessageLogContext &context, const QString &message, const QString &datetime);
void KvMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);
void InstallMessageHandler();
