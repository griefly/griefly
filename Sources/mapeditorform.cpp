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

GraphicsScene::GraphicsScene(QWidget *parent)
{

}

QGraphicsScene* scene;

MapEditor* map_editor2_ = nullptr;

QLabel* pos_label = nullptr;

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (map_editor2_->GetSelectionStage() == 0)
    {
        //qDebug() << mouseEvent->scenePos();
        map_editor2_->SetPointer(
                    static_cast<int>(mouseEvent->scenePos().rx()) / 32,
                    static_cast<int>(mouseEvent->scenePos().ry()) / 32);
    }
    else if (map_editor2_->GetSelectionStage() == 1)
    {
        map_editor2_->SetFirstSelectionPoint(
                    static_cast<int>(mouseEvent->scenePos().rx()) / 32,
                    static_cast<int>(mouseEvent->scenePos().ry()) / 32);
        map_editor2_->SetSelectionStage(2);
    }
    else
    {
        map_editor2_->SetSecondSelectionPoint(
                    static_cast<int>(mouseEvent->scenePos().rx()) / 32,
                    static_cast<int>(mouseEvent->scenePos().ry()) / 32);
        map_editor2_->SetSelectionStage(0);
    }
    pos_label->setText(
                "("
                + QString::number(map_editor2_->GetPointer().first_posx) + ", "
                + QString::number(map_editor2_->GetPointer().first_posy)
                + ")");
}

MapEditorForm::MapEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);

    pos_label = ui->cursor_label;

    scene = new GraphicsScene;
    map_editor2_ = new MapEditor(scene);
    map_editor2_->Resize(100, 100, 1);

    map_editor2_->SetPointer(2, 2);

    ui->graphicsView->setScene(scene);

    //scene->addLine(0.0, 0.0, 100.0, 100.0);

    SetSpriter(new ASprClass);

    //int i = 0;

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

        //ui->imageLabel->setPixmap(QPixmap::fromImage(img));

        map_editor2_->AddItemType(it->first, QPixmap::fromImage(img));
        //map_editor2_->AddItem(it->first, i++, i, 0);

        //QGraphicsPixmapItem* pixmap_item = scene->addPixmap(QPixmap::fromImage(img));
        //pixmap_item->setPos(32 * (i++), 0);

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
            map_editor2_->AddTurfType(it->first);
            ui->listWidgetTurf->addItem(new_item);
        }

    }

    /*for (int i = 0; i < 100; ++i)
    {
        for (int j = 0; j < 100; ++j)
        {
            map_editor2_->AddItem(types_[(j + i) % types_.size()], i, j, 0);
        }
    }*/
}

MapEditorForm::~MapEditorForm()
{
    delete ui;
}

void MapEditorForm::on_createItem_clicked()
{
    int current_row = ui->listWidget->currentRow();
    if (current_row < 0)
    {
        return;
    }
    std::string type = types_[current_row];
    map_editor2_->AddItem(type);
}

void MapEditorForm::on_createTurf_clicked()
{
    int current_row = ui->listWidgetTurf->currentRow();
    if (current_row < 0)
    {
        return;
    }
    std::string type = turf_types_[current_row];
    map_editor2_->SetTurf(type);
}

void MapEditorForm::on_beginSelection_clicked()
{
    map_editor2_->SetSelectionStage(1);
}

void MapEditorForm::on_removeItem_clicked()
{
    map_editor2_->RemoveItems();
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
    map_editor2_->ClearMap();
    map_editor2_->Resize(size_x, size_y, size_z);
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
    map_editor2_->SaveMapgen(file_names[0].toStdString());
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
    map_editor2_->LoadMapgen(file_names[0].toStdString());
}
