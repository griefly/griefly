#include "MapClass.h"
#include "Mob.h"
#include <SDL_thread.h>
#include "LiquidHolder.h"
#include "Turf.h"

#include "NetClass.h"
#include "SyncQueue.h"
#include "EffectSystem.h"
#include "MoveEffect.h"
#include "sync_random.h"
#include "ItemFabric.h"

void Manager::checkMove(Dir direct)
{
    //cautLastItem(direct);
    moveEach(direct);
};


void Manager::touchEach(Dir direct)
{
    IMainItem::mob->dMove = direct;
}

void Manager::moveEach(Dir direct)
{
    switch(direct)
    {
    case D_UP:
        for(int x = 0; x < sizeHmap; x++)
        {
            for(int y = 0; y < sizeWmap; y++)
            {
                auto itr = map->squares[x][y].begin();
                while(itr != map->squares[x][y].end())
                    (*itr++)->y += TITLE_SIZE;
            }
        }
        break;
    case D_DOWN:
        for(int x = 0; x < sizeHmap; x++)
        {
            for(int y = 0; y < sizeWmap; y++)
            {
                auto itr = map->squares[x][y].begin();
                while(itr != map->squares[x][y].end())
                    (*itr++)->y -= TITLE_SIZE;
            }
        }
        break;
    case D_LEFT:
        for(int x = 0; x < sizeHmap; x++)
        {
            for(int y = 0; y < sizeWmap; y++)
            {
                auto itr = map->squares[x][y].begin();
                while(itr != map->squares[x][y].end())
                    (*itr++)->x += TITLE_SIZE;
            }
        }
        break;
    case D_RIGHT:
        for(int x = 0; x < sizeHmap; x++)
        {
            for(int y = 0; y < sizeWmap; y++)
            {
                auto itr = map->squares[x][y].begin();
                while(itr != map->squares[x][y].end())
                    (*itr++)->x -= TITLE_SIZE;
            }
        }
        break;
    }
    undoCenterMove(direct);
};

void Manager::undoCenterMove(Dir direct)
{
    for(int x = max(0, thisMob->posx - sizeHsq); x <= min(thisMob->posx + sizeHsq, sizeHmap - 1); x++)
    {
        for(int y = max(0, thisMob->posy - sizeWsq); y <= min(thisMob->posy + sizeWsq, sizeWmap - 1); y++)
        {
            auto itr = map->squares[x][y].begin();
            while(itr != map->squares[x][y].end())
            {
                Move* eff = getEffectOf<Move>();
                eff->Init(TITLE_SIZE, direct, thisMob->pixSpeed, *itr);
                eff->Start();
                ++itr;
            }
        }
    }
};

/*void Manager::cautLastItem(Dir direct)
{
    int yl;
    int xl;
    switch(direct)
    {
    case D_UP:
        yl = min((thisMob->posy) + sizeWsq + 1, sizeWmap - 1);
        for(xl = max(0, thisMob->posx - sizeHsq); xl <= min(thisMob->posx + sizeHsq, sizeHmap - 1); xl++)
        {
            auto itr = map->squares[xl][yl].begin();
            while(itr != map->squares[xl][yl].end())
            {
                (*itr++)->nowIsntVisible();
            }
        }
        break;
    case D_DOWN:
        yl = max(0, thisMob->posy - sizeWsq - 1);
        for(xl = max(0, thisMob->posx - sizeHsq); xl <= min(thisMob->posx + sizeHsq, sizeHmap - 1); xl++)
        {
            auto itr = map->squares[xl][yl].begin();
            while(itr != map->squares[xl][yl].end())
            {
                (*itr++)->nowIsntVisible();
            }
        }
        break;
    case D_LEFT:
        xl = min(thisMob->posx + sizeHsq + 1, sizeHmap - 1);
        for(yl = max(0, thisMob->posy - sizeWsq); yl <= min(thisMob->posy + sizeWsq, sizeWmap - 1); yl++)
        {
            auto itr = map->squares[xl][yl].begin();
            while(itr != map->squares[xl][yl].end())
            {
                (*itr++)->nowIsntVisible();
            }
        }
        break;
    case D_RIGHT:
        xl = max(0, thisMob->posx - sizeHsq - 1);
        for(yl = max(0, thisMob->posy - sizeWsq); yl <= min(thisMob->posy + sizeWsq, sizeWmap - 1); yl++)
        {
            auto itr = map->squares[xl][yl].begin();
            while(itr != map->squares[xl][yl].end())
            {
                (*itr++)->nowIsntVisible();
            }
        }
        break;
    }
};*/

