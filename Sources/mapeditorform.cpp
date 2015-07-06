#include "mapeditorform.h"
#include "ui_mapeditorform.h"

#include "ItemFabric.h"
#include "MapClass.h"
#include "Text.h"

#include <QBitmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

QGraphicsScene* scene;

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

std::vector<
    std::vector<
        std::vector<
            EditorTile
        >>> editor_map;


MapEditorForm::MapEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);

    scene = new QGraphicsScene;

    ui->graphicsView->setScene(scene);

    scene->addLine(0.0, 0.0, 100.0, 100.0);

    SetSpriter(new ASprClass);

    int i = 0;

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

        QGraphicsPixmapItem* pixmap_item = scene->addPixmap(QPixmap::fromImage(img));
        pixmap_item->setPos(32 * (i++), 0);

        QListWidgetItem* new_item
                = new QListWidgetItem(QIcon(QPixmap::fromImage(img)), bloc->name.c_str());

        ui->listWidget->addItem(new_item);

    }
}

MapEditorForm::~MapEditorForm()
{
    delete ui;
}
