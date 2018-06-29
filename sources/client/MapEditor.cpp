#include "MapEditor.h"

#include <QBitmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>

#include <Mapgen.h>
#include <Dir.h>

#include "representation/SpriteHolder.h"

const QVector<QPixmap>& SpriteCache::GetSprite(const QString& sprite, const QString& state)
{
    auto it = sprites_.find({sprite, state});
    if (it != sprites_.end())
    {
        return it->second;
    }

    GLSprite* gl_sprite = GetSpriter().GetSprite(sprite);
    const auto& sprite_metadata = gl_sprite->GetMetadata();
    if (!sprite_metadata.IsValidState(state))
    {
        return sprites_.insert({{sprite, state}, QVector<QPixmap>()}).first->second;
    }
    const auto& state_metadata = sprite_metadata.GetSpriteMetadata(state);

    QVector<QPixmap> pixmaps;
    for (qint32 dir = 0; dir < state_metadata.dirs; ++dir)
    {
        const int current_frame_pos = state_metadata.first_frame_pos + dir;

        const int image_state_h = current_frame_pos / gl_sprite->FrameW();
        const int image_state_w = current_frame_pos % gl_sprite->FrameW();

        QImage image = gl_sprite->GetFrames()
            [image_state_w * gl_sprite->FrameH() + image_state_h];

        pixmaps.push_back(QPixmap::fromImage(image));
    }
    return sprites_.insert({{sprite, state}, pixmaps}).first->second;
}

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
    copypaste_items_.clear();

    selection_stage_ = 0;

    first_selection_x_ = 0;
    first_selection_y_ = 0;
    second_selection_x_ = 0;
    second_selection_y_ = 0;

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
}

void MapEditor::keyPressedEvent(QKeyEvent *event)
{
    if (    (event->key() == Qt::Key_C)
         && (event->modifiers() & Qt::ControlModifier))
    {
        //CopyItemsFromCurrentTile();
        CopyToAreaBuffer();
    }
    if (    (event->key() == Qt::Key_V)
         && (event->modifiers() & Qt::ControlModifier))
    {
        //PasteItemsToCurrentTile();
        PasteFromAreaBuffer();
    }
}

void MapEditor::CopyItemsFromCurrentTile()
{
    copypaste_items_ = editor_map_[pointer_.first_posx][pointer_.first_posy][0].items;
}

void MapEditor::PasteItemsToCurrentTile()
{
    for (auto it = copypaste_items_.begin(); it != copypaste_items_.end(); ++it)
    {
        auto& new_item = AddItem(
            it->item_type,
            it->sprite_name,
            it->state,
            {},
            pointer_.first_posx,
            pointer_.first_posy,
            0);
        new_item.variables = it->variables;
        UpdateDirs(&new_item);
    }

    emit newSelectionSetted(
                first_selection_x_, first_selection_y_,
                second_selection_x_, second_selection_y_);
}

void MapEditor::CopyToAreaBuffer()
{
    area_buffer_.resize(pointer_.second_posx + 1 - pointer_.first_posx);
    for (int i = 0; i < static_cast<int>(area_buffer_.size()); ++i)
    {
        area_buffer_[i].resize(pointer_.second_posy + 1 - pointer_.first_posy);
    }
    for (int x = 0; x < static_cast<int>(area_buffer_.size()); ++x)
    {
        for (int y = 0; y < static_cast<int>(area_buffer_[0].size()); ++y)
        {
            area_buffer_[x][y] = editor_map_[pointer_.first_posx + x][pointer_.first_posy + y][0];
        }
    }
}

void MapEditor::PasteFromAreaBuffer()
{
    int end_x = std::min(
                    editor_map_.size(),
                    pointer_.first_posx + area_buffer_.size());
    int end_y = std::min(
                    editor_map_[0].size(),
                    pointer_.first_posy + area_buffer_[0].size());

    for (int x = 0; x < end_x - pointer_.first_posx; ++x)
    {
        for (int y = 0; y < end_y - pointer_.first_posy; ++y)
        {
            auto& new_item = SetTurf(
                area_buffer_[x][y].turf.item_type,
                area_buffer_[x][y].turf.sprite_name,
                area_buffer_[x][y].turf.state,
                pointer_.first_posx + x,
                pointer_.first_posy + y,
                0);
            new_item.variables = area_buffer_[x][y].turf.variables;
            UpdateDirs(&new_item);
            for (auto it = area_buffer_[x][y].items.begin();
                      it != area_buffer_[x][y].items.end();
                    ++it)
            {
                auto& new_item = AddItem(
                    it->item_type,
                    it->sprite_name,
                    it->state,
                    {},
                    pointer_.first_posx + x,
                    pointer_.first_posy + y,
                    0);
                new_item.variables = it->variables;
                UpdateDirs(&new_item);
            }
        }
    }

    SetFirstSelectionPoint(pointer_.first_posx, pointer_.first_posy);
    SetSecondSelectionPoint(end_x - 1, end_y - 1);

    emit newSelectionSetted(
                first_selection_x_, first_selection_y_,
                second_selection_x_, second_selection_y_);
}