void Manager::changeMob(id_ptr_on<IMob>& i)
{
    int oldposx = beginMobPosX, oldposy = beginMobPosY;
    if(IMainItem::mob)
    {
        oldposx = IMainItem::mob->posx;
        oldposy = IMainItem::mob->posy;
        delete IMainItem::mob;
    }
    IMainItem::mob = castTo<IMob>(ItemFabric::newVoidItemSaved(i->T_ITEM()));//TODO: repair //REPAIR WHAT?!

    thisMob = i.ret_id();
    thisMob->onMobControl = true;
    thisMob->thisMobControl = true;
    *IMainItem::mob = **thisMob;
    std::stringstream thisstr;
    IMainItem::map->centerFromTo(oldposx, oldposy, thisMob->posx, thisMob->posy);

    i->InitGUI();

    SYSTEM_STREAM << "\nTHIS MOB CHANGE: " << thisMob.ret_id() << " ";
};

Manager::Manager(Mode mode, std::string adrs)
{
    mode_ = mode;
    adrs_ = adrs;
    visiblePoint = new std::list<point>;
    isMove = false;
    done = 0;
    pause = 0;
    last_fps = FPS_MAX;
};
 
void Manager::process()
{

    map->numOfPathfind = 0;
    SDL_Color color = {255, 255, 255, 0};
    if (!NODRAW)
        sFPS = TTF_RenderText_Blended( map->aSpr.font, " ", color);
    int delay = 0;
    int lastTimeFps = SDL_GetTicks();
    int lastTimeC = SDL_GetTicks();
    fps = 0;
    bool process_in = false;
    while(done == 0)
    { 
      map_access.lock();
        *IMainItem::mob = **thisMob;
      if (!NODRAW)
        processInput();
      IMainItem::fabric->Sync();
      tick_recvm.lock();
      if(tick_recv)
      {
          //SYSTEM_STREAM << "Tick recv\n";
          process_in = true;
          process_in_msg();
          --tick_recv;
          MAIN_TICK++;
      }
      tick_recvm.unlock();
      if (thisMob.ret_id())
        *IMainItem::mob = **thisMob;
      if(mode_ == SERVER && (SDL_GetTicks() - lastTimeC > DELAY_MAIN) && !pause && NetMaster->CanSend())
      {
          //NetMaster->SendRand();
          if (MAIN_TICK % HASH_OFTEN)
              NetMaster->SendHash();
          NetMaster->SendTick();
          numOfDeer = 0;
          lastTimeC = SDL_GetTicks();          
          //srand(SDL_GetTicks());
      }
    if(process_in)
    {
        numOfDeer = 0;
        IMainItem::fabric->foreachProcess();
    }
      *IMainItem::mob = **thisMob;
      
      //if(thisMob->posx < sizeWsq || thisMob->posx > sizeWmap - sizeHsq || thisMob->posy < sizeHsq || thisMob->posy > sizeHmap - sizeWsq) 
          //SDL_BlitSurface(back, NULL, screen, NULL);
         
      if (!NODRAW)
      {
          map->Draw();
          FabricProcesser::Get()->process();
      }
        *IMainItem::mob = **thisMob;
      char loc[10];
      
      //checkMoveMob();
      if (!NODRAW)
        thisMob->processGUI();

      texts.Process();

      if((SDL_GetTicks() - lastTimeFps) >= 1000)
      {
          visiblePoint->clear();
            visiblePoint = map->losf.calculateVisisble(visiblePoint, thisMob->posx, thisMob->posy, thisMob->level); 
          _itoa_s(fps, loc, 10);
          //SYSTEM_STREAM << loc << std::endl;
          if (!NODRAW)
          {
              //SDL_FreeSurface(sFPS);
              //sFPS = TTF_RenderText_Blended(map->aSpr.font, loc, color);
          }
          if(!(fps > FPS_MAX - 10 && fps < FPS_MAX - 10))
            delay = (int)(1000.0 / FPS_MAX + delay - 1000.0 / fps);
          lastTimeFps = SDL_GetTicks();
          last_fps = fps;
          fps = 0;
          

          map->numOfPathfind = 0;
      }

      map_access.unlock();
      ++fps;
      if(delay > 0) 
          SDL_Delay(delay);
      else 
          delay = 0;

      gl_screen->Swap();
      //++MAIN_TICK;
      process_in = false;
    }
    TTF_Quit();
    SDL_Quit();
};

void Manager::checkMoveMob()
{
    
    /*SYSTEM_STREAM << "Check move " << thisMob.ret_id() << " " << thisMob.ret_item() <<  " " << thisMob->isMove << " " << isMove << "\n";
    if(thisMob->isMove && !isMove)
    {
        SYSTEM_STREAM << "Now move\n";
        isMove = true; 
        checkMove(thisMob->dMove);
        visiblePoint->clear();
        visiblePoint = map->losf.calculateVisisble(visiblePoint ,thisMob->posx, thisMob->posy, thisMob->level);
    }
    if(!thisMob->isMove && isMove)
    {
        SYSTEM_STREAM << "Now isnt move\n";
        isMove = false;
    }*/
};

