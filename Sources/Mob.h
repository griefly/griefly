#ifndef MOB_H
#define MOB_H

#include <SDL.h>
#include <list>

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
    Manager(std::string adrs);

    size_t GetCreator() const;
    void SetCreator(size_t new_creator);

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
    //
    id_ptr_on<IMob> thisMob;
    MapMaster* map;

    INetClient* net_client;

    TextPainter texts;

    void ToogleAutoplay() { auto_player_ = !auto_player_; }
private:
    std::string adrs_;

    size_t creator_;

    bool auto_player_;
};

#endif
