#include "launcherform.h"
#include "ui_launcherform.h"

LauncherForm::LauncherForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LauncherForm)
{
    ui->setupUi(this);
}

LauncherForm::~LauncherForm()
{
    delete ui;
}
