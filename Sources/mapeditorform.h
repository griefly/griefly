#ifndef MAPEDITORFORM_H
#define MAPEDITORFORM_H

#include <QWidget>

namespace Ui {
class MapEditorForm;
}

class MapEditorForm : public QWidget
{
    Q_OBJECT

public:
    explicit MapEditorForm(QWidget *parent = 0);
    ~MapEditorForm();

private:
    Ui::MapEditorForm *ui;
};

#endif // MAPEDITORFORM_H
