#include "MapClass.h"
#include "Mob.h"
#include "LiquidHolder.h"
#include "Turf.h"

#include "mob_position.h"

#include "NetClientImpl.h"
#include "EffectSystem.h"
#include "MoveEffect.h"
#include "sync_random.h"
#include "ItemFabric.h"
#include "MagicStrings.h"
#include "TileInt.h"

#include "sound.h"

void Manager::checkMove(Dir direct)
{
    //cautLastItem(direct);
    moveEach(direct);
};


void Manager::touchEach(Dir direct)
{
    thisMob->dMove = direct;
}

void Manager::moveEach(Dir direct)
{
    undoCenterMove(direct);
};

void Manager::undoCenterMove(Dir direct)
{
    //TODO
    for (int z = 0; z < sizeDmap; ++z)
        for(int x = std::max(0, castTo<CubeTile>(thisMob->GetOwner().ret_item())->posx() - sizeHsq); 
            x <= std::min(castTo<CubeTile>(thisMob->GetOwner().ret_item())->posx() + sizeHsq, sizeHmap - 1); x++)
        {
            for(int y = std::max(0, castTo<CubeTile>(thisMob->GetOwner().ret_item())->posy() - sizeWsq); 
                y <= std::min(castTo<CubeTile>(thisMob->GetOwner().ret_item())->posy() + sizeWsq, sizeWmap - 1); y++)
            {
                map->squares[x][y][z]->ForEach([&](id_ptr_on<IOnMapBase> item)
                {
                    Move* eff = getEffectOf<Move>();
                    eff->Init(TITLE_SIZE, direct, thisMob->pixSpeed, item);
                    eff->Start();
                });
            }
        }
};

void Manager::changeMob(id_ptr_on<IMob>& i)
{
    thisMob = i.ret_id();

    if (thisMob.valid())
    {
        thisMob->onMobControl = true;
        thisMob->thisMobControl = true;
        IMainItem::map->centerFromTo(castTo<CubeTile>(thisMob->GetOwner().ret_item())->posx(), 
            castTo<CubeTile>(thisMob->GetOwner().ret_item())->posy());
        thisMob->InitGUI();
    }

    SYSTEM_STREAM << "\nTHIS MOB CHANGE: " << thisMob.ret_id() << " ";
};

Manager::Manager(std::string adrs)
{
    adrs_ = adrs;
    auto_player_ = false;
    visiblePoint = new std::list<point>;
    isMove = false;
    done = 0;
    pause = false;
    last_fps = FPS_MAX;
    net_client = NetClient::Init(this);
};

void Manager::UpdateVisible() 
{
    visiblePoint->clear();
    visiblePoint = 
        map->losf.calculateVisisble(visiblePoint, 
            castTo<CubeTile>(thisMob->GetOwner().ret_item())->posx(), 
            castTo<CubeTile>(thisMob->GetOwner().ret_item())->posy(),
            castTo<CubeTile>(thisMob->GetOwner().ret_item())->posz());
}

sf::Sound* sound;
void Manager::process()
{
    sound = new sf::Sound;
    InitSound(sound, "click.ogx");
    map->numOfPathfind = 0;
    SDL_Color color = {255, 255, 255, 0};

    int delay = 0;
    int lastTimeFps = SDL_GetTicks();
    int lastTimeC   = SDL_GetTicks();
    fps = 0;
    bool process_in = false;
    while(done == 0)
    { 

        processInput();
        IMainItem::fabric->Sync();
        if(net_client->Ready() && !pause)
        {
            process_in = true;
            process_in_msg();
            MAIN_TICK++;
        }

        if(process_in && !pause)
        {
            numOfDeer = 0;
            IMainItem::fabric->foreachProcess();
        }
         
        if (!NODRAW)
        {
            map->Draw();
            FabricProcesser::Get()->process();
        }

      
        //checkMoveMob();
        if (!NODRAW)
            thisMob->processGUI();

        texts.Process();

        if((SDL_GetTicks() - lastTimeFps) >= 1000 && !pause)
        {
            UpdateVisible();

            if(!(fps > FPS_MAX - 10 && fps < FPS_MAX - 10))
            delay = (int)(1000.0 / FPS_MAX + delay - 1000.0 / fps);
            lastTimeFps = SDL_GetTicks();
            last_fps = fps;
            fps = 0;
          

            map->numOfPathfind = 0;
        }
        ++fps;
        if(delay > 0) 
            ;//SDL_Delay(delay);
        else 
            delay = 0;

        gl_screen->Swap();
        //++MAIN_TICK;
        process_in = false;
        if (net_client->Process() == false)
        {
            SYSTEM_STREAM << "Fail receive messages" << std::endl;
            SDL_Delay(10000);
            break;
        }
    }
    TTF_Quit();
    SDL_Quit();
};

