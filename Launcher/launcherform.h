#ifndef LAUNCHERFORM_H
#define LAUNCHERFORM_H

#include <QWidget>

namespace Ui {
class LauncherForm;
}

class LauncherForm : public QWidget
{
    Q_OBJECT

public:
    explicit LauncherForm(QWidget *parent = 0);
    ~LauncherForm();

private:
    Ui::LauncherForm *ui;
};

#endif // LAUNCHERFORM_H
