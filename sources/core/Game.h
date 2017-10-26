#pragma once

#include <list>

#include "objects/mobs/Mob.h"
#include "objects/test/UnsyncGenerator.h"
#include "objects/GlobalObjectsHolder.h"

#include "SynchronizedRandom.h"
#include "Names.h"
#include "WorldLoaderSaver.h"
#include "ChatFrameInfo.h"

#include <QApplication>
#include <QKeyEvent>
#include <QString>
#include <QObject>
#include <QThread>
#include <QElapsedTimer>

#include "Interfaces.h"
#include "core_headers/CoreInterface.h"

class Game : public QObject
{
    Q_OBJECT
public:
    void WaitForExit();
    void InitWorld(int id, QString map_name);

    Game(Representation* representation);
    ~Game();
public slots:
    void process();
    void endProcess();
    void generateUnsync();
signals:    
    void sendMap(QString url, QByteArray data);
    void insertHtmlIntoChat(QString html);
private:
    void GenerateFrame();
    void AppendSystemTexts();
    void ProcessInputMessages();
    void Process();

    void AddLastMessages(QByteArray* data);
    void AddMessageToMessageLog(Message message);

    void AddBuildInfo(QByteArray* data);

    kv::FastSerializer serializer_;

    std::vector<Message> messages_to_process_;
    std::vector<Message> messages_log_;
    int log_pos_;

    bool is_end_process_;

    float cpu_load_;

    std::vector<float> cpu_loads_;
    int cpu_loads_id_;

    QString ping_id_;
    QElapsedTimer ping_send_time_;
    int current_ping_;
    bool ping_send_is_requested_;
    bool process_in_;
    QString adrs_;
    quint32 creator_;
    bool auto_player_;

    int current_connections_;

    // Perfomance
    qint64 process_messages_ns_;
    qint64 foreach_process_ns_;
    qint64 force_process_ns_;
    qint64 atmos_process_ns_;
    qint64 deletion_process_ns_;
    qint64 update_visibility_ns_;
    qint64 frame_generation_ns_;

    QThread thread_;

    int mob_;
    kv::CoreInterface::WorldPtr world_;

    Representation* representation_;
};
