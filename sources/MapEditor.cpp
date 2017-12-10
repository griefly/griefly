#include "MapEditor.h"

#include <QBitmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>

#include <fstream>

#include "core/KvGlobals.h"
#include "core/Helpers.h"
#include "core/FastSerializer.h"
#include "core/SaveableOperators.h"

#include "core_headers/Mapgen.h"

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
        auto& new_item = AddItem(it->item_type, pointer_.first_posx, pointer_.first_posy, 0);
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

void MapEditor::SaveMapgen(const QString& name)
{
    int size_x = editor_map_.size();
    int size_y = editor_map_[0].size();
    int size_z = editor_map_[0][0].size();

    kv::FastSerializer data;

    data << size_x;
    data << size_y;
    data << size_z;

    for (int z = 0; z < size_z; ++z)
    {
        for (int x = 0; x < size_x; ++x)
        {
            for (int y = 0; y < size_y; ++y)
            {
                if (editor_map_[x][y][z].turf.pixmap_item)
                {
                    data.WriteType(editor_map_[x][y][z].turf.item_type);
                    data << x;
                    data << y;
                    data << z;
                    data << editor_map_[x][y][z].turf.variables;
                }
                auto& il = editor_map_[x][y][z].items;
                for (auto it = il.begin(); it != il.end(); ++it)
                {
                    data.WriteType(it->item_type);
                    data << x;
                    data << y;
                    data << z;
                    data << it->variables;
                }
            }
        }
    }
    QFile file(name);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.fileName() << " cannot be opened.";
        return;
    }

    // Binary mapgen is really bad for version control systems, so
    // converting it to hex with line breaks

    QByteArray hex = QByteArray(data.GetData(), data.GetIndex()).toHex();

    int index = -1;
    int old_index = 0;
    while (true)
    {
        index = hex.indexOf("0602", index + 1);
        if (index == -1)
        {
            break;
        }
        file.write(hex.data() + old_index, index - old_index);
        file.write("\n");
        old_index = index;
    }
    file.write(hex.data() + old_index, hex.size() - old_index);
    file.write("\n");
}

