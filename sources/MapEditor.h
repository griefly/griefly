#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include <QObject>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QVector>

#include "core/KvGlobals.h"

using MapgenVariablesType = QMap<QString, QJsonValue>;

class MapEditor : public QObject
{
    Q_OBJECT
public:
    struct EditorEntry
    {
        EditorEntry();

        QString item_type;

        MapgenVariablesType variables;

        QGraphicsPixmapItem* pixmap_item;
    };

    struct EditorTile
    {
        EditorEntry turf;
        QVector<EditorEntry> items;
    };

    struct Pointer
    {
        Pointer();

        int first_posx;
        int first_posy;

        int second_posx;
        int second_posy;

        QGraphicsPolygonItem* image;
    };

    MapEditor(QGraphicsScene* scene);

    QJsonObject SaveMapgenJson() const;

    void LoadMapgenJson(const QJsonObject& data);
    void CreateEntity(kv::Position position, const QJsonObject& info, bool is_turf);

    void fix_borders(int* posx, int* posy);

    void SetPointer(int posx, int posy);

    void Resize(int posx, int posy, int posz);

    void AddItemType(const QString& item_type, QVector<QPixmap> images);

    void AddTurfType(const QString& item_type);

    void AddItem(const QString& item_type);

    void UpdateDirs(EditorEntry* ee);

    void RemoveItems();

    void RemoveItems(int posx, int posy, int posz);

    MapEditor::EditorEntry& AddItem(const QString& item_type, int posx, int posy, int posz);

    void SetTurf(const QString& item_type);

    MapEditor::EditorEntry& SetTurf(const QString& item_type, int posx, int posy, int posz);

    void SetSelectionStage(int stage);
    int GetSelectionStage() const;

    void SetFirstSelectionPoint(int x, int y);

    void SetSecondSelectionPoint(int x, int y);

    void ClearMap();

    QVector<EditorEntry>& GetEntriesFor(int posx, int posy, int posz);
    EditorEntry& GetTurfFor(int posx, int posy, int posz);

    const Pointer& GetPointer() const { return pointer_; }
signals:
    void newSelectionSetted(int first_x, int first_y, int second_x, int second_y);
public slots:
    void mousePressedEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void keyPressedEvent(QKeyEvent* event);
private:
    QVector<EditorEntry> copypaste_items_;

    void CopyItemsFromCurrentTile();
    void PasteItemsToCurrentTile();

    QVector<QVector<EditorTile>> area_buffer_;

    void CopyToAreaBuffer();
    void PasteFromAreaBuffer();


    int selection_stage_;

    int first_selection_x_;
    int first_selection_y_;

    int second_selection_x_;
    int second_selection_y_;

    QGraphicsPolygonItem* border_image_;

    Pointer pointer_;

    QMap<QString, QVector<QPixmap>> image_holder_;

    QSet<QString> turf_types_;

    QGraphicsScene* scene_;

    using MapType = QVector<QVector<QVector<EditorTile>>>;
    MapType editor_map_;
};
