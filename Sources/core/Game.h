#pragma once

#include <list>

#include "objects/Mob.h"

#include "HelperStructers.h"

#include <QApplication>
#include <QKeyEvent>
#include <QString>
#include <QObject>
#include <QThread>

#include "FastStringstream.h"

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
public slots:
    void process();
    void endProcess();
signals:
    void sendMap(QString url, QByteArray data);
    void addSystemText(QString key, QString text);
    void insertHtmlIntoChat(QString html);
    void playMusic(QString name, float volume);
private:
    class SavesHolder
    {
    public:
        static const int DATA_LIFETIME_TICKS = 25;
        void PutSave(QByteArray data, int tick);
        QByteArray GetSaveFor(int tick);
        void ClearOldSaves();
    private:
        struct DataTick
        {
            DataTick() {}
            DataTick(QByteArray d, int t) : data(d), tick(t) {}
            QByteArray data;
            int tick;
        };

        QVector<DataTick> saves_;
    } saves_holder_;

    bool is_end_process_;

    std::list<point>* visible_points_;
    int lps_;
    float cpu_load_;
    std::string last_touch_;

    QString ping_id_;
    int ping_send_time_;
    int current_ping_;
    bool ping_send_is_requested_;
    bool process_in_;
    std::string adrs_;
    size_t creator_;
    bool auto_player_;

    QThread thread_;
};

Game& GetGame();
bool IsGameValid();
void SetGame(Game* game);
