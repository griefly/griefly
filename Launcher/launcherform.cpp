#include "launcherform.h"
#include "ui_launcherform.h"

#include <QTimer>
#include <QPixmap>

LauncherForm::LauncherForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LauncherForm)
{
    ui->setupUi(this);

    connect(ui->exitButton, &QPushButton::clicked, &QApplication::quit);

    setWindowFlags(Qt::SubWindow);
    CreateTrayIcon();
}

LauncherForm::~LauncherForm()
{
    delete ui;
}

void LauncherForm::changeEvent(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::WindowStateChange:
        if (this->windowState() & Qt::WindowMinimized)
        {
            // Workaround for travis build
            QTimer::singleShot(0, this, SLOT(hide()));
        }
        break;
    default:
        break;
    }

    QWidget::changeEvent(e);
}

void LauncherForm::onShowHide(QSystemTrayIcon::ActivationReason reason)
{
    if (reason)
    {
        if (reason != QSystemTrayIcon::DoubleClick)
        {
            return;
        }
    }

    if (isVisible())
    {
        hide();
    }
    else
    {
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        raise();  // for MacOS
        activateWindow(); // for Windows
        show();
        setFocus();
    }
}

void LauncherForm::CreateTrayIcon()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(QColor("red"));
    QIcon red_icon(pixmap);

    icon_ = new QSystemTrayIcon(red_icon, this);
    connect(icon_, &QSystemTrayIcon::activated, this, &LauncherForm::onShowHide);
    icon_->show();
}

void LauncherForm::on_exitButton_clicked()
{
    //close();
}
