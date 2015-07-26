#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QStyleFactory>

#include "mainform.h"
#include "mapeditorform.h"

#include "Mob.h"
#include "Params.h"

int main(int argc, char *argv[])
{   
    GetParamsHolder().ParseParams(argc, argv);
    QApplication app(argc, argv);
    SetQApp(&app);

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
