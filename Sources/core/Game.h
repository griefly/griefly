#pragma once

#include <list>

#include "objects/Mob.h"
#include "objects/UnsyncGenerator.h"

#include "SyncRandom.h"
#include "Names.h"

#include <QApplication>
#include <QKeyEvent>
#include <QString>
#include <QObject>
#include <QThread>

#include "FastStringstream.h"

class ObjectFactory;
class Chat;
class TextPainter;

class Game: public QObject
{
    Q_OBJECT
public:
    void WaitForExit();

    void InitWorld(int id, std::string map_name);
    void Process();

    Game();
    ~Game();

    void InitGlobalObjects();

    void MakeTiles(int size_x, int size_y, int size_z);

    void UpdateVisible();
    void ProcessInputMessages();
    void ToogleAutoplay() { auto_player_ = !auto_player_; }

    void GenerateFrame();

    void PlayMusic(std::string name, float volume);

    MapMaster& GetMap();
    const MapMaster& GetMap() const;
    ObjectFactory& GetFactory();
    Chat& GetChat();
    TextPainter& GetTexts();
    SyncRandom& GetRandom();
    Names& GetNames();

    void SetUnsyncGenerator(id_ptr_on<UnsyncGenerator> generator);
    id_ptr_on<UnsyncGenerator> GetUnsyncGenerator();

    void ChangeMob(id_ptr_on<IMob> new_mob);
    id_ptr_on<IMob> GetMob();
    void SetMob(size_t new_mob);
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
    void AddLastMessages(std::stringstream& stream);
    void AddMessageToMessageLog(Message2 message);

    std::vector<Message2> messages_log_;
    int log_pos_;

    bool is_end_process_;

    int lps_;
    float cpu_load_;
    std::string last_touch_;

    std::vector<Message2> messages_to_process_;
    void ProcessBroadcastedMessages();
    void CheckMessagesOrderCorrectness();
    //bool hash_

    QString ping_id_;
    QElapsedTimer ping_send_time_;
    int current_ping_;
    bool ping_send_is_requested_;
    bool process_in_;
    std::string adrs_;
    size_t creator_;
    bool auto_player_;

    int current_connections_;

    QThread thread_;

    MapMaster* map_;
    ObjectFactory* factory_;
    Chat* chat_;
    TextPainter* texts_;

    SyncRandom* sync_random_;
    Names* names_;

    id_ptr_on<UnsyncGenerator> unsync_generator_;
    id_ptr_on<IMob> current_mob_;
};
