#include "launcherform.h"
#include "ui_launcherform.h"

#include <QTimer>
#include <QPixmap>

LauncherForm::LauncherForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LauncherForm)
{
    ui->setupUi(this);

    pop_up_needed_.setKey("4668074d-98a4-4052-b8fb-0a3a7a84905c");
    pop_up_needed_.create(1);

    connect(&timer_, &QTimer::timeout, this, &LauncherForm::checkSharedMemory);
    connect(ui->exitButton, &QPushButton::clicked, &QApplication::quit);

    timer_.start(300);
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
            // SLOT is workaround for travis build
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
        Popup();
    }
}

void LauncherForm::checkSharedMemory()
{
    pop_up_needed_.lock();

    bool* is_needed = static_cast<bool*>(pop_up_needed_.data());

    if (*is_needed)
    {
        Popup();
        *is_needed = false;
    }

    pop_up_needed_.unlock();
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

void LauncherForm::Popup()
{
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();  // for MacOS
    activateWindow(); // for Windows
    show();
    setFocus();
}
