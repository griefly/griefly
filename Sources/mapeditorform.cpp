#include "mapeditorform.h"
#include "ui_mapeditorform.h"

#include "ItemFabric.h"
#include "MapClass.h"
#include "Text.h"

#include <QBitmap>

MapEditorForm::MapEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapEditorForm)
{
    ui->setupUi(this);

    SetSpriter(new ASprClass);
    //SetGLContext(ui->openGLWidget);

    for (auto it = (*itemList()).begin(); it != (*itemList()).end(); ++it)
    {
        IMainObject* loc = it->second(0);
        IOnMapObject* bloc = castTo<IOnMapObject>(loc);
        if (!bloc)
        {
            //delete loc;
            continue;
        }
        ui->listWidget->addItem(bloc->name.c_str());

        if (bloc->GetMetadata() == nullptr)
        {
            continue;
        }

        int current_frame_pos = bloc->GetMetadata()->first_frame_pos;

        int image_state_h_ = current_frame_pos / bloc->GetSprite()->FrameW();
        int image_state_w_ = current_frame_pos % bloc->GetSprite()->FrameW();

        SDL_Surface* s = bloc->GetSprite()->GetSDLSprite()->frames
                [4];
                //[image_state_h_ * bloc->GetSprite()->FrameW() + image_state_w_];
        // SDL_Surface s = bloc->GetSprite()->GetSDLSprite()->frames[image_state_w_][image_state_h_];
        QImage img(static_cast<uchar*>(s->pixels),
                               s->w, s->h, QImage::Format_ARGB32);

        if (it == ++(*itemList()).begin())
            ui->imageLabel->setPixmap(QPixmap::fromImage(img));

    }
}

MapEditorForm::~MapEditorForm()
{
    delete ui;
}
