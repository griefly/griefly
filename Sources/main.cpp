#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QtDebug>
#include <QFile>
#include <QTextStream>

#include "representation/qt/mainform.h"
#include "representation/qt/mapeditorform.h"

#include "net/Network2.h"

#include "Params.h"

#ifdef _BUILD_TESTS
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#endif

#include "core/FastIsType.h"
#include "AutogenMetadata.h"

QFile logs("error.log");

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream logstream(&logs);
    QString datetime = QDateTime::currentDateTime().toString();
    switch (type) {
    case QtDebugMsg:
        logstream << "Debug, " << datetime << ": " << msg << " " << context.file << " " << context.line << " " << context.function << endl;
        break;
    case QtWarningMsg:
        logstream << "Warning, " << datetime << ": " << msg << " " << context.file << " " << context.line << " " << context.function << endl;
        break;
    case QtCriticalMsg:
        logstream << "Critical, " << datetime << ": " << msg << " " << context.file << " " << context.line << " " << context.function << endl;
        break;
    case QtFatalMsg:
        logstream << "Fatal, " << datetime << ": " << msg << " " << context.file << " " << context.line << " " << context.function << endl;
        abort();
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    case QtInfoMsg:
        logstream << "Info, "  << datetime << ": " << msg << " " << context.file << " " << context.line << " " << context.function << endl;
        break;
#endif // QT_VERSION
	}
}

int main(int argc, char *argv[])
{
    // LCOV_EXCL_START
    qRegisterMetaType<Message2>();

    GetParamsHolder().ParseParams(argc, argv);
    QApplication app(argc, argv);

    InitRealTypes();
    InitCastTable();
    // LCOV_EXCL_STOP
#ifdef _BUILD_TESTS
    if (app.arguments().contains("--run-tests"))
    {
        ::testing::InitGoogleMock(&argc, argv);
        // Supressing deathtests thread warning
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        return RUN_ALL_TESTS();
    }
#else
    if (app.arguments().contains("--run-tests"))
    {
        std::cout << "This build is without tests!";
        exit(-42);
    }
#endif
    // LCOV_EXCL_START
    app.setStyle(QStyleFactory::create("fusion"));
    if(GetParamsHolder().GetParamBool("-output_redirect"))
    {
        logs.open(QIODevice::WriteOnly | QIODevice::Append);
        qInstallMessageHandler(myMessageOutput);
    }
    if (!GetParamsHolder().GetParamBool("-editor"))
    {
        MainForm main_form;
        main_form.show();
        return app.exec();
    }
    MapEditorForm editor_form;
    editor_form.show();
        
    return app.exec();
    // LCOV_EXCL_STOP
}