void Manager::processInput()
{
      static Uint8* keys;
      static int lastShoot = 0;
      SDL_Event event;    
      while (SDL_PollEvent(&event))
      { 
        if(event.type == SDL_QUIT) done = 1; 
        if(event.type == SDL_KEYUP)
        {
            if(event.key.keysym.sym == SDLK_o) pause = !pause;  
        }
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            auto item = map->click(event.button.x, event.button.y);
            if (item.ret_id())
                last_touch = item->name;
        }
      }   
      keys = SDL_GetKeyState(NULL);
      if(true)//TODO: !pause THIS PROPLEM SOMETHINK WITH THIS
      {
      if(keys[SDLK_UP])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
              //SYSTEM_STREAM << "\nSEND UP\n";
              std::stringstream loc;
              loc << "SDLK_UP";
              NetMaster->SendToStrSock(*(NetMaster->sockstr), loc);
              lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_DOWN])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
              std::stringstream loc;
              loc << "SDLK_DOWN";
              NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
              //if(keys[SDLK_j]) thisMob->level = 1;
              //thisMob->checkMove(D_DOWN);
              lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_LEFT])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
              std::stringstream loc;
              loc << "SDLK_LEFT";
              NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
              //if(keys[SDLK_j]) thisMob->level = 1;
              //thisMob->checkMove(D_LEFT);
              lastShoot = (int)MAIN_TICK;
          }
      } 
      if(keys[SDLK_RIGHT])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_RIGHT";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            //if(keys[SDLK_j]) thisMob->level = 1;
            //thisMob->checkMove(D_RIGHT);
             lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_j])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_j";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_p])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_p";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_q])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_q";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_f])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_f";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_w])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_w";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_a])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_a";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_s])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_s";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      /*if(keys[SDLK_SPACE])
      {
          if(MAIN_TICK - lastShoot > 100)
          {
              int locx = thisMob->posx;
            int locy = thisMob->posy;
            if(thisMob->dMove == D_UP) locy -= 1;
            if(thisMob->dMove == D_DOWN) locy += 1;
            if(thisMob->dMove == D_LEFT) locx -= 1;
            if(thisMob->dMove == D_RIGHT) locx += 1;
          lastShoot = (int)MAIN_TICK;
          auto un = map->newItemOnMap<IOnMapItem>(hash("firebullet"), locx, locy);
          un->dMove = thisMob->dMove;
          }
      }
      if(keys[SDLK_n])
      {
          if(1/*MAIN_TICK - lastShoot > 100)
          {
          lastShoot = (int)MAIN_TICK;
          auto un = map->newItemOnMap<IOnMapItem>(hash("testmob"), IMainItem::mob->posx, IMainItem::mob->posy);
          un->dMove = (Dir)IMainItem::mob->dMove;
          }
      }*/
      if(keys[SDLK_F5])
      {
          int locatime = SDL_GetTicks();
          IMainItem::fabric->saveMap(GetMode() == SERVER ? "servermap.map" : "clientmap.map");
          SYSTEM_STREAM << "Map saved in "<< (SDL_GetTicks() - locatime) * 1.0 / 1000 << " second" << std::endl;
      }
      if(keys[SDLK_F6])
      {
          int locatime = SDL_GetTicks();
          IMainItem::fabric->clearMap();
          IMainItem::fabric->loadMap(GetMode() == SERVER ? "servermap.map" : "clientmap.map");
          SYSTEM_STREAM << "Map load in " << (SDL_GetTicks() - locatime) * 1.0 / 1000 << " second" << std::endl;
      }
      if(keys[SDLK_h])
      {
          SYSTEM_STREAM << "World's hash: " << IMainItem::fabric->hash_all() << std::endl; 
      }
      /*if(keys[SDLK_d])
      {
        auto itr = map->turf[MainItem::mob->posx][MainItem::mob->posy].begin();
        (*itr)->level = 0;
        (*itr)->transparent = true;
        (*itr)->SetSprite("icons/turf.png");
      }*/
      if(keys[SDLK_1])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_1";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_2])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_2";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_3])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_3";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_4])
      {
          if(MAIN_TICK - lastShoot > 4)
          {
            std::stringstream loc;
            loc << "SDLK_4";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
          }
      }
      if(keys[SDLK_h])
      {
          int locatime = SDL_GetTicks();
          auto itr = map->squares[IMainItem::mob->posx][IMainItem::mob->posy].begin();
          int i = 0;
          while(itr != map->squares[IMainItem::mob->posx][IMainItem::mob->posy].end())
          {
              SYSTEM_STREAM << i <<": Level " << (*itr)->level;
              itr++;
              i++;
          };
          SYSTEM_STREAM << "Num item: " << i << " in " << (SDL_GetTicks() - locatime) * 1.0 / 1000 << " sec" << std::endl;
      }
      if(keys[SDLK_d])
      {
        if(MAIN_TICK - lastShoot > 4)
        {
            std::stringstream loc;
            loc << "SDLK_d";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
        }
      }
      if(keys[SDLK_e])
      {
        if(MAIN_TICK - lastShoot > 4)
        {
            std::stringstream loc;
            loc << "SDLK_e";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
        }
      }
      if(keys[SDLK_c])
      {
        if(MAIN_TICK - lastShoot > 4)
        {
            std::stringstream loc;
            loc << "SDLK_c";
            NetMaster->SendToStrSock(*NetMaster->sockstr, loc);
            lastShoot = (int)MAIN_TICK;
        }
      }
      /*if(keys[SDLK_t])
      {
            int newx = get_rand() % sizeHmap;
            int newy = get_rand() % sizeWmap;
            //printf("begin\n");
            map->centerFromTo(mob.thisMob->posx, mob.thisMob->posy, newx, newy);
            //printf("end\n");
            mob.thisMob->posx = newx;
            mob.thisMob->posy = newy;
      }*/
      }
      /*if(keys[SDLK_f])
      {
          int toposx, toposy;
          scanf("%d%d", &toposx, &toposy);    
          pathMessage msg = {thisMob->posx, thisMob->posy, toposx, toposy, thisMob};
          SDL_mutexP(map->mut);
          map->pathListMessage.push_back(msg);
          SDL_mutexV(map->mut);
          printf("Path finded in sec\n");
          thisMob->sendPathRequest = true;
      }*/
};

