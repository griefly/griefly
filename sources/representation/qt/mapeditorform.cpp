#include "mapeditorform.h"
#include "ui_mapeditorform.h"

#include "core/ObjectFactory.h"
#include "core/objects/turfs/Turf.h"

#include "core_headers/Mapgen.h"

#include "representation/View2.h"
#include "representation/SpriteHolder.h"

#include <map>
#include <set>

#include <QBitmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>

#include "AutogenMetadata.h"

using namespace kv;

GraphicsScene::GraphicsScene(QWidget *parent) : QGraphicsScene(parent)
{
    // Nothing
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    emit mousePressed(mouseEvent);
    if (mouseEvent->button() == Qt::RightButton)
    {
        emit rightClick();
    }
}

void GraphicsScene::keyPressEvent(QKeyEvent *event)
{
    emit keyboardPressed(event);
}

MapEditorForm::MapEditorForm(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);

    is_turf_selected_ = false;

    InitSettersForTypes();

    scene_ = new GraphicsScene;
    map_editor_ = new MapEditor(scene_);

    connect(scene_, &GraphicsScene::mousePressed, map_editor_, &MapEditor::mousePressedEvent);
    connect(scene_, &GraphicsScene::keyboardPressed, map_editor_, &MapEditor::keyPressedEvent);
    connect(scene_, &GraphicsScene::rightClick, this, &MapEditorForm::mapClicked);
    connect(map_editor_, &MapEditor::newSelectionSetted, this, &MapEditorForm::newSelectionSetted);

    map_editor_->Resize(100, 100, 1);

    map_editor_->SetPointer(2, 2);

    ui->graphicsView->setScene(scene_);

    SetSpriter(new SpriteHolder);

    qDebug() << "Start generate images for creators";
    const auto& objects_metadata = kv::GetCoreInstance().GetObjectsMetadata();
    for (const kv::CoreInterface::ObjectMetadata& metadata : objects_metadata)
    {
        const RawViewInfo& view_info = metadata.default_view;

        if (   view_info.base_frameset.sprite_name.isEmpty()
            || view_info.base_frameset.state.isEmpty())
        {
            qDebug() << "EMPTY frameset:" << metadata.name;
            continue;
        }

        QVector<QPixmap> images;

        View2 view;
        view.LoadViewInfo(view_info);

        if (view.GetBaseFrameset().GetMetadata() == nullptr)
        {
            qDebug() << "EMPTY metadata:" << metadata.name;
            continue;
        }

        for (quint32 dir = 0; dir < view.GetBaseFrameset().GetMetadata()->dirs; ++dir)
        {
            int current_frame_pos = view.GetBaseFrameset().GetMetadata()->first_frame_pos + dir;

            int image_state_h_ = current_frame_pos / view.GetBaseFrameset().GetSprite()->FrameW();
            int image_state_w_ = current_frame_pos % view.GetBaseFrameset().GetSprite()->FrameW();

            QImage img = view.GetBaseFrameset().GetSprite()->GetFrames()
                [image_state_w_ * view.GetBaseFrameset().GetSprite()->FrameH() + image_state_h_];

            images.push_back(QPixmap::fromImage(img));
        }
        map_editor_->AddItemType(metadata.name, images);

        if (images.length() == 0)
        {
            qDebug() << images.length();
        }

        QListWidgetItem* new_item
            = new QListWidgetItem(QIcon(images[0]), metadata.name);

        if (!metadata.turf)
        {
            types_.push_back(metadata.name);
            ui->listWidget->addItem(new_item);
        }
        else
        {
            turf_types_.push_back(metadata.name);
            map_editor_->AddTurfType(metadata.name);
            ui->listWidgetTurf->addItem(new_item);
        }
    }
    qDebug() << "End generating";
}

MapEditorForm::~MapEditorForm()
{
    delete ui;
    delete scene_;
    delete map_editor_;
}

void MapEditorForm::newSelectionSetted(int first_x, int first_y, int second_x, int second_y)
{
    ui->cursor_label->setText(tr("(%1, %2)").arg(first_x).arg(first_y));

    auto entries = map_editor_->GetEntriesFor(first_x, first_y, 0);

    ui->listWidgetTile->clear();
    for (auto it = entries.begin(); it != entries.end(); ++it)
    {
        ui->listWidgetTile->addItem(it->item_type);
    }

    ui->listWidgetVariables->clear();
    ui->lineEditAsString->clear();

    on_listWidgetTile_itemSelectionChanged();
}

void MapEditorForm::on_createItem_clicked()
{
    int current_row = ui->listWidget->currentRow();
    if (current_row < 0)
    {
        return;
    }
    QString type = types_[current_row];
    map_editor_->AddItem(type);
}

void MapEditorForm::mapClicked()
{
    if (is_turf_selected_)
    {
        on_createTurf_clicked();
    }
    else
    {
        on_createItem_clicked();
    }
}

