#include <QApplication>
#include <QStyleFactory>

#include "launcherform.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("fusion"));

    LauncherForm form;
    form.show();

    return app.exec();
}
