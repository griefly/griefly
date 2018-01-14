#include "mainform.h"
#include "ui_mainform.h"

#include <QMap>

#include "Params.h"
#include "net/MagicStrings.h"

#include "Game.h"

#include "representation/Sound.h"

#include "representation/Screen.h"
#include "representation/Metadata.h"
#include "representation/Representation.h"

#include "qtopengl.h"

#include "net/Network2.h"
#include "core_headers/Messages.h"
#include "core_headers/CoreInterface.h"

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTime>
#include <QTextBlock>
#include <QMessageBox>

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm),
    fps_cap_(-1),
    current_fps_(0),
    represent_max_ms_(0)
{
    ui->setupUi(this);

    left_column_ = 512;
    right_column_ = 256;

    setWindowTitle(QString("Griefly %1").arg(kv::GetCoreInstance().GetGameVersion()));

    // Without GL widget looks weird
    ui->widget->hide();

    connect(&Network2::GetInstance(), &Network2::connectionSuccess,
            this, &MainForm::startGameLoop);
    connect(&Network2::GetInstance(), &Network2::connectionFailed,
            this, &MainForm::connectionFailed);

    connect(&Network2::GetInstance(), &Network2::mapSendingStarted,
            this, &MainForm::uploadStarted);
    connect(&Network2::GetInstance(), &Network2::mapSendingFinished,
            this, &MainForm::uploadFinished);

    connect(ui->widget, &QtOpenGL::enterPressed,
            this, &MainForm::setFocusOnLineEdit);
    connect(ui->widget, &QtOpenGL::f2Pressed,
            this, &MainForm::oocPrefixToLineEdit);

    connect(ui->command_line_edit, &GamingLineEdit::keyToPass, ui->widget, &QtOpenGL::handlePassedKey);

    QTimer::singleShot(0, Qt::PreciseTimer, this, SLOT(connectToHost()));

    map_sending_ = false;
}

