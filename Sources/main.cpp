#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QStyleFactory>

#include "representation/qt/mainform.h"
#include "representation/qt/mapeditorform.h"

#include "net/Network2.h"

#include "Params.h"

#ifdef _BUILD_TESTS
#include <gtest/gtest.h>
#endif

int main(int argc, char *argv[])
{   
    qRegisterMetaType<Message2>();

    GetParamsHolder().ParseParams(argc, argv);
    QApplication app(argc, argv);

#ifdef _BUILD_TESTS
    if (app.arguments().contains("--run-tests"))
    {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
#else
    if (app.arguments().contains("--run-tests"))
    {
        std::cout << "This build is without tests!";
        exit(-42);
    }
#endif
    app.setStyle(QStyleFactory::create("fusion"));

    if (!GetParamsHolder().GetParamBool("-editor"))
    {
        MainForm main_form;
        main_form.show();
        return app.exec();
    }
    MapEditorForm editor_form;
    editor_form.show();
    return app.exec();
}
