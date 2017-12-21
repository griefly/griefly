#ifndef MAPEDITORFORM_H
#define MAPEDITORFORM_H

#include <QWidget>
#include <QGraphicsScene>
#include <QListWidgetItem>

#include <vector>

#include "MapEditor.h"
#include "core/FastSerializer.h"

namespace Ui {
class MapEditorForm;
}

class GraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QWidget *parent = 0);
signals:
    void mousePressed(QGraphicsSceneMouseEvent* mouseEvent);
    void keyboardPressed(QKeyEvent* event);
    void rightClick();
public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void keyPressEvent(QKeyEvent* event);
};

class MapEditorForm : public QWidget
{
    Q_OBJECT

public:
    explicit MapEditorForm(QWidget *parent = 0);
    ~MapEditorForm();
public slots:
    void newSelectionSetted(int first_x, int first_y, int second_x, int second_y);
    void mapClicked();
private slots:
    void on_createItem_clicked();

    void on_createTurf_clicked();

    void on_beginSelection_clicked();

    void on_removeItem_clicked();

    void on_newMap_clicked();

    void on_listWidgetTile_itemSelectionChanged();

//    void on_listWidgetVariables_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidgetVariables_itemSelectionChanged();

    void on_lineEditAsString_returnPressed();

    void on_listWidgetTurf_clicked(const QModelIndex &index);

    void on_listWidget_clicked(const QModelIndex &index);

    void on_resizeMap_clicked();

    void on_lineEditAsInt_returnPressed();

    void on_lineEditAsBool_returnPressed();

    void on_saveMapJson_clicked();

    void on_loadMapJson_clicked();

private:
    bool is_turf_selected_;

    MapEditor::EditorEntry* GetCurrentEditorEntry();

    void UpdateVariablesColor(MapEditor::EditorEntry& ee);

    GraphicsScene* scene_;
    MapEditor* map_editor_;

    // Metainfo
    std::vector<QString> types_;
    std::vector<QString> turf_types_;

    kv::FastSerializer serializer_;

    Ui::MapEditorForm *ui;
};

#endif // MAPEDITORFORM_H