void Manager::checkMoveMob()
{
};

#define SEND_KEY_MACRO(key) \
      if((auto_player_ && (rand() % 100 == 1)) || (!NODRAW && keys[key])) \
      { \
          if(MAIN_TICK - lastShoot >= 2) \
          { \
              Message msg; \
              msg.text = #key; \
              net_client->Send(msg); \
              lastShoot = (int)MAIN_TICK; \
          } \
      }

void Manager::processInput()
{
    static Uint8* keys;
    static int lastShoot = 0;
    if (!NODRAW)
    {
        SDL_Event event;    
        while (SDL_PollEvent(&event))
        { 
            if(event.type == SDL_QUIT) 
                done = 1; 
            if(event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_o) 
                    pause = !pause;
                if (event.key.keysym.sym == SDLK_F12)
                    ToogleAutoplay();
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                auto item = map->click(event.button.x, event.button.y);
                if (item.ret_id())
                    last_touch = item->name;
                sound->play();
            }
            if (event.type == SDL_VIDEORESIZE)
            {
                int max_scale = std::max((event.resize.w / 4), (event.resize.h / 3));

                int new_w = max_scale * 4;
                int new_h = max_scale * 3;
                gl_screen->ResetScreen(new_w, new_h, 32, SDL_OPENGL | SDL_RESIZABLE);
            }
        }

        SDL_PumpEvents();
        keys = SDL_GetKeyState(NULL);
        /*if(keys[SDLK_h])
        {
            int locatime = SDL_GetTicks();
            auto itr = map->squares[thisMob->posx][thisMob->posy].begin();
            int i = 0;
            while(itr != map->squares[thisMob->posx][thisMob->posy].end())
            {
                SYSTEM_STREAM << i <<": Level " << (*itr)->level;
                itr++;
                i++;
            };
            SYSTEM_STREAM << "Num item: " << i << " in " << (SDL_GetTicks() - locatime) * 1.0 / 1000 << " sec" << std::endl;
        }*/
        if(keys[SDLK_F5])
        {
            int locatime = SDL_GetTicks();
            IMainItem::fabric->saveMap("clientmap.map");
            SYSTEM_STREAM << "Map saved in "<< (SDL_GetTicks() - locatime) * 1.0 / 1000 << " second" << std::endl;
        }
        if(keys[SDLK_F6])
        {
            int locatime = SDL_GetTicks();
            IMainItem::fabric->clearMap();
            IMainItem::fabric->loadMap("clientmap.map");
            SYSTEM_STREAM << "Map load in " << (SDL_GetTicks() - locatime) * 1.0 / 1000 << " second" << std::endl;
        }
        if(keys[SDLK_h])
        {
            SYSTEM_STREAM << "World's hash: " << IMainItem::fabric->hash_all() << std::endl; 
        }
    }

    SEND_KEY_MACRO(SDLK_SPACE);
    SEND_KEY_MACRO(SDLK_UP);
    SEND_KEY_MACRO(SDLK_DOWN);
    SEND_KEY_MACRO(SDLK_LEFT);
    SEND_KEY_MACRO(SDLK_RIGHT);
    SEND_KEY_MACRO(SDLK_j);
    SEND_KEY_MACRO(SDLK_p);
    SEND_KEY_MACRO(SDLK_q);
    SEND_KEY_MACRO(SDLK_f);
    SEND_KEY_MACRO(SDLK_a);
    SEND_KEY_MACRO(SDLK_s);
    SEND_KEY_MACRO(SDLK_d);
    SEND_KEY_MACRO(SDLK_w);
    SEND_KEY_MACRO(SDLK_1);
    SEND_KEY_MACRO(SDLK_2);
    SEND_KEY_MACRO(SDLK_3);
    SEND_KEY_MACRO(SDLK_4);
    SEND_KEY_MACRO(SDLK_d);
    SEND_KEY_MACRO(SDLK_e);
    SEND_KEY_MACRO(SDLK_c);
};

