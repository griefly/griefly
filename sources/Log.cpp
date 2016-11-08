#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QUuid>
#include <QDateTime> 

#include "Log.h"
#include "KVAbort.h"

QFile logs;
QTextStream logstream;

void InitializeLog()
{
    QString datetime = QDateTime::currentDateTime().toString();
    QString uuid = QUuid::createUuid().toString();
    QString filename = QString("debug_reports/log-%1-%2.txt").arg(datetime).arg(uuid);
    logs.setFileName(filename);
    if (logs.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << logs.fileName() << " cannot be opened.";
        KvAbort();
    }
    logstream.setDevice(&logs);
}

void KvMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QString datetime = QDateTime::currentDateTime().toString();
    QString information;
    if(!context.file)
    {
        information = QString("%1: %2").arg(datetime, message);
    }
    else
    {
        information = QString("%1: %2 %3 %4 %5").arg(datetime, context.file, QString(context.line), context.function, message);
    }
    switch (type)
    {
    case QtDebugMsg:
        logstream << information << endl;
        break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    case QtInfoMsg:
        logstream << information << endl;
        break;
#endif // QT_VERSION
    case QtWarningMsg:
        logstream << information << endl;
        break;
    case QtCriticalMsg:
        logstream << information << endl;
        break;
    case QtFatalMsg:
        logstream << information << endl;
        KvAbort();
    }
}
void InstallMessageHandler()
{   
    qInstallMessageHandler(KvMessageHandler);
}

