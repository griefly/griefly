#include "mapeditorform.h"
#include "ui_mapeditorform.h"

MapEditorForm::MapEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);
}

MapEditorForm::~MapEditorForm()
{
    delete ui;
}
