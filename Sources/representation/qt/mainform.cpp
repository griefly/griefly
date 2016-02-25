#include "mainform.h"
#include "ui_mainform.h"

#include <iostream>

#include "core/Map.h"
#include "Debug.h"
#include "Params.h"
#include "core/Game.h"
#include "net/MagicStrings.h"

#include "representation/Sound.h"

#include "representation/Screen.h"
#include "representation/Metadata.h"

#include "qtopengl.h"

#include "representation/Chat.h"
#include "net/Network2.h"
#include "net/NetworkMessagesTypes.h"
#include "representation/Text.h"

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTime>
#include <QTextBlock>
#include <QMessageBox>

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);

    ui->textBrowser->setAcceptRichText(false);
    ui->textBrowser->setContextMenuPolicy(Qt::NoContextMenu);
    ui->textBrowser->setOpenLinks(false);
    ui->textBrowser->setReadOnly(true);
    ui->textBrowser->setUndoRedoEnabled(false);

    if (!GetParamsHolder().GetParamBool("-debug_to_chat"))
    {
        SetLogToFile();
    }

    {
        QList<int> sizes;
        sizes.push_back(512);
        sizes.push_back(256);
        ui->splitter->setSizes(sizes);
    }
    {
        QList<int> sizes;
        sizes.push_back(100);
        sizes.push_back(256);
        ui->splitterRight->setSizes(sizes);
    }


    setWindowTitle("Griefly");

    left_column = ui->leftColumn->width();
    right_column = ui->rightColumn->width();

    ui->widget->hide();

    connect(&Network2::GetInstance(), &Network2::connectionSuccess, this, &MainForm::startGameLoop);
    connect(&Network2::GetInstance(), &Network2::connectionFailed, this, &MainForm::connectionFailed);

    connect (&Network2::GetInstance(), &Network2::mapSendingStarted, this, &MainForm::uploadStarted);
    connect (&Network2::GetInstance(), &Network2::mapSendingFinished, this, &MainForm::uploadFinished);

    connect(this, &MainForm::autoConnect, this, &MainForm::on_lineEdit_returnPressed);
    connect(ui->widget, &QtOpenGL::enterPressed, this, &MainForm::setFocusOnLineEdit);
    connect(ui->widget, &QtOpenGL::f2Pressed, this, &MainForm::oocPrefixToLineEdit);

    connect(ui->lineEdit, &GamingLineEdit::keyToPass, ui->widget, &QtOpenGL::handlePassedKey);

    if (GetParamsHolder().GetParamBool("-auto_connect"))
    {
        activeTimer = new QTimer(this);
        activeTimer->setInterval(1 * 1000);
        activeTimer->setSingleShot(true);
        connect(activeTimer, &QTimer::timeout, this, &MainForm::helperAutoConnect);
        activeTimer->start();
    }

    map_sending_ = false;
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

void MainForm::addSystemText(QString key, QString text)
{
    texts_[key] = text;
    if (text == "")
    {
        texts_.remove(key);
    }

    ui->mainTabTextBrowser->clear();
    for (auto it = texts_.begin(); it != texts_.end(); ++it)
    {
        ui->mainTabTextBrowser->insertHtml(*it + "<br>");
    }
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
    {
        NODRAW = true;
    }

    if (GetParamsHolder().GetParamBool("name"))
    {
        Debug::SetUniqueName(GetParamsHolder().GetParam<std::string>("name"));
    }

    SetRepresentation(new Representation);

    Game* game = new Game;
    SetGame(game);

    connect(game, &Game::insertHtmlIntoChat, this, &MainForm::insertHtmlIntoChat);
    connect(game, &Game::addSystemText, this, &MainForm::addSystemText);
    connect(game, &Game::playMusic, this, &MainForm::playMusic);

    connect(game, &Game::sendMap, &Network2::GetInstance(), &Network2::sendMap);

    game->InitWorld(id, map.toStdString());

    connect(this, &MainForm::closing, game, &Game::endProcess);
    connect(this, &MainForm::generateUnsync, game, &Game::generateUnsync);

    /*if (GetParamsHolder().GetParamBool("-auto"))
    {
        man.ToogleAutoplay();
    }*/

    QTime fps_timer;
    fps_timer.start();
    int fps_counter = 0;
    while (true)
    {
        ++fps_counter;

        GetRepresentation().Process();
        GetScreen().Swap();

        if (isHidden())
        {
            break;
        }

        if (fps_timer.elapsed() > 1000)
        {
            addSystemText("FPS", "FPS: " + QString::number(fps_counter));
            fps_timer.restart();
            fps_counter = 0;
        }
    }

    emit closing();
    QCoreApplication::processEvents(QEventLoop::AllEvents);

    GetGame().WaitForExit();

    Network2::GetInstance().Disconnect();

    return;
}