void Manager::initWorld()
{
    tick_recv = 0;
    isMove = 0;
    Uint32 rmask, gmask, bmask, amask;
    SetMasks(&rmask, &gmask, &bmask, &amask);

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    { 
        SYSTEM_STREAM << "Unable to init SDL: " << SDL_GetError() << std::endl; 
        exit(1); 
    }
    atexit(SDL_Quit);
    SYSTEM_STREAM << "Begin TTF init\n";
    SYSTEM_STREAM << TTF_Init() << " - return\n";
    SYSTEM_STREAM << " End TTF init\n";
    atexit(TTF_Quit);
    IMainItem::fabric = new ItemFabric;
    map = new MapMaster;
    SDL_WM_SetCaption("GOS", "GOS");
    if (!NODRAW)
        gl_screen = new Screen(sizeW, sizeH);



    map->screen = gl_screen;
    map->loManager = 0;
    IMainItem::map = map;
    IMainItem::mobMaster = this;
    
    map->mobi = this;

    srand(SDL_GetTicks());
    id_ptr_on<IMob> newmob;

    newmob = IMainItem::fabric->newItemOnMap<IMob>(hash("ork"), sizeHmap / 2, sizeWmap / 2);
    //GLSprite testspr("icons/kivsjak.png");
    changeMob(newmob);
    newmob->x = beginMobPosX * TITLE_SIZE;
    newmob->y = beginMobPosY * TITLE_SIZE;

    auto tptr = IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("Teleportator"), sizeHmap / 2, sizeWmap / 2);

    map->makeMap();
    thisMob->passable = 1;
    LiquidHolder::LoadReaction();

    NetMaster = new NetMain;
    NetMaster->now_creator = tptr;
    if(mode_ == SERVER)
        NetMaster->StartServer();
    NetMaster->StartClient(DEFAULT_PORT, adrs_);

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
    std::string msg;
    while(true)
    {
        msg = NetMaster->in_messages.Get();
        std::string idstr = msg.substr(0, msg.find_first_of(" "));
        msg = msg.substr(msg.find_first_of(" ") + 1);
        if(msg == "nexttick")
            break;
        
        std::stringstream ss;
        ss << idstr;
        size_t idnum;
        ss >> idnum;
        if(msg == "rand")
        {
            set_rand(idnum);
            continue;
        }
        id_ptr_on<IMessageReceiver> i;
        i = idnum;
        if(i)
            i->processGUImsg(msg);
    }
}

bool Manager::isMobVisible(int posx, int posy)
{
    // TODO: matrix for fast check
    if(visiblePoint == nullptr)
        return false;
    for (auto it = visiblePoint->begin(); it != visiblePoint->end(); ++it)
        if(it->posx == posx && it->posy == posy)
            return true;
    return false;
}


