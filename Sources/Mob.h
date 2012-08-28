#ifndef MOB_H
#define MOB_H

#include <SDL.h>
#include <list>
#include <Poco/Mutex.h>

#include "OnMapInt.h"
#include "Testmob.h"
#include "Firebullet.h"
#include "MonstrClass.h"
#include "CAliveMob.h"
#include "CSmallItem.h"

#include "Screen.h"
#include "Text.h"

class NetClient;
class MapMaster;

struct point;

class Manager
{
public:
    CSprite* image;
    void move(int direct);
    void touchEach(Dir direct);//Say items that they must move
    void moveEach(Dir direct);
    void undoCenterMove(Dir direct);
    void cautLastItem(Dir direct);
    void checkMove(Dir direct);
    void cautFromTo(int fromPosx, int fromPosy, int toPosx, int toPosy){ SYSTEM_STREAM << "void Manager::cautFromTo - not exist\n";};
    void globalMove(int direct);
    void initWorld();
    void loadIniFile();
    void process();
    void checkMoveMob();
    //void processGUI();
    void processInput();

    bool isMobVisible(int posx, int posy);

    int threadPathfind(void* data);

    void changeMob(id_ptr_on<IMob>& i);
    Manager(Mode mode, std::string adrs);

    bool isMove;
    int numOfDeer;
    //main
    int done;
    bool pause;
    //SDL_Surface* screen;
    //SDL_Surface* back;
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
    Poco::Mutex tick_recvm;
    void process_in_msg();
    //
    id_ptr_on<IMob> thisMob;
    MapMaster* map;

    NetClient net_client;

    Poco::Mutex map_access;
    Mode GetMode(){return mode_;}

    TextPainter texts;

    Screen* gl_screen;
private:
    Mode mode_;
    std::string adrs_;
//    std::list<char*>* msg;
};

#endif
