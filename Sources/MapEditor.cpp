#include "MapEditor.h"

#include <QBitmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>

#include <fstream>

#include "constheader.h"

MapEditor::EditorEntry::EditorEntry()
{
    pixmap_item = nullptr;
}

MapEditor::Pointer::Pointer()
{
    first_posx = 0;
    first_posy = 0;
    second_posx = 0;
    second_posy = 0;
    image = nullptr;
}


MapEditor::MapEditor(QGraphicsScene* scene)
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

void MapEditor::mousePressedEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (GetSelectionStage() == 0)
    {
        //qDebug() << mouseEvent->scenePos();
        SetPointer(
                    static_cast<int>(mouseEvent->scenePos().rx()) / 32,
                    static_cast<int>(mouseEvent->scenePos().ry()) / 32);
    }
    else if (GetSelectionStage() == 1)
    {
        SetFirstSelectionPoint(
                    static_cast<int>(mouseEvent->scenePos().rx()) / 32,
                    static_cast<int>(mouseEvent->scenePos().ry()) / 32);
        SetSelectionStage(2);
    }
    else
    {
        SetSecondSelectionPoint(
                    static_cast<int>(mouseEvent->scenePos().rx()) / 32,
                    static_cast<int>(mouseEvent->scenePos().ry()) / 32);
        SetSelectionStage(0);
    }
   // TOOD: emit "posChanged"
   // pos_label->setText(
   //             "("
   //             + QString::number(map_editor2_->GetPointer().first_posx) + ", "
   //             + QString::number(map_editor2_->GetPointer().first_posy)
   //             + ")");
}

void MapEditor::SaveMapgen(const std::string &name)
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

void MapEditor::LoadMapgen(const std::string &name)
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
        std::string t_item;
        size_t x, y, z;
        sfile >> t_item;
        if (t_item == "")
        {
            continue;
        }
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

void MapEditor::fix_borders(int *posx, int *posy)
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

void MapEditor::SetPointer(int posx, int posy)
{
    fix_borders(&posx, &posy);

    SetFirstSelectionPoint(posx, posy);
    SetSecondSelectionPoint(posx, posy);

    //pointer_.image->setPos(posx * 32, posy * 32);
}

void MapEditor::Resize(int posx, int posy, int posz)
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

void MapEditor::AddItemType(const std::string &item_type, QPixmap image)
{
    image_holder_[item_type] = image;
}

void MapEditor::AddTurfType(const std::string &item_type)
{
    turf_types_.insert(item_type);
}

void MapEditor::AddItem(const std::string &item_type)
{
    for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
    {
        for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
        {
            AddItem(item_type, x, y, 0);
        }
    }
}

void MapEditor::RemoveItems()
{
    for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
    {
        for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
        {
            RemoveItems(x, y, 0);
        }
    }
}

void MapEditor::RemoveItems(int posx, int posy, int posz)
{
    auto& items = editor_map_[posx][posy][posz].items;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        scene_->removeItem(it->pixmap_item);
        delete it->pixmap_item;
    }
    items.clear();
}

void MapEditor::AddItem(const std::string &item_type, int posx, int posy, int posz)
{
    EditorEntry new_entry;
    new_entry.item_type = item_type;
    new_entry.pixmap_item = scene_->addPixmap(image_holder_[item_type]);
    new_entry.pixmap_item->setPos(posx * 32, posy * 32);
    new_entry.pixmap_item->setZValue(50);

    editor_map_[posx][posy][posz].items.push_back(new_entry);
}

void MapEditor::SetTurf(const std::string &item_type)
{
    for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
    {
        for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
        {
            SetTurf(item_type, x, y, 0);
        }
    }
}

void MapEditor::SetTurf(const std::string &item_type, int posx, int posy, int posz)
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

void MapEditor::SetSelectionStage(int stage)
{
    selection_stage_ = stage;
}

int MapEditor::GetSelectionStage() const
{
    return selection_stage_;
}

void MapEditor::SetFirstSelectionPoint(int x, int y)
{
    fix_borders(&x, &y);

    first_selection_x_ = x;
    first_selection_y_ = y;
}

void MapEditor::SetSecondSelectionPoint(int x, int y)
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

    emit newSelectionSetted(
                first_selection_x_, first_selection_y_,
                second_selection_x_, second_selection_y_);
}

void MapEditor::ClearMap()
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

const std::vector<MapEditor::EditorEntry> &MapEditor::GetEntriesFor(int posx, int posy, int posz)
{
    return editor_map_[posx][posy][posz].items;
}