QJsonObject MapEditor::EntryToJson(const MapEditor::EditorEntry& entry) const
{
    using namespace mapgen;

    QJsonObject object_info;
    object_info.insert(key::TYPE, entry.item_type);
    object_info.insert(key::SPRITE, entry.sprite_name);
    object_info.insert(key::STATE, entry.state);
    QJsonObject variables;
    for (auto var = entry.variables.begin(); var != entry.variables.end(); ++var)
    {
        if (var.value().isNull())
        {
            continue;
        }
        const QJsonValue value = var.value();
        variables.insert(var.key(), value);
    }
    object_info.insert(key::VARIABLES, variables);

    return object_info;
}

QJsonObject MapEditor::SaveMapgenJson() const
{
    using namespace mapgen;

    const int size_x = editor_map_.size();
    const int size_y = editor_map_[0].size();
    const int size_z = editor_map_[0][0].size();

    QJsonArray tiles;
    for (int z = 0; z < size_z; ++z)
    {
        for (int x = 0; x < size_x; ++x)
        {
            for (int y = 0; y < size_y; ++y)
            {
                const EditorTile tile = editor_map_[x][y][z];
                QJsonObject tile_info;
                tile_info.insert(key::X, x);
                tile_info.insert(key::Y, y);
                tile_info.insert(key::Z, z);
                if (editor_map_[x][y][z].turf.pixmap_item)
                {
                    tile_info.insert(key::TURF, EntryToJson(tile.turf));
                }

                QJsonArray objects;

                const auto& il = editor_map_[x][y][z].items;
                for (auto it = il.begin(); it != il.end(); ++it)
                {
                    objects.append(EntryToJson(*it));
                }
                tile_info.insert(key::OBJECTS, objects);

                tiles.append(tile_info);
            }
        }
    }
    QJsonObject retval;
    retval.insert(key::WIDTH, size_x);
    retval.insert(key::HEIGHT, size_y);
    retval.insert(key::DEPTH, size_z);
    retval.insert(key::TILES, tiles);

    return retval;
}

void MapEditor::LoadMapgenJson(const QJsonObject& data)
{
    using namespace mapgen;

    ClearMap();

    // TODO: validate json

    const int width = data.value(key::WIDTH).toInt();
    const int height = data.value(key::HEIGHT).toInt();
    const int depth = data.value(key::DEPTH).toInt();

    Resize(width, height, depth);

    qDebug() << width << height << depth;

    const QJsonArray& tiles = data.value(key::TILES).toArray();

    for (const QJsonValue& value : tiles)
    {
        const QJsonObject tile = value.toObject();

        const int x = tile.value(key::X).toInt();
        const int y = tile.value(key::Y).toInt();
        const int z = tile.value(key::Z).toInt();

        CreateEntity(x, y, z, tile.value(key::TURF).toObject(), true);

        const QJsonArray objects = tile.value(key::OBJECTS).toArray();
        for (const QJsonValue& object_value : objects)
        {
            CreateEntity(x, y, z, object_value.toObject(), false);
        }
    }
}

void MapEditor::CreateEntity(int x, int y, int z, const QJsonObject& info, bool is_turf)
{
    using namespace mapgen;

    if (info.isEmpty())
    {
        return;
    }

    const QString item_type = info.value(key::TYPE).toString();

    QString sprite;
    QString state;

    if (info.contains(key::SPRITE))
    {
        sprite = info.value(key::SPRITE).toString();
        state = info.value(key::STATE).toString();
    }
    else
    {
        auto sprite_state = sprite_state_names_holder_[item_type];
        sprite = sprite_state.first;
        state = sprite_state.second;
    }

    MapEditor::EditorEntry* entry;
    if (is_turf)
    {
        entry = &SetTurf(item_type, sprite, state, x, y, z);
    }
    else
    {
        entry = &AddItem(item_type, sprite, state, {}, x, y, z);
    }

    const QJsonObject variables = info.value(key::VARIABLES).toObject();

    for (const QString& key : variables.keys())
    {
        entry->variables.insert(key, variables.value(key));
    }
    UpdateDirs(entry);
}

