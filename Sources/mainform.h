#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QTimer>

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
    void helperAutoConnect();
    void on_lineEdit_returnPressed();

    void on_splitter_splitterMoved(int pos, int index);
signals:
    void autoConnect();

private:
    void startGameLoop();

    QTimer* activeTimer;

    int left_column;
    int right_column;

    int argc_;
    char** argv_;
    Ui::MainForm *ui;
};

#endif // MAINFORM_H
