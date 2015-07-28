#ifndef MAPEDITORFORM_H
#define MAPEDITORFORM_H

#include <QWidget>
#include <QGraphicsScene>
#include <QListWidgetItem>

#include <vector>

#include "MapEditor.h"

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
public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
};

class MapEditorForm : public QWidget
{
    Q_OBJECT

public:
    explicit MapEditorForm(QWidget *parent = 0);
    ~MapEditorForm();

public slots:
    void newSelectionSetted(int first_x, int first_y, int second_x, int second_y);
private slots:
    void on_createItem_clicked();

    void on_createTurf_clicked();

    void on_beginSelection_clicked();

    void on_removeItem_clicked();

    void on_newMap_clicked();

    void on_saveMap_clicked();

    void on_loadMap_clicked();

    void on_listWidgetTile_itemSelectionChanged();

//    void on_listWidgetVariables_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidgetVariables_itemSelectionChanged();

    void on_lineEditRaw_returnPressed();

    void on_lineEditAsString_returnPressed();

private:
    MapEditor::EditorEntry& GetCurrentEditorEntry();

    void UpdateVariablesColor(MapEditor::EditorEntry& ee);

    GraphicsScene* scene_;
    MapEditor* map_editor_;

    // Metainfo
    std::vector<std::string> types_;
    std::vector<std::string> turf_types_;

    Ui::MapEditorForm *ui;
};

#endif // MAPEDITORFORM_H