void MainForm::connectionFailed(QString reason)
{
    insertHtmlIntoChat("<b>Connection failed!</b>");
    insertHtmlIntoChat(reason);
    insertHtmlIntoChat("<br>If you would like to reconnect then do it manually<br>");
}

void MainForm::insertHtmlIntoChat(QString html)
{
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(cursor);

    cursor.insertHtml(html);
    cursor.insertBlock();

    const int MIN_TEXT_BLOCKS = 3;
    const int MAX_SIZE_OF_DOCUMENT = 1500.0;

    while (   (ui->textBrowser->document()->blockCount() > MIN_TEXT_BLOCKS)
           && (ui->textBrowser->document()->size().height() > MAX_SIZE_OF_DOCUMENT))
    {
        RemoveFirstBlockFromTextEditor();
    }

    cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(cursor);
}

void MainForm::RemoveFirstBlockFromTextEditor()
{
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.removeSelectedText();
    cursor.select(QTextCursor::BlockUnderCursor);
    while (cursor.selectedText().length() == 0)
    {
        cursor.deleteChar();
        cursor.select(QTextCursor::BlockUnderCursor);
    }
}

void MainForm::playMusic(QString name, float volume)
{
    qDebug() << name << " " << volume;
    if (name != "")
    {
        GetSoundPlayer().PlayMusic(name.toStdString(), volume);
    }
    else
    {
        GetSoundPlayer().StopMusic();
    }
}

void MainForm::oocPrefixToLineEdit()
{
    ui->lineEdit->setText("OOC ");
}

void MainForm::uploadStarted()
{
    map_sending_ = true;
}

void MainForm::uploadFinished()
{
    map_sending_ = false;
}

void MainForm::closeEvent(QCloseEvent* event)
{
    const int WAIT_MAP_SENDING = 10 * 1000;
    if (map_sending_)
    {
        if (!close_request_timer_.isValid())
        {
            close_request_timer_.start();
        }
        QMessageBox::StandardButton answer;
        if (close_request_timer_.elapsed() < WAIT_MAP_SENDING)
        {
            answer = QMessageBox::information(
                this, "Griefly", "Wait a little bit (about 10 seconds)...");
        }
        else
        {
            answer = QMessageBox::question(
                this, "Griefly", "Are you sure? Something still in the process...",
                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                QMessageBox::No);
        }
        if (answer != QMessageBox::Yes)
        {
            event->ignore();
            return;
        }
    }
    event->accept();
}

void MainForm::ConnectToHost()
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
        ConnectToHost();
        return;
    }

    if (!connected)
    {
        return;
    }

    if (ui->lineEdit->text() == "/generate_unsync")
    {
        qDebug() << "Unsync signal will be sended to the core...";
        ui->lineEdit->clear();
        emit generateUnsync();
        return;
    }
    if (ui->lineEdit->text() == "/restart_round")
    {
        qDebug() << "Restart round message will be sended to the server...";
        ui->lineEdit->clear();
        Message2 msg;
        msg.type = MessageType::RESTART_ROUND;
        Network2::GetInstance().SendMsg(msg);
        return;
    }
    Message2 msg;

    QString text = ui->lineEdit->text();
    if (text.length() == 0)
    {
        return;
    }

    QJsonObject object;

    msg.type = MessageType::MESSAGE;
    if (Chat::IsOOCMessage(text.toStdString()))
    {
        msg.type = MessageType::OOC_MESSAGE;
        object["login"] = QString("");
        object["text"] = ui->lineEdit->text().mid(3).trimmed();
    }
    else
    {
        object["text"] = ui->lineEdit->text();
    }

    if (object["text"].toString().length() == 0)
    {
        return;
    }

    QJsonDocument doc(object);
    msg.json = doc.toJson();

    Network2::GetInstance().SendMsg(msg);

    ui->lineEdit->clear();

    //ui->widget->setFocus();
}

void MainForm::on_splitter_splitterMoved(int pos, int index)
{
    left_column = ui->leftColumn->width();
    right_column = ui->rightColumn->width();

    int min_size = std::min(ui->leftColumn->width(), ui->leftColumn->height());
    ui->widget->resize(min_size, min_size);

    ui->splitterRight->resize(ui->rightColumn->width(), ui->rightColumn->height());

    if (IsScreenValid())
    {
        GetScreen().PerformSizeUpdate();
        GetScreen().Clear();
        GetScreen().Swap();
    }

    int x_pos = (ui->leftColumn->width() - min_size) / 2;
    int y_pos = (ui->leftColumn->height() - min_size) / 2;
    ui->widget->move(x_pos, y_pos);
}
