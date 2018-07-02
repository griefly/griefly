#include "mapeditorform.h"
#include "ui_mapeditorform.h"

#include <Mapgen.h>
#include <CoreInterface.h>

#include "representation/View2.h"
#include "representation/SpriteHolder.h"

#include <map>
#include <set>

#include <QBitmap>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonArray>

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

    LoadAssets();

    qDebug() << "Start generate images for creators";

    for (const Asset& asset : qAsConst(assets_))
    {
        RawViewInfo view_info;
        view_info.base_frameset.sprite_name = asset.sprite;
        view_info.base_frameset.state = asset.state;

        if (   view_info.base_frameset.sprite_name.isEmpty()
            || view_info.base_frameset.state.isEmpty())
        {
            qDebug() << "EMPTY frameset:" << asset.asset_name;
            continue;
        }

        View2 view;
        view.LoadViewInfo(view_info);

        if (view.GetBaseFrameset().GetMetadata() == nullptr)
        {
            qDebug() << "EMPTY metadata:" << asset.asset_name;
            continue;
        }

        QPixmap pixmap = [&]()
        {
            const int current_frame_pos = view.GetBaseFrameset().GetMetadata()->first_frame_pos;

            const int image_state_h_ = current_frame_pos / view.GetBaseFrameset().GetSprite()->FrameW();
            const int image_state_w_ = current_frame_pos % view.GetBaseFrameset().GetSprite()->FrameW();

            QImage image = view.GetBaseFrameset().GetSprite()->GetFrames()
                [image_state_w_ * view.GetBaseFrameset().GetSprite()->FrameH() + image_state_h_];

            return QPixmap::fromImage(image);
        }();
        map_editor_->AddItemType(asset.asset_name, asset.sprite, asset.state);

        QListWidgetItem* new_item
            = new QListWidgetItem(QIcon(pixmap), asset.asset_name);

        if (!asset.turf)
        {
            types_.push_back(asset.asset_name);
            ui->listWidget->addItem(new_item);
        }
        else
        {
            turf_types_.push_back(asset.asset_name);
            map_editor_->AddTurfType(asset.asset_name);
            ui->listWidgetTurf->addItem(new_item);
        }
    }

    qDebug() << "End generating";
    showMaximized();
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

    ResetVariablesPanel();

    on_listWidgetTile_itemSelectionChanged();
}

