#include "mainform.h"
#include "ui_mainform.h"

#include <iostream>

#include "Map.h"
#include "Debug.h"
#include "Params.h"
#include "Manager.h"
#include "MagicStrings.h"

#include "Metadata.h"

#include "qtopengl.h"

#include "Chat.h"
#include "Network2.h"
#include "NetworkMessagesTypes.h"

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
    InitChat(ui->textBrowser);
    QList<int> sizes;
    sizes.push_back(512);
    sizes.push_back(256);
    ui->splitter->setSizes(sizes);

    setWindowTitle("Griefly");

    left_column = ui->leftColumn->width();
    right_column = ui->rightColumn->width();

    ui->widget->hide();

    SetMainWidget(this);

    connect(&Network2::GetInstance(), &Network2::connectionSuccess, this, &MainForm::startGameLoop);
    connect(&Network2::GetInstance(), &Network2::connectionFailed, this, &MainForm::connectionFailed);

    connect(this, &MainForm::autoConnect, this, &MainForm::on_lineEdit_returnPressed);
    connect(ui->widget, &QtOpenGL::enterPressed, this, &MainForm::setFocusOnLineEdit);

    connect(ui->lineEdit, &GamingLineEdit::keyToPass, ui->widget, &QtOpenGL::handlePassedKey);

    if (GetParamsHolder().GetParamBool("-auto_connect"))
    {
        activeTimer = new QTimer(this);
        activeTimer->setInterval(1 * 1000);
        activeTimer->setSingleShot(true);
        connect(activeTimer, &QTimer::timeout, this, &MainForm::helperAutoConnect);
        activeTimer->start();
    }
}

void MainForm::setFocusOnLineEdit()
{
    ui->lineEdit->setFocus();
}

void MainForm::helperAutoConnect()
{
    ui->lineEdit->setText("/connect");

    emit autoConnect();
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::resizeEvent(QResizeEvent* event) {
    ui->lineEdit->resize(width(), ui->lineEdit->height());
    ui->lineEdit->move(ui->lineEdit->x(), height() - ui->lineEdit->height());
    ui->splitter->resize(width(), ui->lineEdit->y());

    QList<int> sizes;
    sizes.push_back((width() * left_column) / (left_column + right_column));
    sizes.push_back((width() * right_column) / (left_column + right_column));
    ui->splitter->setSizes(sizes);
    on_splitter_splitterMoved(0, 0);
}

void MainForm::startGameLoop(int id, QString map)
{
    ui->widget->show();
    on_splitter_splitterMoved(0, 0);

    if (GetParamsHolder().GetParamBool("-nodraw"))
        NODRAW = true;

    if (GetParamsHolder().GetParamBool("name"))
        Debug::SetUniqueName(GetParamsHolder().GetParam<std::string>("name"));

    Manager man;
    man.InitWorld(id, map.toStdString());
    if (GetParamsHolder().GetParamBool("-auto"))
        man.ToogleAutoplay();
    man.Process();
    return;
}

void MainForm::connectionFailed(QString reason)
{
    ui->textBrowser->insertHtml("<b>Connection failed!</b>");
    ui->textBrowser->append(reason);
    ui->textBrowser->insertHtml("<br>If you would like to reconnect then do it manually<br>");
}

void MainForm::connectToHost()
{
    std::string adrs = "127.0.0.1";
    if (GetParamsHolder().GetParamBool("ip"))
    {
        adrs = GetParamsHolder().GetParam<std::string>("ip");
    }

    int port = 1111;
    if (GetParamsHolder().GetParamBool("port"))
    {
        port = GetParamsHolder().GetParam<int>("port");
    }

    std::string login = "Guest";
    if (GetParamsHolder().GetParamBool("login"))
    {
        login = GetParamsHolder().GetParam<std::string>("login");
    }

    std::string password = "";
    if (GetParamsHolder().GetParamBool("password"))
    {
        password = GetParamsHolder().GetParam<std::string>("password");
    }

    Network2::GetInstance().TryConnect(
                QString::fromStdString(adrs),
                port,
                QString::fromStdString(login),
                QString::fromStdString(password));
}

void MainForm::on_lineEdit_returnPressed()
{
    static bool connected = false;
    if (!connected && ui->lineEdit->text().toStdString() == "/connect")
    {
        connected = true;
        ui->lineEdit->clear();
        connectToHost();
        return;
    }

    if (!connected)
    {
        return;
    }


    Message2 msg;
    msg.type = MessageType::MESSAGE;
    if (ui->lineEdit->text().length() == 0)
        return;

    QJsonObject object;
    object["text"] = ui->lineEdit->text();

    QJsonDocument doc(object);
    msg.json = doc.toJson();

    Network2::GetInstance().SendMsg(msg);

    ui->lineEdit->clear();

    //ui->widget->setFocus();
}

void MainForm::on_splitter_splitterMoved(int pos, int index)
{
    ui->textBrowser->resize(
                ui->rightColumn->width(),
                ui->textBrowser->height());

    left_column = ui->leftColumn->width();
    right_column = ui->rightColumn->width();

    int min_size = std::min(ui->leftColumn->width(), ui->leftColumn->height());
    ui->widget->resize(min_size, min_size);

    ui->textBrowser->resize(ui->textBrowser->width(), ui->rightColumn->height());

    if (GetScreen())
    {
        GetScreen()->PerformSizeUpdate();
        GetScreen()->Clear();
        GetScreen()->Swap();
    }

    int x_pos = (ui->leftColumn->width() - min_size) / 2;
    int y_pos = (ui->leftColumn->height() - min_size) / 2;
    ui->widget->move(x_pos, y_pos);
}