void MapEditor::fix_borders(int *posx, int *posy)
{
    if (*posx < 0)
    {
        *posx = 0;
    }
    if (*posx >= static_cast<int>(editor_map_.size()))
    {
        *posx = static_cast<int>(editor_map_.size()) - 1;
    }
    if (*posy < 0)
    {
        *posy = 0;
    }
    if (*posy >= static_cast<int>(editor_map_[0].size()))
    {
        *posy = static_cast<int>(editor_map_[0].size()) - 1;
    }
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

    emit newSelectionSetted(
        first_selection_x_,
        first_selection_y_,
        second_selection_x_,
        second_selection_y_);
}

void MapEditor::AddItemType(
    const QString& item_type, const QString& sprite, const QString& state)
{
    sprite_state_names_holder_[item_type] = {sprite, state};
}

void MapEditor::AddTurfType(const QString& item_type)
{
    turf_types_.insert(item_type);
}

void MapEditor::AddItem(const QString &item_type,
    const QString& sprite,
    const QString& state,
    const QVector<std::pair<QString, QJsonValue>>& variables)
{
    for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
    {
        for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
        {
            AddItem(item_type, sprite, state, variables, x, y, 0);
        }
    }

    emit newSelectionSetted(
                first_selection_x_, first_selection_y_,
                second_selection_x_, second_selection_y_);
}

void MapEditor::UpdateDirs(MapEditor::EditorEntry* ee)
{
    if (!ee)
    {
        return;
    }
    const QJsonValue data = ee->variables["direction_"].toObject()[mapgen::key::type::INT32];
    if (!data.isNull())
    {
        const Dir dir = static_cast<Dir>(data.toInt());
        const int byond_dir = kv::helpers::DirToByond(dir);

        const QVector<QPixmap>& sprites = sprite_cache_.GetSprite(ee->sprite_name, ee->state);
        if (byond_dir < sprites.size())
        {
            ee->pixmap_item->setPixmap(sprites[byond_dir]);
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

    emit newSelectionSetted(
        first_selection_x_, first_selection_y_,
        second_selection_x_, second_selection_y_);
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

MapEditor::EditorEntry& MapEditor::AddItem(
    const QString &item_type,
    const QString& sprite,
    const QString& state,
    const QVector<std::pair<QString, QJsonValue>>& variables,
    int posx,
    int posy,
    int posz)
{
    EditorEntry new_entry;
    new_entry.item_type = item_type;
    const QVector<QPixmap>& sprites = sprite_cache_.GetSprite(sprite, state);
    new_entry.pixmap_item = scene_->addPixmap(sprites[0]);
    new_entry.pixmap_item->setPos(posx * 32, posy * 32);
    new_entry.pixmap_item->setZValue(50);
    new_entry.sprite_name = sprite;
    new_entry.state = state;

    for (const auto& variable : variables)
    {
        new_entry.variables[variable.first] = variable.second;
    }

    editor_map_[posx][posy][posz].items.push_back(new_entry);
    return editor_map_[posx][posy][posz].items.back();
}

void MapEditor::SetTurf(const QString& item_type, const QString& sprite, const QString& state)
{
    // For performance everything should be removed firstly
    for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
    {
        for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
        {
            scene_->removeItem(editor_map_[x][y][0].turf.pixmap_item);
            delete editor_map_[x][y][0].turf.pixmap_item;
            editor_map_[x][y][0].turf.pixmap_item = nullptr;
        }
    }

    for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
    {
        for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
        {
            SetTurf(item_type, sprite, state, x, y, 0);
        }
    }

    emit newSelectionSetted(
                first_selection_x_, first_selection_y_,
                second_selection_x_, second_selection_y_);
}

MapEditor::EditorEntry& MapEditor::SetTurf(
    const QString &item_type, const QString& sprite, const QString& state, int posx, int posy, int posz)
{
    if (editor_map_[posx][posy][posz].turf.pixmap_item != nullptr)
    {
        scene_->removeItem(editor_map_[posx][posy][posz].turf.pixmap_item);
        delete editor_map_[posx][posy][posz].turf.pixmap_item;
    }

    EditorEntry new_entry;
    new_entry.item_type = item_type;
    const QVector<QPixmap>& sprites = sprite_cache_.GetSprite(sprite, state);
    new_entry.pixmap_item = scene_->addPixmap(sprites[0]);
    new_entry.pixmap_item->setPos(posx * 32, posy * 32);
    new_entry.sprite_name = sprite;
    new_entry.state = state;
    editor_map_[posx][posy][posz].turf = new_entry;
    return editor_map_[posx][posy][posz].turf;
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

QVector<MapEditor::EditorEntry>& MapEditor::GetEntriesFor(int posx, int posy, int posz)
{
    return editor_map_[posx][posy][posz].items;
}

MapEditor::EditorEntry& MapEditor::GetTurfFor(int posx, int posy, int posz)
{
    return editor_map_[posx][posy][posz].turf;
}
