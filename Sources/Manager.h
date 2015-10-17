#pragma once

#include <list>

#include "MobInt.h"

#include "HelperStructers.h"

#include "TextInput.h"
#include <QApplication>
#include <QKeyEvent>
#include <QString>

class Manager
{
public:
    void move(int direct);
    void moveEach(Dir direct);
    void undoCenterMove(Dir direct);
    void cautLastItem(Dir direct);
    void checkMove(Dir direct);
    void globalMove(int direct);
    void initWorld(int id, std::string map_name);
    void loadIniFile();
    void process();
    void checkMoveMob();
    //void processGUI();
    void processInput();

    void ProcessClick(int mouse_x, int mouse_y);

    void HandleKeyboardDown(QKeyEvent* event);
    void HandleKeyboardUp(QKeyEvent* event);

    void ClearGUIZone();

    bool isMobVisible(int posx, int posy);

    int threadPathfind(void* data);

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

QApplication* GetQApp();
void SetQApp(QApplication* new_app);

QWidget* GetMainWidget();
void SetMainWidget(QWidget* widget);

Manager* GetManager();
void SetManager(Manager* manager);