void MainForm::setFocusOnLineEdit()
{
    ui->command_line_edit->setFocus();
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::addSytemTextToTab(const QString& tab, const QString& text)
{
    if (texts_.find(tab) == texts_.end())
    {
        QWidget* new_tab = new QWidget;
        auto new_grid_layout = new QGridLayout(new_tab);
        auto new_text_edit = new QTextEdit(new_tab);
        new_text_edit->setUndoRedoEnabled(false);
        new_text_edit->setReadOnly(true);

        new_grid_layout->addWidget(new_text_edit, 0, 0, 1, 1);

        texts_.insert(tab, new_text_edit);

        const int index = ui->texts_tabs->count();
        ui->texts_tabs->insertTab(index, new_tab, tab);
    }

    QTextEdit* text_edit = texts_[tab];
    text_edit->insertHtml(QString("%1<br>").arg(text));
}

void MainForm::clearSystemTexts()
{
    for (const auto& text_edit : qAsConst(texts_))
    {
        text_edit->clear();
    }
}

void MainForm::removeEmptyTabs()
{
    QVector<QString> to_remove;
    for (const auto& text_edit : qAsConst(texts_))
    {
        if (text_edit->document()->isEmpty())
        {
            delete text_edit->parent();
            to_remove.append(texts_.key(text_edit));
        }
    }
    for (const auto& removed : qAsConst(to_remove))
    {
        texts_.remove(removed);
    }
}

void MainForm::resizeEvent(QResizeEvent*)
{
    const int left = (width() * left_column_) / (left_column_ + right_column_);
    const int right = (width() * right_column_) / (left_column_ + right_column_);
    ui->splitter->setSizes({left, right});

    on_splitter_splitterMoved(0, 0);
}

void MainForm::startGameLoop(int id, QString map)
{
    ui->widget->show();
    on_splitter_splitterMoved(0, 0);

    if (GetParamsHolder().GetParamBool("-max_fps"))
    {
        fps_cap_ = GetParamsHolder().GetParam<int>("-max_fps");
    }

    representation_ = new Representation;
    ui->widget->SetRepresentation(representation_);
    connect(ui->widget, &QtOpenGL::focusLost, this,
    [this]()
    {
        representation_->ResetKeysState();
    });

    Game* game = new Game(representation_);

    connect(game, &Game::insertHtmlIntoChat,
            this, &MainForm::insertHtmlIntoChat);
    connect(representation_, &Representation::clearSystemTexts,
            this, &MainForm::clearSystemTexts);
    connect(representation_, &Representation::systemText,
            this, &MainForm::addSytemTextToTab);
    connect(representation_, &Representation::removeEmptyTabs,
            this, &MainForm::removeEmptyTabs);

    connect(game, &Game::sendMap,
            &Network2::GetInstance(), &Network2::sendMap);
    connect(representation_, &Representation::chatMessage, this, &MainForm::insertHtmlIntoChat);

    game->InitWorld(static_cast<quint32>(id), map, {true});

    connect(this, &MainForm::closing, game, &Game::endProcess);
    connect(this, &MainForm::generateUnsync, game, &Game::generateUnsync);

    QElapsedTimer fps_timer;
    fps_timer.start();

    int fps_counter = 0;

    QElapsedTimer process_performance;
    qint64 max_process_time = 0;

    int time_per_frame_ms;
    if (fps_cap_ <= 0)
    {
        time_per_frame_ms = 0;
    }
    else
    {
        time_per_frame_ms = 1000 / fps_cap_;
    }
    while (true)
    {
        process_performance.start();
        representation_->HandleInput();
        GetGLWidget()->updateGL();
        ++fps_counter;
        qint64 process_time = process_performance.nsecsElapsed();
        if (process_time > max_process_time)
        {
            max_process_time = process_time;
        }
        int sleep_time = time_per_frame_ms - process_time / 1e6;
        if (sleep_time > 0)
        {
            QThread::msleep(sleep_time);
        }
        if (isHidden())
        {
            break;
        }

        if (fps_timer.elapsed() > 1000)
        {
            current_fps_ = fps_counter;
            represent_max_ms_ = max_process_time / 1e6;

            AddSystemTexts();

            representation_->ResetPerformance();
            max_process_time = 0;
            fps_timer.restart();
            fps_counter = 0;
        }
    }

    emit closing();
    QCoreApplication::processEvents(QEventLoop::AllEvents);

    game->WaitForExit();

    Network2::GetInstance().Disconnect();

    return;
}

void MainForm::connectionFailed(QString reason)
{
    insertHtmlIntoChat("<b>Connection failed!</b>");
    insertHtmlIntoChat(reason);
    insertHtmlIntoChat("<br>If you would like to reconnect then do it manually (close the game window and start the game again)<br>");
}

void MainForm::insertHtmlIntoChat(QString html)
{
    QTextCursor cursor = ui->chat_text_edit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->chat_text_edit->setTextCursor(cursor);

    cursor.insertHtml(html);
    cursor.insertBlock();

    const int MIN_TEXT_BLOCKS = 3;
    const int MAX_SIZE_OF_DOCUMENT = 1500.0;

    while (   (ui->chat_text_edit->document()->blockCount() > MIN_TEXT_BLOCKS)
           && (ui->chat_text_edit->document()->size().height() > MAX_SIZE_OF_DOCUMENT))
    {
        RemoveFirstBlockFromTextEditor();
    }

    cursor = ui->chat_text_edit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->chat_text_edit->setTextCursor(cursor);
}

void MainForm::RemoveFirstBlockFromTextEditor()
{
    QTextCursor cursor = ui->chat_text_edit->textCursor();
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

void MainForm::oocPrefixToLineEdit()
{
    ui->command_line_edit->setText("OOC ");
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

void MainForm::connectToHost()
{
    QString adrs = "127.0.0.1";
    if (GetParamsHolder().GetParamBool("ip"))
    {
        adrs = GetParamsHolder().GetParam<QString>("ip");
    }

    int port = 1111;
    if (GetParamsHolder().GetParamBool("port"))
    {
        port = GetParamsHolder().GetParam<int>("port");
    }

    QString login = "Guest";
    if (GetParamsHolder().GetParamBool("login"))
    {
        login = GetParamsHolder().GetParam<QString>("login");
    }

    QString password = "";
    if (GetParamsHolder().GetParamBool("password"))
    {
        password = GetParamsHolder().GetParam<QString>("password");
    }

    Network2::GetInstance().TryConnect(adrs, port, login, password);
}

void MainForm::AddSystemTexts()
{
    ui->client_text_edit->clear();

    ui->client_text_edit->insertHtml(QString("FPS: %1<br>").arg(current_fps_));
    ui->client_text_edit->insertHtml(QString("Represent max: %1 ms<br>").arg(represent_max_ms_));

    const qint64 mutex_ns = representation_->GetPerformance().mutex_ns;
    ui->client_text_edit->insertHtml(QString("Represent mutex lock max: %1 ms").arg(mutex_ns / 1e6));
}

bool IsOOCMessage(const QString& text)
{
    if (    text.length() >= 3
        && (   (text.left(3) == "OOC")
            || (text.left(3) == "ooc"))
        )
    {
        return true;
    }
    return false;
}

void MainForm::on_command_line_edit_returnPressed()
{
    QString text = ui->command_line_edit->text();
    ui->command_line_edit->clear();
    if (text.length() == 0)
    {
        return;
    }

    if (text == "/generate_unsync")
    {
        qDebug() << "Unsync signal will be sended to the core...";
        emit generateUnsync();
        return;
    }
    if (text == "/restart_round")
    {
        qDebug() << "Restart round message will be sended to the server...";
        kv::Message message;
        message.type = kv::message_type::RESTART_ROUND;
        Network2::GetInstance().Send(message);
        return;
    }
    if (text.startsWith("/next_tick"))
    {
        QStringList list = text.split(" ");
        int count = 1;
        if (list.size() >= 2)
        {
            QString number = list.at(1);
            bool ok = false;
            count = number.toInt(&ok);
            if (!ok)
            {
                count = 1;
            }
        }
        qDebug() <<
            QString("%1 nexttick messages will be sended to the server...")
                .arg(count);
        kv::Message message;
        message.type = kv::message_type::NEXT_TICK;
        for (int i = 0; i < count; ++i)
        {
            Network2::GetInstance().Send(message);
        }
        return;
    }

    kv::Message message;
    message.type = kv::message_type::MESSAGE;
    QJsonObject object;
    if (IsOOCMessage(text))
    {
        message.type = kv::message_type::OOC_MESSAGE;
        object["login"] = QString("");
        object["text"] = text.mid(3).trimmed();
    }
    else
    {
        object["text"] = text;
    }

    if (object["text"].toString().length() == 0)
    {
        return;
    }

    message.data = object;

    Network2::GetInstance().Send(message);
}

void MainForm::on_splitter_splitterMoved(int, int)
{
    left_column_ = ui->left_column->width();
    right_column_ = ui->right_column->width();

    int min_size = qMin(ui->left_column->width(), ui->left_column->height());
    ui->widget->resize(min_size, min_size);

    ui->splitter_right->resize(ui->right_column->width(), ui->right_column->height());

    if (IsScreenValid())
    {
        GetScreen().PerformSizeUpdate();
    }

    int x_pos = (ui->left_column->width() - min_size) / 2;
    int y_pos = (ui->left_column->height() - min_size) / 2;
    ui->widget->move(x_pos, y_pos);
}
