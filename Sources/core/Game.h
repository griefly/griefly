#pragma once

#include <list>

#include "objects/Mob.h"

#include "HelperStructers.h"

#include <QApplication>
#include <QKeyEvent>
#include <QString>
#include <QObject>
#include <QThread>

class Game: public QObject
{
    Q_OBJECT
public:
    void InitWorld(int id, std::string map_name);
    void Process();

    bool IsMobVisible(int posx, int posy);

    Game();

    void UpdateVisible();
    void ProcessInputMessages();
    void ToogleAutoplay() { auto_player_ = !auto_player_; }

    std::list<point>* GetVisiblePoints() { return visible_points_; }
public slots:
    void process();
private:
    std::list<point>* visible_points_;
    int fps_;
    int last_fps_;
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
void SetGame(Game* manager);
