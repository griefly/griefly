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

class MapEditor2
{
public:
    struct EditorEntry
    {
        EditorEntry()
        {
            pixmap_item = nullptr;
        }

        unsigned int item_type;

        QGraphicsPixmapItem* pixmap_item;
    };

    struct EditorTile
    {
        EditorEntry turf;
        std::vector<EditorEntry> items;
    };

    struct Pointer
    {
        Pointer()
        {
            first_posx = 0;
            first_posy = 0;
            second_posx = 0;
            second_posy = 0;
            image = nullptr;
        }

        int first_posx;
        int first_posy;

        int second_posx;
        int second_posy;

        QGraphicsPolygonItem* image;
    };

    MapEditor2(QGraphicsScene* scene)
        : scene_(scene)
    {
        selection_stage_ = 0;

        QPolygonF p;
        p.push_back(QPointF(0.0, 0.0));
        p.push_back(QPointF(0.0, 32.0));
        p.push_back(QPointF(32.0, 32.0));
        p.push_back(QPointF(32.0, 0.0));

        QPen pen(QColor(200, 0, 0));
        QBrush brush(QColor(100, 0, 100, 100));

        pointer_.image = scene->addPolygon(p, pen, brush);
        pointer_.image->setZValue(100);

        QPolygonF p2;
        p2.push_back(QPointF(0.0, 0.0));
        p2.push_back(QPointF(0.0, 10 * 32.0));
        p2.push_back(QPointF(10 * 32.0, 10 * 32.0));
        p2.push_back(QPointF(10 * 32.0, 0.0));

        QPen pen2(QColor(0, 100, 200));
        pen2.setWidth(2);

        border_image_ = scene->addPolygon(p2, pen2);
        border_image_->setZValue(99);
    }

    void SaveMapgen(const std::string& name)
    {
        std::fstream sfile;
        sfile.open(name, std::ios_base::out | std::ios_base::trunc);
        if(sfile.fail())
        {
            return;
        }

        int size_x = editor_map_.size();
        int size_y = editor_map_[0].size();
        int size_z = editor_map_[0][0].size();

        sfile << size_x << std::endl;
        sfile << size_y << std::endl;
        sfile << size_z << std::endl;

        for (int z = 0; z < size_z; ++z)
            for (int x = 0; x < size_x; ++x)
                for (int y = 0; y < size_y; ++y)
                {
                    if (editor_map_[x][y][z].turf.pixmap_item)
                    {
                        sfile << editor_map_[x][y][z].turf.item_type << " ";
                        sfile << x << " ";
                        sfile << y << " ";
                        sfile << z << " ";
                        sfile << std::endl;
                    }
                    auto& il = editor_map_[x][y][z].items;
                    for (auto it = il.begin(); it != il.end(); ++it)
                    {
                        sfile << it->item_type << " ";
                        sfile << x << " ";
                        sfile << y << " ";
                        sfile << z << " ";
                        sfile << std::endl;
                    }
                }
    }

    void LoadMapgen(const std::string& name)
    {
        std::fstream sfile;
        sfile.open(name, std::ios_base::in);
        if(sfile.fail())
        {
            SYSTEM_STREAM << "Error open " << name << std::endl;
            return;
        }

        ClearMap();

        int x, y, z;
        sfile >> x;
        sfile >> y;
        sfile >> z;

        Resize(x, y, z);

        while (!sfile.eof())
        {
            unsigned int t_item;
            size_t x, y, z;
            sfile >> t_item;
            sfile >> x;
            sfile >> y;
            sfile >> z;

            if (turf_types_.find(t_item) != turf_types_.end())
            {
                SetTurf(t_item, x, y, z);
            }
            else
            {
                AddItem(t_item, x, y, z);
            }
        }
    }

    void fix_borders(int* posx, int* posy)
    {
        if (*posx < 0)
            *posx = 0;
        if (*posx >= static_cast<int>(editor_map_.size()))
            *posx = static_cast<int>(editor_map_.size()) - 1;
        if (*posy < 0)
            *posy = 0;
        if (*posy >= static_cast<int>(editor_map_[0].size()))
            *posy = static_cast<int>(editor_map_[0].size()) - 1;
    }

    void SetPointer(int posx, int posy)
    {
        fix_borders(&posx, &posy);

        SetFirstSelectionPoint(posx, posy);
        SetSecondSelectionPoint(posx, posy);

        //pointer_.image->setPos(posx * 32, posy * 32);
    }

    void Resize(int posx, int posy, int posz)
    {
        editor_map_.resize(posx);
        for (auto it_y = editor_map_.begin(); it_y != editor_map_.end(); ++it_y)
        {
            it_y->resize(posy);
            for (auto it_z = it_y->begin(); it_z != it_y->end(); ++it_z)
            {
                it_z->resize(posz);
            }
        }

        QPolygonF p2;
        p2.push_back(QPointF(0.0, 0.0));
        p2.push_back(QPointF(0.0, posy * 32.0));
        p2.push_back(QPointF(posx * 32.0, posy * 32.0));
        p2.push_back(QPointF(posx * 32.0, 0.0));
        border_image_->setPolygon(p2);
    }

    void AddItemType(unsigned int item_type, QPixmap image)
    {
        image_holder_[item_type] = image;
    }

    void AddTurfType(unsigned int type)
    {
        turf_types_.insert(type);
    }

