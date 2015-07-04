#include "mapeditorform.h"
#include "ui_mapeditorform.h"

#include "ItemFabric.h"
#include "MapClass.h"
#include "Text.h"

MapEditorForm::MapEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);

    for (auto it = (*itemList()).begin(); it != (*itemList()).end(); ++it)
    {
        IMainObject* loc = it->second(0);
        IOnMapObject* bloc = castTo<IOnMapObject>(loc);
        if (!bloc)
        {
            delete loc;
            continue;
        }
        ui->listWidget->addItem(bloc->name.c_str());
    }
}

MapEditorForm::~MapEditorForm()
{
    delete ui;
}
