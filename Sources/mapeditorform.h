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

private:
    std::vector<unsigned int> types_;
    std::vector<unsigned int> turf_types_;

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
