#ifndef MAPEDITORFORM_H
#define MAPEDITORFORM_H

#include <QWidget>

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

    Ui::MapEditorForm *ui;
};

#endif // MAPEDITORFORM_H
