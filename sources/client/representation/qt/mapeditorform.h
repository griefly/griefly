#ifndef MAPEDITORFORM_H
#define MAPEDITORFORM_H

#include <QWidget>
#include <QGraphicsScene>
#include <QListWidgetItem>

#include <vector>

#include "MapEditor.h"

#include <CoreInterface.h>

namespace Ui {
class MapEditorForm;
}

class GraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QWidget *parent = nullptr);
signals:
    void mousePressed(QGraphicsSceneMouseEvent* mouseEvent);
    void keyboardPressed(QKeyEvent* event);
    void rightClick();
public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void keyPressEvent(QKeyEvent* event);
};

struct Asset
{
    QString name;
    QString sprite;
    QString state;
    bool turf;

    struct VariableInfo
    {
        QString name;
        QString type;
        // TODO: value?
    };
    QVector<VariableInfo> variables;
};

class MapEditorForm : public QWidget
{
    Q_OBJECT
public:
    explicit MapEditorForm(QWidget *parent = nullptr);
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

    void on_set_value_push_button_clicked();

    void on_unset_value_push_button_clicked();

private:
    void LoadAssets();

    bool is_turf_selected_;

    MapEditor::EditorEntry* GetCurrentEditorEntry();
    QString GetCurrentVariableType();

    void ResetVariablesPanel();

    void UpdateVariablesColor(MapEditor::EditorEntry& ee);

    GraphicsScene* scene_;
    MapEditor* map_editor_;

    // Metainfo
    std::vector<QString> types_;
    std::vector<QString> turf_types_;

    kv::CoreInterface::ObjectsMetadata objects_metadata_;

    std::vector<Asset> assets_;

    Ui::MapEditorForm *ui;
};

#endif // MAPEDITORFORM_H
