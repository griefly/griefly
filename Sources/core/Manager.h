#pragma once

#include <list>

#include "objects/Mob.h"

#include "HelperStructers.h"

#include <QApplication>
#include <QKeyEvent>
#include <QString>

class Manager
{
public:
    void InitWorld(int id, std::string map_name);
    void Process();

    bool IsMobVisible(int posx, int posy);

    void HandleKeyboardDown(QKeyEvent* event);
    void HandleKeyboardUp(QKeyEvent* event);

    Manager();

    void UpdateVisible();
    void ProcessInputMessages();
    void ToogleAutoplay() { auto_player_ = !auto_player_; }

    std::list<point>* GetVisiblePoints() { return visible_points_; }
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
};

QWidget& GetMainWidget();
void SetMainWidget(QWidget* widget);

Manager& GetManager();
bool IsManagerValid();
void SetManager(Manager* manager);