void Manager::initWorld()
{
    mob_position::set_mng(this);

    tick_recv = 0;
    isMove = 0;
    Uint32 rmask, gmask, bmask, amask;
    SetMasks(&rmask, &gmask, &bmask, &amask);

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    { 
        SYSTEM_STREAM << "Unable to init SDL: " << SDL_GetError() << std::endl; 
        SDL_Delay(10000);
        return;
    }
    atexit(SDL_Quit);
    SYSTEM_STREAM << "Begin TTF init\n";
    SYSTEM_STREAM << TTF_Init() << " - return\n";
    SYSTEM_STREAM << " End TTF init\n";
    atexit(TTF_Quit);
    SYSTEM_STREAM << "Begin NET init\n";
    SYSTEM_STREAM << SDLNet_Init() << " - return\n";
    SYSTEM_STREAM << " End NET init\n";
    atexit(SDLNet_Quit);
    IMainItem::fabric = new ItemFabric;
    map = new MapMaster;
    SDL_WM_SetCaption("K&V", "K&V");
    if (!NODRAW)
        gl_screen = new Screen(sizeW, sizeH);



    map->screen = gl_screen;
    map->loManager = 0;
    IMainItem::map = map;
    IMainItem::mobMaster = this;
    
    map->mobi = this;

    srand(SDL_GetTicks());
    id_ptr_on<IMob> newmob;

    map->makeTiles();

    newmob = IMainItem::fabric->newItemOnMap<IMob>(hash("ork"), map->squares[sizeHmap / 2][sizeWmap / 2][1]);
    changeMob(newmob);

    auto tptr = IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("Teleportator"), map->squares[sizeHmap / 2][sizeWmap / 2][1]);
    SetCreator(tptr.ret_id());

    map->makeMap();
    thisMob->passable = 1;
    LiquidHolder::LoadReaction();

    LoginData data;
    data.who = newmob.ret_id();
    data.word_for_who = 1;
    net_client->Connect(adrs_, DEFAULT_PORT, data);

    texts["FPS"].SetUpdater
    ([this](std::string* str)
    {
        std::stringstream ss; 
        ss << last_fps; 
        ss >> *str;
    }).SetFreq(1000).SetSize(20);

    texts["LastTouch"].SetUpdater
    ([this](std::string* str)
    {
        *str = last_touch;
    }).SetFreq(20).SetPlace(0, 400).SetSize(35);

    
};

void Manager::loadIniFile()
{

};

void Manager::process_in_msg()
{
    Message msg;
    while (true)
    {
        net_client->Recv(&msg);
        if (msg.text == Net::NEXTTICK)
            return;
        
        id_ptr_on<IMessageReceiver> i;
        i = msg.to;

        if (i.valid())
            i->processGUImsg(msg);
        else
            SYSTEM_STREAM << "Wrong id accepted - " << msg.to << std::endl;
    }
}

size_t Manager::GetCreator() const 
{
    return creator_;
}

void Manager::SetCreator(size_t new_creator) 
{
    creator_ = new_creator;
}

bool Manager::isMobVisible(int posx, int posy)
{
    // TODO: matrix for fast check
    if (visiblePoint == nullptr)
        return false;
    for (auto it = visiblePoint->begin(); it != visiblePoint->end(); ++it)
        if(it->posx == posx && it->posy == posy)
            return true;
    return false;
}


