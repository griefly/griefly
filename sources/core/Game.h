#pragma once

#include <list>

#include "objects/Mob.h"
#include "objects/test/UnsyncGenerator.h"

#include "SyncRandom.h"
#include "Names.h"
#include "WorldLoaderSaver.h"

#include <QApplication>
#include <QKeyEvent>
#include <QString>
#include <QObject>
#include <QThread>

#include "Interfaces.h"

class TextPainter;

class Game : public QObject, public GameInterface
{
    Q_OBJECT
public:
    void WaitForExit();
    void InitWorld(int id, QString map_name);

    Game();
    ~Game();

    virtual void MakeTiles(int size_x, int size_y, int size_z) override;

    virtual void PlayMusic(const QString& name, float volume) override;
    virtual void AddSound(const QString& name) override;

    virtual AtmosInterface& GetAtmosphere() override;
    virtual MapInterface& GetMap() override;
    virtual const MapInterface& GetMap() const override;
    virtual ObjectFactoryInterface& GetFactory() override;
    virtual ChatInterface& GetChat() override;
    virtual TextPainter& GetTexts() override;
    virtual SyncRandom& GetRandom() override;
    virtual Names& GetNames() override;

    virtual void SetUnsyncGenerator(quint32 generator) override;
    virtual void ChangeMob(IdPtr<IMob> new_mob) override;
    virtual IdPtr<IMob> GetMob() override;
    virtual void SetMob(quint32 new_mob) override;

    virtual void SetPlayerId(quint32 net_id, quint32 real_id) override;
    virtual quint32 GetPlayerId(quint32 net_id) override;
    virtual quint32 GetNetId(quint32 real_id) override;
    virtual const std::map<quint32, quint32>& GetPlayersTable() const override;
public slots:
    void process();
    void endProcess();
    void generateUnsync();
signals:    
    void sendMap(QString url, QByteArray data);
    void addSystemText(QString key, QString text);
    void insertHtmlIntoChat(QString html);
    void playMusic(QString name, float volume);
private:
    IdPtr<UnsyncGenerator> GetUnsyncGenerator();
    void GenerateFrame();

    void UpdateVisible();
    void ProcessInputMessages();
    void InitGlobalObjects();
    void Process();

    void AddLastMessages(QByteArray* data);
    void AddMessageToMessageLog(Message2 message);

    void AddBuildInfo(QByteArray* data);

    FastSerializer serializer_;

    std::vector<Message2> messages_log_;
    int log_pos_;

    bool is_end_process_;

    float cpu_load_;

    std::vector<float> cpu_loads_;
    int cpu_loads_id_;

    QString last_touch_;

    std::vector<Message2> messages_to_process_;
    void ProcessBroadcastedMessages();
    void CheckMessagesOrderCorrectness();
    //bool hash_

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

    AtmosInterface* atmos_;
    MapInterface* map_;
    ObjectFactoryInterface* factory_;
    ChatInterface* chat_;
    TextPainter* texts_;

    SyncRandom* sync_random_;
    Names* names_;
    WorldLoaderSaver* world_loader_saver_;

    IdPtr<UnsyncGenerator> unsync_generator_;
    IdPtr<IMob> current_mob_;

    std::map<quint32, quint32> players_table_;
};
