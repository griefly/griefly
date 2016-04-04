#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QStyleFactory>

#include "representation/qt/mainform.h"
#include "representation/qt/mapeditorform.h"

#include "net/Network2.h"

#include "Params.h"

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{   
    qRegisterMetaType<Message2>();

    GetParamsHolder().ParseParams(argc, argv);
    QApplication app(argc, argv);

    if (app.arguments().contains("--run-tests"))
    {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

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