void MapEditorForm::on_createItem_clicked()
{
    const int current_row = ui->listWidget->currentRow();
    if (current_row < 0)
    {
        return;
    }
    const QString type = types_[current_row];
    auto it = std::find_if(assets_.begin(), assets_.end(),
    [&](const Asset& asset)
    {
        return asset.asset_name == type;
    });
    map_editor_->AddItem(it->type_name, it->sprite, it->state);
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
    const int current_row = ui->listWidgetTurf->currentRow();
    if (current_row < 0)
    {
        return;
    }
    const QString type = turf_types_[current_row];
    auto it = std::find_if(assets_.begin(), assets_.end(),
    [&](const Asset& asset)
    {
        return asset.asset_name == type;
    });

    QVector<std::pair<QString, QJsonValue>> variables;
    for (auto variable_info : it->variables)
    {
        if (variable_info.value.isNull())
        {
            continue;
        }
        variables.append({variable_info.name, variable_info.value});
    }

    map_editor_->SetTurf(it->type_name, it->sprite, it->state, variables);
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

    const QString item_type = GetCurrentEditorEntry()->item_type;

    auto it = std::find_if(assets_.begin(), assets_.end(),
    [&](const Asset& asset)
    {
        return asset.asset_name == item_type;
    });
    if (it == assets_.end())
    {
        return;
    }
    const auto& variables = it->variables;

    int counter = 0;
    for (auto it = variables.begin(); it != variables.end(); ++it, ++counter)
    {
        if (it->type == mapgen::key::type::UNKNOWN)
        {
            continue;
        }
        ui->listWidgetVariables->addItem(it->name);
        if (it->name == name)
        {
            ui->listWidgetVariables->setCurrentRow(counter);
            break;
        }
    }

    if (counter == variables.size())
    {
        ResetVariablesPanel();
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

QString MapEditorForm::GetCurrentVariableType()
{
    MapEditor::EditorEntry* ee = GetCurrentEditorEntry();
    if (!ee)
    {
        return QString();
    }

    auto it = std::find_if(assets_.begin(), assets_.end(),
    [&](const Asset& asset)
    {
        return asset.asset_name == ee->item_type;
    });
    if (it == assets_.end())
    {
        return QString();
    }
    const auto& variables = it->variables;
    QListWidgetItem* current_item = ui->listWidgetVariables->currentItem();
    if (current_item == nullptr)
    {
        return QString();
    }

    auto variable = std::find_if(variables.begin(), variables.end(), [name = current_item->text()](auto variable)
    {
        return variable.name == name;
    });
    if (variable == variables.end())
    {
        return QString();
    }
    ui->current_variable_type_label->setText(QString("Current type: %1").arg(variable->type));
    return variable->type;
}

void MapEditorForm::ResetVariablesPanel()
{
    ui->string_line_edit->hide();
    ui->int32_spin_box->hide();
    ui->bool_check_box->hide();
    ui->unsupported_label->show();

    ui->string_line_edit->clear();
    ui->int32_spin_box->clear();
    ui->bool_check_box->setChecked(false);

    ui->frame->hide();
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

    QString variable_type = GetCurrentVariableType();
    if (variable_type.isEmpty())
    {
        return;
    }

    const QJsonObject& variable_object
        = ee->variables[ui->listWidgetVariables->currentItem()->text()].toObject();

    ui->frame->show();

    ui->string_line_edit->hide();
    ui->int32_spin_box->hide();
    ui->bool_check_box->hide();
    ui->unsupported_label->hide();

    ui->set_value_push_button->setEnabled(true);
    ui->unset_value_push_button->setEnabled(true);

    QJsonValue variable_value;
    if (variable_object.size() == 0)
    {
        ui->unset_value_push_button->setEnabled(false);
    }
    else
    {
        const QString& type = variable_object.begin().key();
        variable_value = variable_object.begin().value();

        qDebug() << variable_value;

        if (type != variable_type)
        {
            qDebug() << "Variable type mismatch: " << type << variable_type;
            abort();
        }
    }

    if (variable_type == mapgen::key::type::STRING)
    {
        ui->string_line_edit->setText(variable_value.toString());
        ui->string_line_edit->show();
    }
    else if (variable_type == mapgen::key::type::INT32)
    {
        const int value = static_cast<int>(variable_value.toDouble());
        ui->int32_spin_box->setValue(value);
        ui->int32_spin_box->show();
    }
    else if (variable_type == mapgen::key::type::BOOL)
    {
        const bool value = variable_value.toBool();
        ui->bool_check_box->setChecked(value);
        ui->bool_check_box->show();
    }
    else
    {
        ui->unsupported_label->show();
        ui->set_value_push_button->setEnabled(false);
    }

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

void MapEditorForm::on_set_value_push_button_clicked()
{
    MapEditor::EditorEntry* ee = GetCurrentEditorEntry();
    if (!ee)
    {
        return;
    }

    const QString type = GetCurrentVariableType();
    if (type.isEmpty())
    {
        return;
    }

    const QString current_variable = ui->listWidgetVariables->currentItem()->text();

    if (type == mapgen::key::type::STRING)
    {
        const QString variable_value = ui->string_line_edit->text();
        ee->variables[current_variable] = QJsonObject{{mapgen::key::type::STRING, variable_value}};
    }
    else if (type == mapgen::key::type::INT32)
    {
        const int variable_value = ui->int32_spin_box->value();
        ee->variables[current_variable] = QJsonObject{{mapgen::key::type::INT32, variable_value}};
    }
    else if (type == mapgen::key::type::BOOL)
    {
        const bool variable_value = ui->bool_check_box->isChecked();
        ee->variables[current_variable] = QJsonObject{{mapgen::key::type::BOOL, variable_value}};
    }

    on_listWidgetVariables_itemSelectionChanged();
    UpdateVariablesColor(*ee);
}

void MapEditorForm::on_unset_value_push_button_clicked()
{
    MapEditor::EditorEntry* ee = GetCurrentEditorEntry();
    if (!ee)
    {
        return;
    }

    const QString current_variable = ui->listWidgetVariables->currentItem()->text();
    qDebug() << current_variable;
    ee->variables.remove(current_variable);

    ResetVariablesPanel();
    on_listWidgetVariables_itemSelectionChanged();
}

namespace
{

namespace key
{

const QString IS_TURF("is_turf");
const QString SPRITE("sprite");
const QString SPRITE_STATE("sprite_state");
const QString ASSET_NAME("asset_name");
const QString TYPENAME("typename");
const QString VARIABLES("variables");
const QString NAME("name");
const QString TYPE("type");
const QString VALUE("value");

}

} // namespace

void MapEditorForm::LoadAssets()
{
    QDir dir("assets/");
    const QStringList& files = dir.entryList({"*.json"});
    for (const QString& filename : files)
    {
        QFile file(dir.path() + "/" + filename);
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "Unable to open" << file.fileName();
            continue;
        }
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        const QJsonObject asset_json = document.object();

        // TODO: proper validation
        Asset asset;
        asset.turf = asset_json[key::IS_TURF].toBool();
        asset.sprite = asset_json[key::SPRITE].toString();
        asset.state = asset_json[key::SPRITE_STATE].toString();
        asset.type_name = asset_json[key::TYPENAME].toString();
        asset.asset_name = asset_json[key::ASSET_NAME].toString();

        const QJsonArray variables = asset_json[key::VARIABLES].toArray();
        for (const QJsonValue& value : variables)
        {
            const QJsonObject object = value.toObject();
            Asset::VariableInfo info;
            info.name = object[key::NAME].toString();
            info.type = object[key::TYPE].toString();
            info.value = object[key::VALUE];
            asset.variables.append(info);
        }

        assets_.push_back(asset);
    }

}
