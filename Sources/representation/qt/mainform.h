#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QTimer>
#include <QLineEdit>
#include <QKeyEvent>

namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();
public slots:
    void startGameLoop(int id, QString map);
    void connectionFailed(QString reason);

    void insertHtmlIntoChat(QString html);
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void setFocusOnLineEdit();

    void helperAutoConnect();
    void on_lineEdit_returnPressed();

    void on_splitter_splitterMoved(int pos, int index);
signals:
    void autoConnect();

private:

    void connectToHost();

    QTimer* activeTimer;

    int left_column;
    int right_column;

    int argc_;
    char** argv_;
    Ui::MainForm *ui;
};

#endif // MAINFORM_H
