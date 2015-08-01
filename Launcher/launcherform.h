#ifndef LAUNCHERFORM_H
#define LAUNCHERFORM_H

#include <QWidget>
#include <QEvent>
#include <QSystemTrayIcon>
#include <QSharedMemory>
#include <QTimer>

namespace Ui {
class LauncherForm;
}

class LauncherForm : public QWidget
{
    Q_OBJECT

public:
    explicit LauncherForm(QWidget *parent = 0);
    ~LauncherForm();

    void changeEvent(QEvent* e);
public slots:
    void onShowHide(QSystemTrayIcon::ActivationReason reason);
    void checkSharedMemory();
private slots:
    void on_exitButton_clicked();
private:
    void Popup();

    QSharedMemory pop_up_needed_;
    QTimer timer_;

    void CreateTrayIcon();

    QSystemTrayIcon* icon_;

    Ui::LauncherForm *ui;
};

#endif // LAUNCHERFORM_H