void MapEditorForm::on_createTurf_clicked()
{
    int current_row = ui->listWidgetTurf->currentRow();
    if (current_row < 0)
    {
        return;
    }
    QString type = turf_types_[current_row];
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

void MapEditorForm::on_listWidgetTile_itemSelectionChanged()
{
    QString name;
    if (QListWidgetItem* item = ui->listWidgetVariables->currentItem())
    {
        name = item->text();
    }

    ui->listWidgetVariables->clear();

    QString item_type = GetCurrentEditorEntry()->item_type;

    auto& variables = GetSettersForTypes()[item_type];

    int counter = 0;
    for (auto it = variables.begin(); it != variables.end(); ++it, ++counter)
    {
        ui->listWidgetVariables->addItem(it->first);
        if (it->first == name)
        {
            ui->listWidgetVariables->setCurrentRow(counter);
        }
    }

    MapEditor::EditorEntry* ee = GetCurrentEditorEntry();
    if (ee)
    {
        UpdateVariablesColor(*ee);
    }
}

MapEditor::EditorEntry* MapEditorForm::GetCurrentEditorEntry()
{
    int current_index = ui->listWidgetTile->currentRow();

    int current_x = map_editor_->GetPointer().first_posx;
    int current_y = map_editor_->GetPointer().first_posy;

    if (ui->listWidgetTile->selectedItems().size() == 0)
    {
        return &map_editor_->GetTurfFor(current_x, current_y, 0);
    }

    auto& entries = map_editor_->GetEntriesFor(current_x, current_y, 0);
    if (entries.size() > static_cast<quint32>(current_index))
    {
        return &entries[current_index];
    }
    return nullptr;
}

void MapEditorForm::UpdateVariablesColor(MapEditor::EditorEntry& ee)
{
    for (int i = 0; i < ui->listWidgetVariables->count(); ++i)
    {
        if (!ee.variables[ui->listWidgetVariables->item(i)->text()].isNull())
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

    const QJsonObject& variable_object
        = ee->variables[ui->listWidgetVariables->currentItem()->text()].toObject();

    if (variable_object.size() != 1)
    {
        qDebug() << "Bad variables:" << variable_object;
        return;
    }

    const QString& type = variable_object.begin().key();
    const QJsonValue& variable_value = variable_object.begin().value();

    qDebug() << variable_value;

    if (type == mapgen::key::type::STRING)
    {
        ui->lineEditAsString->setText(variable_value.toString());

        ui->lineEditAsInt->setText("<Wrong type>");
        ui->lineEditAsBool->setText("<Wrong type>");
    }
    else if (type == mapgen::key::type::INT32)
    {
        const int value = static_cast<int>(variable_value.toDouble());
        const QString parsed_value = QString::number(value);
        ui->lineEditAsInt->setText(parsed_value);

        ui->lineEditAsString->setText("<Wrong type>");
        ui->lineEditAsBool->setText("<Wrong type>");
    }
    else if (type == mapgen::key::type::BOOL)
    {
        const bool value = variable_value.toBool();
        const QString parsed_value = value ? "1" : "0";

        ui->lineEditAsBool->setText(parsed_value);

        ui->lineEditAsString->setText("<Wrong type>");
        ui->lineEditAsInt->setText("<Wrong type>");
    }
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

    const QString current_variable = ui->listWidgetVariables->currentItem()->text();
    const QString variable_value = ui->lineEditAsString->text();

    ee->variables[current_variable] = QJsonObject{{mapgen::key::type::STRING, variable_value}};

    on_listWidgetVariables_itemSelectionChanged();
    UpdateVariablesColor(*ee);
}

void MapEditorForm::on_lineEditAsInt_returnPressed()
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

    const QString current_variable = ui->listWidgetVariables->currentItem()->text();

    const QString loc = ui->lineEditAsInt->text();

    bool ok = false;
    const int value = loc.toInt(&ok);
    if (!ok)
    {
        return;
    }

    ee->variables[current_variable] = QJsonObject{{mapgen::key::type::INT32, value}};

    on_listWidgetVariables_itemSelectionChanged();
    UpdateVariablesColor(*ee);

    map_editor_->UpdateDirs(ee);
}

void MapEditorForm::on_lineEditAsBool_returnPressed()
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

    const QString current_variable = ui->listWidgetVariables->currentItem()->text();

    const QString loc = ui->lineEditAsBool->text();

    bool ok = false;
    const bool value = !!loc.toInt(&ok);
    if (!ok)
    {
        return;
    }

    ee->variables[current_variable] = QJsonObject{{mapgen::key::type::BOOL, value}};;

    on_listWidgetVariables_itemSelectionChanged();
    UpdateVariablesColor(*ee);
}

void MapEditorForm::on_listWidgetTurf_clicked(const QModelIndex&)
{
    is_turf_selected_ = true;
}

void MapEditorForm::on_listWidget_clicked(const QModelIndex&)
{
    is_turf_selected_ = false;
}

void MapEditorForm::on_resizeMap_clicked()
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
    map_editor_->Resize(size_x, size_y, size_z);
}

void MapEditorForm::on_saveMapJson_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Mapgen files (*.json)"));
    QStringList file_names;
    if (!dialog.exec())
    {
        return;
    }

    file_names = dialog.selectedFiles();

    if (file_names.isEmpty())
    {
        return;
    }

    const QJsonObject object = map_editor_->SaveMapgenJson();

    const QJsonDocument document(object);
    const QByteArray data = document.toJson(QJsonDocument::Indented);

    QFile file(file_names.first());
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        return;
    }
    file.write(data);
}

void MapEditorForm::on_loadMapJson_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Mapgen files (*.json)"));
    if (!dialog.exec())
    {
        return;
    }

    const QStringList file_names = dialog.selectedFiles();
    if (file_names.isEmpty())
    {
        return;
    }
    const QString name = file_names.first();
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error open " << name;
        return;
    }
    const QByteArray raw_data = file.readAll();
    const QJsonDocument document = QJsonDocument::fromJson(raw_data);

    if (document.isEmpty())
    {
        qDebug() << "Unable to parse file:" << name;
        return;
    }

    map_editor_->LoadMapgenJson(document.object());
}
