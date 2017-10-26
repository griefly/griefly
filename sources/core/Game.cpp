#include "Game.h"

#include "KvAbort.h"

#include "Map.h"

#include "SynchronizedRandom.h"
#include "ObjectFactory.h"
#include "net/MagicStrings.h"
#include "objects/Tile.h"
#include "Params.h"

#include "objects/mobs/Mob.h"
#include "Names.h"
#include "objects/movable/Movable.h"
#include "objects/mobs/Human.h"
#include "objects/mobs/LoginMob.h"
#include "objects/Lobby.h"
#include "objects/PhysicsEngine.h"
#include "objects/SpawnPoints.h"
#include "objects/test/UnsyncGenerator.h"

#include "AutogenMetadata.h"

#include "Version.h"

#include "net/Network2.h"
#include "net/NetworkMessagesTypes.h"
#include "representation/Representation.h"

#include "representation/Screen.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QByteArray>
#include <QUuid>
#include <QElapsedTimer>
#include <QFileInfo>

using namespace kv;

Game::Game(Representation* representation)
    : representation_(representation)
{
    process_messages_ns_ = 0;
    foreach_process_ns_ = 0;
    force_process_ns_ = 0;
    atmos_process_ns_ = 0;
    deletion_process_ns_ = 0;
    update_visibility_ns_ = 0;
    frame_generation_ns_ = 0;

    auto_player_ = false;
    cpu_load_ = 0.0f;

    const int LOADS_AMOUNT = 60;
    cpu_loads_.resize(LOADS_AMOUNT);
    cpu_loads_id_ = 0;

    ping_id_ = "";

    current_ping_ = 0;

    ping_send_is_requested_ = true;

    is_end_process_ = false;

    current_connections_ = 0;

    const int MESSAGES_LOG_SIZE = 2 * 1024;
    messages_log_.resize(MESSAGES_LOG_SIZE);
    for (quint32 i = 0; i < messages_log_.size(); ++i)
    {
        messages_log_[i].type = 0;
        messages_log_[i].json.append("(empty)");
    }
    log_pos_ = 0;

    moveToThread(&thread_);
    connect(&thread_, &QThread::started, this, &Game::process);
}

Game::~Game()
{
    // Nothing
}

void Game::Process()
{
    QElapsedTimer fps_timer;
    fps_timer.start();
    process_in_ = false;

    QElapsedTimer ping_timer;
    ping_timer.start();

    QElapsedTimer cpu_timer;

    int cpu_consumed_ms = 0;

    while (true)
    {
        Network2::GetInstance().WaitForMessageAvailable();

        cpu_timer.start();

        QCoreApplication::processEvents(QEventLoop::AllEvents, 40);
        if (is_end_process_)
        {
            break;
        }

        ProcessInputMessages();

        const int ATMOS_OFTEN = 1;
        const int ATMOS_MOVE_OFTEN = 1;

        if (process_in_)
        {
            QElapsedTimer timer;

            world_->StartTick();
            for (const Message &message : messages_to_process_)
            {
                const QJsonObject object = Network2::ParseJson(message);
                world_->ProcessMessage({message.type, object});
            }
            messages_to_process_.clear();
            world_->FinishTick();

            GenerateFrame();
            frame_generation_ns_ += timer.nsecsElapsed();
            frame_generation_ns_ /= 2;
        }

        cpu_consumed_ms += cpu_timer.elapsed();

        qint64 fps_elapsed = fps_timer.elapsed();
        if (fps_elapsed >= 1000)
        {
            fps_timer.restart();
            cpu_load_ = ((cpu_consumed_ms * 1.0) / fps_elapsed) * 100;
            cpu_consumed_ms = 0;
            cpu_loads_[cpu_loads_id_] = cpu_load_;
            cpu_loads_id_ = (cpu_loads_id_ + 1) % cpu_loads_.size();
        }

        if (   ping_timer.elapsed() > 1000
            && ping_send_is_requested_)
        {
            ping_id_ = QUuid::createUuid().toString();
            Network2::GetInstance().SendPing(ping_id_);
            ping_timer.restart();
            ping_send_is_requested_ = false;
            ping_send_time_.restart();
        }

        process_in_ = false;
    }
    thread_.exit();
}

