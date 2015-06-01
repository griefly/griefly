#pragma once

#include <list>

#include "MobInt.h"

#include "HelperStructers.h"

#include "TextInput.h"
#include <QApplication>
#include <QKeyEvent>

class Manager
{
public:
    void move(int direct);
    void touchEach(Dir direct);//Say items that they must move
    void moveEach(Dir direct);
    void undoCenterMove(Dir direct);
    void cautLastItem(Dir direct);
    void checkMove(Dir direct);
    void globalMove(int direct);
    void initWorld();
    void loadIniFile();
    void process();
    void checkMoveMob();
    //void processGUI();
    void processInput();

    void ProcessClick(int mouse_x, int mouse_y);

    void HandleKeyboardDown(QKeyEvent* event);

    void ClearGUIZone();

    bool isMobVisible(int posx, int posy);

    int threadPathfind(void* data);

    Manager(std::string adrs);

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
