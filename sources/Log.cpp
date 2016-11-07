#include "Log.h"
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QUuid>
#include "KVAbort.h"
#include <QDateTime> 

QFile logs;
QTextStream logstream;

void LogInitializer()
{
    logstream.setDevice(&logs);
    if(!logstream.device())
    {
        qDebug() << "No device was assigned.";
        KvAbort();
    }
    logs.setFileName("/debug_reports/log-"+ QDateTime::currentDateTime().toString() + "-" + QUuid::createUuid().toString() + ".txt");
    if(logs.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << "File cannot be opened.";
        KvAbort();
    }
}

void KvMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QString datetime = QDateTime::currentDateTime().toString();
    QString information = !context.file ? QString("%1: %2").arg(datetime, message) : QString("%1: %2 %3 %4 %5").arg(datetime, context.file, QString(context.line), context.function, message);
    switch (type) {
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