namespace
{

QJsonValue ConvertSerializedToJson(const QByteArray& data)
{
    using namespace mapgen;

    kv::FastDeserializer deserializer(data.data(), data.size());
    const kv::FastSerializer::Type type = deserializer.GetNextType();
    switch (type)
    {
    case kv::FastSerializer::BOOL_TYPE:
    {
        bool retval;
        deserializer >> retval;
        return QJsonObject{{key::type::BOOL, retval}};
    }
    case kv::FastSerializer::INT32_TYPE:
    {
        qint32 retval;
        deserializer >> retval;
        return QJsonObject{{key::type::INT32, retval}};
    }
    case kv::FastSerializer::UINT32_TYPE:
    {
        quint32 retval;
        deserializer >> retval;
        return QJsonObject{{key::type::UINT32, static_cast<double>(retval)}};
    }
    case kv::FastSerializer::STRING_TYPE:
    {
        QString retval;
        deserializer >> retval;
        return QJsonObject{{key::type::STRING, retval}};
    }
    case kv::FastSerializer::BYTEARRAY_TYPE:
    {
        QByteArray retval;
        deserializer >> retval;
        return QJsonObject{
            {key::type::BYTEARRAY, QString::fromLatin1(retval.toHex())}};
    }
    case kv::FastSerializer::TYPE_TYPE:
    {
        QString retval;
        deserializer >> retval;
        return QJsonObject{{key::type::TYPE, retval}};
    }
    default:
        qDebug() << "Unknown type:" << type;
        return QJsonValue();
    }
    KV_UNREACHABLE
}

QByteArray ConvertJsonToSerialized(const QJsonValue& data)
{
    using namespace mapgen;

    kv::FastSerializer serializer(1024);

    const QJsonObject object = data.toObject();
    const QStringList keys = object.keys();
    if (keys.isEmpty())
    {
        qFatal("ConvertJsonToSerialized: no keys!");
    }
    const QString type = keys.first();
    const QJsonValue value = object.value(type);

    if (type == key::type::BOOL)
    {
        serializer << value.toBool();
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::INT32)
    {
        serializer << value.toInt();
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::UINT32)
    {
        serializer << static_cast<quint32>(value.toDouble());
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::STRING)
    {
        serializer << value.toString();
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::BYTEARRAY)
    {
        serializer << QByteArray::fromHex(value.toString().toLatin1());
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::TYPE)
    {
        serializer << value.toString();
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    qDebug() << "Unknown type:" << data;
    return QByteArray();
}

QJsonObject EntryToJson(const MapEditor::EditorEntry& entry)
{
    using namespace mapgen;

    QJsonObject object_info;
    object_info.insert(key::TYPE, entry.item_type);
    QJsonObject variables;
    for (auto var = entry.variables.begin(); var != entry.variables.end(); ++var)
    {
        if (var.value().isEmpty())
        {
            continue;
        }
        const QJsonValue value = ConvertSerializedToJson(var.value());
        variables.insert(var.key(), value);
    }
    object_info.insert(key::VARIABLES, variables);

    return object_info;
}

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

void MapEditor::LoadMapgen(const QString& name)
{
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error open " << name;
        return;
    }

    ClearMap();

    QByteArray raw_data;
    while (file.bytesAvailable())
    {
        QByteArray local = file.readLine();
        if (local.size() < 1)
        {
            break;
        }
        local = local.left(local.size() - 1);
        raw_data.append(local);
    }
    raw_data = QByteArray::fromHex(raw_data);
    kv::FastDeserializer data(raw_data.data(), raw_data.size());

    int x;
    data >> x;

    int y;
    data >> y;

    int z;
    data >> z;

    Resize(x, y, z);

    qDebug() << x << y << z;

    while (!data.IsEnd())
    {
        QString item_type;
        qint32 x;
        qint32 y;
        qint32 z;

        data.ReadType(&item_type);

        data >> x;
        data >> y;
        data >> z;

        MapEditor::EditorEntry* ee;
        if (turf_types_.find(item_type) != turf_types_.end())
        {
            ee = &SetTurf(item_type, x, y, z);
        }
        else
        {
            ee = &AddItem(item_type, x, y, z);
        }

        data >> ee->variables;
        UpdateDirs(ee);
    }
}

void MapEditor::LoadMapgenJson(const QJsonObject& data)
{
    using namespace mapgen;

    ClearMap();

    const int width = data.value(key::WIDTH).toInt();
    const int height = data.value(key::HEIGHT).toInt();
    const int depth = data.value(key::DEPTH).toInt();

    Resize(width, height, depth);

    qDebug() << width << height << depth;

    const QJsonArray& tiles = data.value(key::TILES).toArray();

    for (const QJsonValue& value : tiles)
    {
        const QJsonObject tile = value.toObject();

        kv::Position position;
        position.x = tile.value(key::X).toInt();
        position.y = tile.value(key::Y).toInt();
        position.z = tile.value(key::Z).toInt();

        CreateEntity(position, tile.value(key::TURF).toObject(), true);

        const QJsonArray objects = tile.value(key::OBJECTS).toArray();
        for (const QJsonValue& object_value : objects)
        {
            CreateEntity(position, object_value.toObject(), false);
        }
    }
}

void MapEditor::CreateEntity(kv::Position position, const QJsonObject& info, bool is_turf)
{
    using namespace mapgen;

    if (info.isEmpty())
    {
        return;
    }

    const QString item_type = info.value(key::TYPE).toString();

    MapEditor::EditorEntry* entry;
    if (is_turf)
    {
        entry = &SetTurf(item_type, position.x, position.y, position.z);
    }
    else
    {
        entry = &AddItem(item_type, position.x, position.y, position.z);
    }

    const QJsonObject variables = info.value(key::VARIABLES).toObject();

    for (const QString& key : variables.keys())
    {
        entry->variables.insert(key, ConvertJsonToSerialized(variables.value(key)));
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
                first_selection_x_, first_selection_y_,
                second_selection_x_, second_selection_y_);
}

void MapEditor::AddItemType(const QString &item_type, QVector<QPixmap> image)
{
    image_holder_[item_type] = image;
}

void MapEditor::AddTurfType(const QString &item_type)
{
    turf_types_.insert(item_type);
}

void MapEditor::AddItem(const QString &item_type)
{
    for (int x = pointer_.first_posx; x <= pointer_.second_posx; ++x)
    {
        for (int y = pointer_.first_posy; y <= pointer_.second_posy; ++y)
        {
            AddItem(item_type, x, y, 0);
        }
    }

    emit newSelectionSetted(
                first_selection_x_, first_selection_y_,
                second_selection_x_, second_selection_y_);
}

void MapEditor::UpdateDirs(MapEditor::EditorEntry* ee)
{
    QByteArray& data = ee->variables["direction_"];
    if (ee && data.size())
    {
        kv::FastDeserializer deserializer(data.data(), data.size());
        Dir dir;
        deserializer >> dir;
        int byond_dir = helpers::DirToByond(dir);

        if (byond_dir < image_holder_[ee->item_type].size())
        {
            ee->pixmap_item->setPixmap(image_holder_[ee->item_type][byond_dir]);
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

MapEditor::EditorEntry& MapEditor::AddItem(const QString &item_type, int posx, int posy, int posz)
{
    EditorEntry new_entry;
    new_entry.item_type = item_type;
    new_entry.pixmap_item = scene_->addPixmap(image_holder_[item_type][0]);
    new_entry.pixmap_item->setPos(posx * 32, posy * 32);
    new_entry.pixmap_item->setZValue(50);

    editor_map_[posx][posy][posz].items.push_back(new_entry);
    return editor_map_[posx][posy][posz].items.back();
}

void MapEditor::SetTurf(const QString &item_type)
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
            SetTurf(item_type, x, y, 0);
        }
    }

    emit newSelectionSetted(
                first_selection_x_, first_selection_y_,
                second_selection_x_, second_selection_y_);
}

MapEditor::EditorEntry& MapEditor::SetTurf(const QString &item_type, int posx, int posy, int posz)
{
    if (editor_map_[posx][posy][posz].turf.pixmap_item != nullptr)
    {
        scene_->removeItem(editor_map_[posx][posy][posz].turf.pixmap_item);
        delete editor_map_[posx][posy][posz].turf.pixmap_item;
    }

    EditorEntry new_entry;
    new_entry.item_type = item_type;
    new_entry.pixmap_item = scene_->addPixmap(image_holder_[item_type][0]);
    new_entry.pixmap_item->setPos(posx * 32, posy * 32);
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

