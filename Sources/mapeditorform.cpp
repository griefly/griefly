#include "mapeditorform.h"
#include "ui_mapeditorform.h"

#include "ItemFabric.h"
#include "MapClass.h"
#include "Text.h"

#include <map>
#include <set>

#include <QBitmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QInputDialog>

#include "AutogenMetadata.h"
#include "StreamWrapper.h"

GraphicsScene::GraphicsScene(QWidget *parent)
{

}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    emit mousePressed(mouseEvent);
}

MapEditorForm::MapEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);

    InitSettersForTypes();

    scene_ = new GraphicsScene;
    map_editor_ = new MapEditor(scene_);

    connect(scene_, &GraphicsScene::mousePressed, map_editor_, &MapEditor::mousePressedEvent);
    connect(map_editor_, &MapEditor::newSelectionSetted, this, &MapEditorForm::newSelectionSetted);

    map_editor_->Resize(100, 100, 1);

    map_editor_->SetPointer(2, 2);

    ui->graphicsView->setScene(scene_);

    SetSpriter(new ASprClass);

    for (auto it = (*itemList()).begin(); it != (*itemList()).end(); ++it)
    {
        IMainObject* loc = it->second(0);
        IOnMapObject* bloc = castTo<IOnMapObject>(loc);
        if (!bloc)
        {
            //delete loc;
            continue;
        }
        bool is_turf = false;
        if (castTo<ITurf>(loc))
        {
            is_turf = true;
        }

        if (bloc->GetMetadata() == nullptr)
        {
            continue;
        }

        int current_frame_pos = bloc->GetMetadata()->first_frame_pos;

        int image_state_h_ = current_frame_pos / bloc->GetSprite()->FrameW();
        int image_state_w_ = current_frame_pos % bloc->GetSprite()->FrameW();

        SDL_Surface* s = bloc->GetSprite()->GetSDLSprite()->frames
                [image_state_w_ * bloc->GetSprite()->FrameH() + image_state_h_];
        QImage img(static_cast<uchar*>(s->pixels),
                               s->w, s->h, QImage::Format_ARGB32);

        map_editor_->AddItemType(it->first, QPixmap::fromImage(img));

        QListWidgetItem* new_item
                = new QListWidgetItem(QIcon(QPixmap::fromImage(img)), bloc->T_ITEM().c_str());

        if (!is_turf)
        {
            types_.push_back(it->first);
            ui->listWidget->addItem(new_item);
        }
        else
        {
            turf_types_.push_back(it->first);
            map_editor_->AddTurfType(it->first);
            ui->listWidgetTurf->addItem(new_item);
        }

    }
}

MapEditorForm::~MapEditorForm()
{
    delete ui;
    delete scene_;
    delete map_editor_;
}

void MapEditorForm::newSelectionSetted(int first_x, int first_y, int second_x, int second_y)
{
    ui->cursor_label->setText(
        "("
        + QString::number(first_x) + ", "
        + QString::number(first_y)
        + ")");

    auto entries = map_editor_->GetEntriesFor(first_x, first_y, 0);

    ui->listWidgetTile->clear();
    for (auto it = entries.begin(); it != entries.end(); ++it)
    {
        ui->listWidgetTile->addItem(it->item_type.c_str());
    }

    ui->listWidgetVariables->clear();
    ui->lineEditAsString->clear();
    ui->lineEditRaw->clear();
}

void MapEditorForm::on_createItem_clicked()
{
    int current_row = ui->listWidget->currentRow();
    if (current_row < 0)
    {
        return;
    }
    std::string type = types_[current_row];
    map_editor_->AddItem(type);
}

void MapEditorForm::on_createTurf_clicked()
{
    int current_row = ui->listWidgetTurf->currentRow();
    if (current_row < 0)
    {
        return;
    }
    std::string type = turf_types_[current_row];
    map_editor_->SetTurf(type);
}

void MapEditorForm::on_beginSelection_clicked()
{
    map_editor_->SetSelectionStage(1);
}

void MapEditorForm::on_removeItem_clicked()
{
    map_editor_->RemoveItems();
}

void MapEditorForm::on_newMap_clicked()
{
    bool ok = false;

    int size_x = ui->posxEdit->text().toInt(&ok);
    if (!ok || (size_x <= 0))
    {
        return;
    }
    int size_y = ui->posyEdit->text().toInt(&ok);
    if (!ok || (size_y <= 0))
    {
        return;
    }
    int size_z = ui->poszEdit->text().toInt(&ok);
    if (!ok || (size_z <= 0))
    {
        return;
    }
    map_editor_->ClearMap();
    map_editor_->Resize(size_x, size_y, size_z);
}