const QString ON_LOGIN_MESSAGE =
        "Welcome to Griefly! It is yet another space station remake, so if you are here then you probably"
        " already know how to play."
        " Just in case: arrows for movement, left mouse click for hand actions (hit, attack, take), chat for speaking."
        " Pull objects - ctrl + click, rotate objects - R + click, look at objects - shift + click."
        " Use prefix ooc in the chat if you would like to use the ooc channel (it is a global channel).\n";

void Game::WaitForExit()
{
    thread_.wait();
}

QVector<ObjectInfo>* id_ptr_id_table = nullptr;

void Game::InitWorld(int id, QString map_name)
{
    InitSettersForTypes();
    mob_ = id;

    qDebug() << "Begin choose map";
    if (map_name == "no_map")
    {
        if (!GetParamsHolder().GetParamBool("mapgen_name"))
        {
            kv::Abort("No mapgen param!");
        }

        const QString mapgen_name = GetParamsHolder().GetParam<QString>("mapgen_name");
        if (QFileInfo::exists(mapgen_name))
        {
            QFile file(mapgen_name);
            if (!file.open(QIODevice::ReadOnly))
            {
                kv::Abort(QString("Error open: %1").arg(mapgen_name));
            }

            QByteArray raw_data;
            while (file.bytesAvailable())
            {
                QByteArray local = file.readLine();
                if (local.size() < 1)
                {
                    break;
                }
                local = local.left(local.size() - 1);
                raw_data.append(local);
            }
            raw_data = QByteArray::fromHex(raw_data);
            qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

            // TODO: config
            world_ = kv::GetCoreInstance().CreateWorldFromMapgen(raw_data, id, {true});
        }
        else
        {
            kv::Abort(QString("Mapgen file does not exist: %1").arg(mapgen_name));
        }
    }
    else
    {
        qDebug() << "Begin load map";
        QElapsedTimer load_timer;
        load_timer.start();

        QByteArray map_data = Network2::GetInstance().GetMapData();

        if (map_data.length() == 0)
        {
            kv::Abort("An empty map received");
        }

        world_ = kv::GetCoreInstance().CreateWorldFromSave(map_data, id);

        qDebug() << "Map is loaded, " << load_timer.elapsed() << " ms";
    }

    emit insertHtmlIntoChat(ON_LOGIN_MESSAGE);

    thread_.start();
}

