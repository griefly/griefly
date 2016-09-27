#include "Log.h"

QFile logs("log-"+ QDateTime::currentDateTime().toString() + "-" + QUuid::createUuid().toString() + ".txt");
QTextStream logstream(&logs);

void InformationToStream(const QString type, const QMessageLogContext &context, const QString &message, const QString &datetime)
{
    logstream << type << datetime << ": " << message << " " << context.file << " " << context.line << " " << context.function << endl;
}

void KvMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QString datetime = QDateTime::currentDateTime().toString();
    switch (type) {
    case QtDebugMsg:
        InformationToStream("Debug, ", context, message, datetime);
        break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    case QtInfoMsg:
        InformationToStream("Info, ", context, message, datetime);
        break;
#endif // QT_VERSION
    case QtWarningMsg:
        InformationToStream("Warning, ", context, message, datetime);
        break;
    case QtCriticalMsg:
        InformationToStream("Critical, ", context, message, datetime);
        break;
    case QtFatalMsg:
        InformationToStream("Fatal, ", context, message, datetime);
        kv_abort();
	}
}
void InstallMessageHandler()
{
    logs.open(QIODevice::WriteOnly | QIODevice::Append);
    qInstallMessageHandler(KvMessageHandler);
}