    void AddItem(unsigned int item_type)
    {
        for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
        {
            for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
            {
                AddItem(item_type, x, y, 0);
            }
        }
    }

    void RemoveItems()
    {
        for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
        {
            for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
            {
                RemoveItems(x, y, 0);
            }
        }
    }

    void RemoveItems(int posx, int posy, int posz)
    {
        auto& items = editor_map_[posx][posy][posz].items;
        for (auto it = items.begin(); it != items.end(); ++it)
        {
            scene_->removeItem(it->pixmap_item);
            delete it->pixmap_item;
        }
    }

    void AddItem(unsigned int item_type, int posx, int posy, int posz)
    {
        EditorEntry new_entry;
        new_entry.item_type = item_type;
        new_entry.pixmap_item = scene_->addPixmap(image_holder_[item_type]);
        new_entry.pixmap_item->setPos(posx * 32, posy * 32);
        new_entry.pixmap_item->setZValue(50);

        editor_map_[posx][posy][posz].items.push_back(new_entry);
    }

    void SetTurf(unsigned int item_type)
    {
        for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
        {
            for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
            {
                SetTurf(item_type, x, y, 0);
            }
        }
    }

    void SetTurf(unsigned int item_type, int posx, int posy, int posz)
    {
        if (editor_map_[posx][posy][posz].turf.pixmap_item != nullptr)
        {
            scene_->removeItem(editor_map_[posx][posy][posz].turf.pixmap_item);
            delete editor_map_[posx][posy][posz].turf.pixmap_item;
        }

        EditorEntry new_entry;
        new_entry.item_type = item_type;
        new_entry.pixmap_item = scene_->addPixmap(image_holder_[item_type]);
        new_entry.pixmap_item->setPos(posx * 32, posy * 32);
        editor_map_[posx][posy][posz].turf = new_entry;
    }

    void SetSelectionStage(int stage)
    {
        selection_stage_ = stage;
    }
    int GetSelectionStage() const
    {
        return selection_stage_;
    }

    void SetFirstSelectionPoint(int x, int y)
    {
        fix_borders(&x, &y);

        first_selection_x_ = x;
        first_selection_y_ = y;
    }

    void SetSecondSelectionPoint(int x, int y)
    {
        fix_borders(&x, &y);

        second_selection_x_ = x;
        second_selection_y_ = y;

        if (first_selection_x_ > second_selection_x_)
            std::swap(first_selection_x_, second_selection_x_);
        if (first_selection_y_ > second_selection_y_)
            std::swap(first_selection_y_, second_selection_y_);

        pointer_.first_posx = first_selection_x_;
        pointer_.first_posy = first_selection_y_;

        pointer_.second_posx = second_selection_x_;
        pointer_.second_posy = second_selection_y_;

        QPolygonF p;
        p.push_back(QPointF(pointer_.first_posx * 32.0, pointer_.first_posy * 32.0));
        p.push_back(QPointF(pointer_.first_posx * 32.0, (pointer_.second_posy + 1)  * 32.0));
        p.push_back(QPointF((pointer_.second_posx + 1) * 32.0, (pointer_.second_posy + 1)  * 32.0));
        p.push_back(QPointF((pointer_.second_posx + 1) * 32.0, pointer_.first_posy * 32.0));
        pointer_.image->setPolygon(p);
        pointer_.image->setPos(0, 0);
    }

    void ClearMap()
    {
        for (auto it_x = editor_map_.begin(); it_x != editor_map_.end(); ++it_x)
        {
            for (auto it_y = it_x->begin(); it_y != it_x->end(); ++it_y)
            {
                for (auto it_z = it_y->begin(); it_z != it_y->end(); ++it_z)
                {
                    auto& items = it_z->items;
                    for (auto it = items.begin(); it != items.end(); ++it)
                    {
                        scene_->removeItem(it->pixmap_item);
                        delete it->pixmap_item;
                    }

                    if (it_z->turf.pixmap_item)
                    {
                        scene_->removeItem(it_z->turf.pixmap_item);
                        delete it_z->turf.pixmap_item;
                    }

                    items.clear();
                    it_z->turf.pixmap_item = nullptr;
                }
            }
        }
    }

private:
    int selection_stage_;

    int first_selection_x_;
    int first_selection_y_;

    int second_selection_x_;
    int second_selection_y_;

    QGraphicsPolygonItem* border_image_;

    Pointer pointer_;

    std::map<unsigned int, QPixmap> image_holder_;

    std::set<unsigned int> turf_types_;

    QGraphicsScene* scene_;

    typedef std::vector<std::vector<std::vector<EditorTile>>> MapType;
    MapType editor_map_;
};

MapEditor2* map_editor2_ = nullptr;

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (map_editor2_->GetSelectionStage() == 0)
    {
        qDebug() << mouseEvent->scenePos();
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
}

MapEditorForm::MapEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);

    scene = new GraphicsScene;
    map_editor2_ = new MapEditor2(scene);
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
                = new QListWidgetItem(QIcon(QPixmap::fromImage(img)), bloc->name.c_str());

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
    unsigned int type = types_[current_row];
    map_editor2_->AddItem(type);
}

void MapEditorForm::on_createTurf_clicked()
{
    int current_row = ui->listWidgetTurf->currentRow();
    if (current_row < 0)
    {
        return;
    }
    unsigned int type = turf_types_[current_row];
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