void Game::ProcessInputMessages()
{
    while (Network2::GetInstance().IsMessageAvailable())
    {
        Message msg = Network2::GetInstance().PopMessage();

        AddMessageToMessageLog(msg);

        if (msg.type == MessageType::NEW_TICK)
        {
            process_in_ = true;
            break;
        }
        if (msg.type == MessageType::MAP_UPLOAD)
        {
            QElapsedTimer timer;
            timer.start();
            QJsonObject obj = Network2::ParseJson(msg);
            QString map_url = obj["url_to_upload_map"].toString();

            QJsonValue tick_v = obj["tick"];
            int tick = tick_v.toVariant().toInt();

            qDebug() << "Map upload to " << map_url << ", tick " << tick;

            QByteArray data;

            qDebug() << "Map will be generated";

            serializer_.ResetIndex();
            world_->SaveWorld(&serializer_);
            data = QByteArray(serializer_.GetData(), serializer_.GetIndex());

            AddLastMessages(&data);
            AddBuildInfo(&data);

            qDebug() << " " << data.length();

            emit sendMap(map_url, data);
            qDebug() << "It took " << timer.elapsed() << "ms to send the map";
            continue;
        }

        if (msg.type == MessageType::REQUEST_HASH)
        {
            const quint32 hash = world_->Hash();
            const qint32 game_tick = world_->GetGameTick();

            Message msg;

            msg.type = MessageType::HASH_MESSAGE;
            msg.json.append(
                      "{\"hash\":"
                    + QString::number(hash)
                    + ",\"tick\":"
                    + QString::number(game_tick)
                    + "}");

            Network2::GetInstance().SendMsg(msg);

            continue;
        }

        if (msg.type == MessageType::PING)
        {
            QJsonObject obj = Network2::ParseJson(msg);

            QString ping_id = obj["ping_id"].toString();

            if (ping_id != ping_id_)
            {
                continue;
            }

            current_ping_ = ping_send_time_.elapsed();
            ping_send_is_requested_ = true;
            continue;
        }
        if (msg.type == MessageType::CURRENT_CONNECTIONS)
        {
            QJsonObject obj = Network2::ParseJson(msg);
            QJsonValue amount_v = obj["amount"];
            current_connections_ = amount_v.toVariant().toInt();
            continue;
        }

        if (msg.type == MessageType::CLIENT_IS_OUT_OF_SYNC)
        {
            emit insertHtmlIntoChat("The client is out of sync, so the server will drop the connection. Try to reconnect.");
            continue;
        }
        if (msg.type == MessageType::CLIENT_TOO_SLOW)
        {
            emit insertHtmlIntoChat("The client is too slow, so the server will drop the connection. Try to reconnect.");
            continue;
        }
        if (msg.type == MessageType::SERVER_IS_RESTARTING)
        {
            emit insertHtmlIntoChat("The server is restarting, so the connection will be dropped. Try to reconnect.");
            continue;
        }
        if (msg.type == MessageType::EXIT_SERVER)
        {
            emit insertHtmlIntoChat("The server is near to exit, so it will drop the connection. Try to reconnect.");
            continue;
        }  
        if (   msg.type == MessageType::ORDINARY
            || msg.type == MessageType::MOUSE_CLICK
            || msg.type == MessageType::MESSAGE
            || msg.type == MessageType::NEW_CLIENT
            || msg.type == MessageType::OOC_MESSAGE)
        {
            messages_to_process_.push_back(msg);
            continue;
        }

        qDebug() << "Unhandled message, type: " << msg.type << ", json: " << msg.json;
        // TODO: other stuff
    }
}

void Game::GenerateFrame()
{
    AppendSystemTexts();

    kv::GrowingFrame frame = representation_->GetGrowingFrame();
    world_->Represent({{mob_, &frame}});

    representation_->Swap();
}

void Game::AppendSystemTexts()
{
    kv::GrowingFrame frame = representation_->GetGrowingFrame();

    frame.Append(
        FrameData::TextEntry{"Main", QString("CPU load: %1%").arg(cpu_load_)});

    float sum = 0.0f;
    for (float load : cpu_loads_)
    {
        sum += load;
    }
    frame.Append(
        FrameData::TextEntry{"Main", QString("Average CPU load: %1%").arg(sum / cpu_loads_.size())});
    frame.Append(
        FrameData::TextEntry{"Main", QString("Players: %1").arg(current_connections_)});
    frame.Append(
        FrameData::TextEntry{"Main", QString("Ping: %1 ms").arg(current_ping_)});

    frame.Append(
        FrameData::TextEntry{
            "Performance",
            QString("Process messages: %1 ms").arg((process_messages_ns_ * 1.0) / 1000000.0)});

    frame.Append(
        FrameData::TextEntry{
            "Performance",
            QString("Process objects: %1 ms").arg((foreach_process_ns_ * 1.0) / 1000000.0)});
}

void Game::process()
{
    Process();
}

void Game::endProcess()
{
    is_end_process_ = true;
    qDebug() << "void Game::endProcess()";
}

void Game::generateUnsync()
{
    world_->PerformUnsync();
}

void Game::AddLastMessages(QByteArray* data)
{
    data->append('\n');
    for (int i = (log_pos_ + 1) % messages_log_.size();
             i != log_pos_;
             i = (i + 1) % messages_log_.size())
    {
        data->append(QByteArray::number(messages_log_[i].type) + " ");
        data->append(messages_log_[i].json + '\n');
    }
}

void Game::AddMessageToMessageLog(Message message)
{
    messages_log_[log_pos_] = message;
    log_pos_ = (log_pos_ + 1) % messages_log_.size();
}

void Game::AddBuildInfo(QByteArray* data)
{
    QString system_info("Build info: %1, Qt: %2");
    system_info = system_info.arg(GetBuildInfo()).arg(GetQtVersion());
    data->append(system_info);
}
