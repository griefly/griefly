#include "mapeditorform.h"
#include "ui_mapeditorform.h"

#include "ItemFabric.h"
#include "MapClass.h"
#include "Text.h"

#include <map>

#include <QBitmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

GraphicsScene::GraphicsScene(QWidget *parent)
{

}

QGraphicsScene* scene;

class MapEditor2
{
public:
    struct EditorEntry
    {
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
            posx = 0;
            posy = 0;
            image = nullptr;
        }

        int posx;
        int posy;

        QGraphicsPolygonItem* image;
    };

    MapEditor2(QGraphicsScene* scene)
        : scene_(scene)
    {
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

    void SetPointer(int posx, int posy)
    {
        if (posx < 0)
            posx = 0;
        if (posx >= static_cast<int>(editor_map_.size()))
            posx = static_cast<int>(editor_map_.size()) - 1;
        if (posy < 0)
            posy = 0;
        if (posy >= static_cast<int>(editor_map_[0].size()))
            posy = static_cast<int>(editor_map_[0].size()) - 1;

        pointer_.posx = posx;
        pointer_.posy = posy;
        pointer_.image->setPos(posx * 32, posy * 32);
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

    void AddItem(unsigned int item_type)
    {
        AddItem(item_type, pointer_.posx, pointer_.posy, 0);
    }

    void AddItem(unsigned int item_type, int posx, int posy, int posz)
    {
        EditorEntry new_entry;
        new_entry.item_type = item_type;
        new_entry.pixmap_item = scene_->addPixmap(image_holder_[item_type]);
        new_entry.pixmap_item->setPos(posx * 32, posy * 32);
        editor_map_[posx][posy][posz].items.push_back(new_entry);
    }
    void SetTurf(unsigned int item_type, int posx, int posy, int posz)
    {
        EditorEntry new_entry;
        new_entry.item_type = item_type;
        new_entry.pixmap_item = scene_->addPixmap(image_holder_[item_type]);
        new_entry.pixmap_item->setPos(posx * 32, posy * 32);
        editor_map_[posx][posy][posz].turf = new_entry;
    }

private:
    QGraphicsPolygonItem* border_image_;

    Pointer pointer_;

    std::map<unsigned int, QPixmap> image_holder_;

    QGraphicsScene* scene_;

    typedef std::vector<std::vector<std::vector<EditorTile>>> MapType;
    MapType editor_map_;
};

MapEditor2* map_editor2_ = nullptr;

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    qDebug() << mouseEvent->scenePos();
    map_editor2_->SetPointer(
                static_cast<int>(mouseEvent->scenePos().rx()) / 32,
                static_cast<int>(mouseEvent->scenePos().ry()) / 32);
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

        types_.push_back(it->first);

        ui->listWidget->addItem(new_item);

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
