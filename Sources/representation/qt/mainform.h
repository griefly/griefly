#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QTimer>
#include <QLineEdit>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QMap>

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
    void addSystemText(QString key, QString text);

    void startGameLoop(int id, QString map);
    void connectionFailed(QString reason);

    void insertHtmlIntoChat(QString html);
    void playMusic(QString name, float volume);
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void setFocusOnLineEdit();

    void helperAutoConnect();
    void on_lineEdit_returnPressed();

    void on_splitter_splitterMoved(int pos, int index);
signals:
    void autoConnect();
    void closing();
private:

    void connectToHost();

    QTimer* activeTimer;

    int left_column;
    int right_column;

    int argc_;
    char** argv_;
    Ui::MainForm *ui;

    QMap<QString, QString> texts_;
};

#endif // MAINFORM_H
