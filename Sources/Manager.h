#pragma once

#include <list>

#include "Mob.h"

#include "HelperStructers.h"

#include "TextInput.h"
#include <QApplication>
#include <QKeyEvent>
#include <QString>

class Manager
{
public:
    void CheckMove(Dir direct);
    void InitWorld(int id, std::string map_name);
    void Process();
    void HandleInput();

    bool IsMobVisible(int posx, int posy);

    void ProcessClick(int mouse_x, int mouse_y);

    void HandleKeyboardDown(QKeyEvent* event);
    void HandleKeyboardUp(QKeyEvent* event);

    Manager();

    bool isMove;
    int numOfDeer;
    //main
    int done;
    bool pause;
    //SDL_Surface* screen;
    //SDL_Surface* back;
    void UpdateVisible();
    std::list<point>* visiblePoint;
    ///////////
    int fps;
    int last_fps;
    int delay;
    int lastTimeFps;
    int lastTimeC;
    std::string last_touch;
    SDL_Surface* sFPS;
    ///////////
    int tick_recv;
    void process_in_msg();

    void ToogleAutoplay() { auto_player_ = !auto_player_; }
private:
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
