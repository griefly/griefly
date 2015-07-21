#ifndef MAPEDITORFORM_H
#define MAPEDITORFORM_H

#include <QWidget>
#include <QGraphicsScene>

#include <vector>

namespace Ui {
class MapEditorForm;
}

class MapEditorForm : public QWidget
{
    Q_OBJECT

public:
    explicit MapEditorForm(QWidget *parent = 0);
    ~MapEditorForm();

private slots:
    void on_createItem_clicked();

    void on_createTurf_clicked();

    void on_beginSelection_clicked();

    void on_removeItem_clicked();

    void on_newMap_clicked();

    void on_saveMap_clicked();

    void on_loadMap_clicked();

private:
    // Metainfo
    std::vector<std::string> types_;
    std::vector<std::string> turf_types_;

    Ui::MapEditorForm *ui;
};

class GraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QWidget *parent = 0);
public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
};

#endif // MAPEDITORFORM_H