void MapEditorForm::on_saveMap_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Mapgen files (*.gen)"));
    QStringList file_names;
    if (!dialog.exec())
    {
        return;
    }

    file_names = dialog.selectedFiles();
    map_editor_->SaveMapgen(file_names[0].toStdString());
}

void MapEditorForm::on_loadMap_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Mapgen files (*.gen)"));
    QStringList file_names;
    if (!dialog.exec())
    {
        return;
    }

    file_names = dialog.selectedFiles();
    map_editor_->LoadMapgen(file_names[0].toStdString());
}

void MapEditorForm::on_listWidgetTile_itemSelectionChanged()
{
    ui->listWidgetVariables->clear();
    if (ui->listWidgetTile->selectedItems().size() == 0)
    {
        return;
    }
    QListWidgetItem* item = ui->listWidgetTile->selectedItems().first();

    auto& variables = get_setters_for_types()[item->text().toStdString()];

    for (auto it = variables.begin(); it != variables.end(); ++it)
    {
        ui->listWidgetVariables->addItem(it->first.c_str());
    }

    MapEditor::EditorEntry* ee = GetCurrentEditorEntry();
    if (ee)
    {
        UpdateVariablesColor(*ee);
    }
}

/*void MapEditorForm::on_listWidgetVariables_itemDoubleClicked(QListWidgetItem *item)
{
    MapEditor::EditorEntry& ee = GetCurrentEditorEntry();

    std::string& variable_value = ee.variables[item->text().toStdString()];

    bool ok = false;

    QString result =
            QInputDialog::getText(
                nullptr, "Text Input", "New variable value:", QLineEdit::Normal, variable_value.c_str(), &ok);

    if (ok)
    {
        variable_value = result.toStdString();
    }
    UpdateVariablesColor(ee);
}*/

MapEditor::EditorEntry* MapEditorForm::GetCurrentEditorEntry()
{
    int current_index = ui->listWidgetTile->currentRow();

    int current_x = map_editor_->GetPointer().first_posx;
    int current_y = map_editor_->GetPointer().first_posy;
    auto& entries = map_editor_->GetEntriesFor(current_x, current_y, 0);

    if (entries.size())
    {
        return &entries[current_index];
    }
    return nullptr;
}

void MapEditorForm::UpdateVariablesColor(MapEditor::EditorEntry& ee)
{
    for (int i = 0; i < ui->listWidgetVariables->count(); ++i)
    {
        if (ee.variables[ui->listWidgetVariables->item(i)->text().toStdString()].size())
        {
            ui->listWidgetVariables->item(i)->setBackgroundColor(QColor(200, 150, 170));
        }
        else
        {
            ui->listWidgetVariables->item(i)->setBackgroundColor(QColor(255, 255, 255));
        }
    }
}

void MapEditorForm::on_listWidgetVariables_itemSelectionChanged()
{
    MapEditor::EditorEntry* ee = GetCurrentEditorEntry();
    if (!ee)
    {
        return;
    }

    std::string& variable_value = ee->variables[ui->listWidgetVariables->currentItem()->text().toStdString()];

    ui->lineEditRaw->setText(variable_value.c_str());

    std::stringstream ss;
    ss << variable_value;

    std::string parsed_value;
    if (WrapReadMessage(ss, parsed_value).fail())
    {
        parsed_value = "PARSING_ERROR";
    }

    ui->lineEditAsString->setText(parsed_value.c_str());
}

void MapEditorForm::on_lineEditRaw_returnPressed()
{
    if (!ui->listWidgetVariables->currentItem())
    {
        return;
    }

    MapEditor::EditorEntry* ee = GetCurrentEditorEntry();
    if (!ee)
    {
        return;
    }

    std::string& variable_value = ee->variables[ui->listWidgetVariables->currentItem()->text().toStdString()];

    variable_value = ui->lineEditRaw->text().toStdString();

    on_listWidgetVariables_itemSelectionChanged();
    UpdateVariablesColor(*ee);
}

void MapEditorForm::on_lineEditAsString_returnPressed()
{
    if (!ui->listWidgetVariables->currentItem())
    {
        return;
    }

    MapEditor::EditorEntry* ee = GetCurrentEditorEntry();
    if (!ee)
    {
        return;
    }

    std::string& variable_value = ee->variables[ui->listWidgetVariables->currentItem()->text().toStdString()];

    std::stringstream ss;
    std::string loc = ui->lineEditAsString->text().toStdString();
    WrapWriteMessage(ss, loc);

    variable_value = ss.str();

    on_listWidgetVariables_itemSelectionChanged();
    UpdateVariablesColor(*ee);
}
