#pragma once

#include <list>

#include "objects/Mob.h"

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

    bool IsMobVisible(int posx, int posy);

    Game();

    void UpdateVisible();
    void ProcessInputMessages();
    void ToogleAutoplay() { auto_player_ = !auto_player_; }

    std::list<point>* GetVisiblePoints() { return visible_points_; }
    void PlayMusic(std::string name, float volume);

    MapMaster& GetMap();
    ObjectFactory& GetFactory();
    Chat& GetChat();
    TextPainter& GetTexts();
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

    std::list<point>* visible_points_;
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
};
