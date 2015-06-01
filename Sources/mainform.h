#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>

namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void on_lineEdit_returnPressed();

    void on_splitter_splitterMoved(int pos, int index);

private:

    int left_column;
    int right_column;

    void startGameLoop();

    int argc_;
    char** argv_;
    Ui::MainForm *ui;
};

#endif // MAINFORM_H
