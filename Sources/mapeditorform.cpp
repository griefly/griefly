#include "mapeditorform.h"
#include "ui_mapeditorform.h"

#include "ItemFabric.h"
#include "MapClass.h"
#include "Text.h"

#include <map>

#include <QBitmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

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

    MapEditor2(QGraphicsScene* scene)
        : scene_(scene)
    {

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
    }

    void AddItemType(unsigned int item_type, QPixmap image)
    {
        image_holder_[item_type] = image;
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
    std::map<unsigned int, QPixmap> image_holder_;

    QGraphicsScene* scene_;

    typedef std::vector<std::vector<std::vector<EditorTile>>> MapType;
    MapType editor_map_;
};

MapEditor2* map_editor2_ = nullptr;

MapEditorForm::MapEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);

    scene = new QGraphicsScene;    
    map_editor2_ = new MapEditor2(scene);
    map_editor2_->Resize(100, 100, 1);

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
}

MapEditorForm::~MapEditorForm()
{
    delete ui;
}

void MapEditorForm::on_createItem_clicked()
{
    static int i = 0;
    int current_row = ui->listWidget->currentRow();
    if (current_row < 0)
    {
        return;
    }
    unsigned int type = types_[current_row];
    map_editor2_->AddItem(type, i++, i, 0);
}
