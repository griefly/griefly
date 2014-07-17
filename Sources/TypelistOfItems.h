#pragma once

#include <Typelist.h>
#include <TypeManip.h>

namespace KV_Private_NS_NullType
{
    typedef Loki::NullType typelist;
}

#define NOW_NAMESPACE KV_Private_NS_NullType

#ifdef OPEN_STATIC_VARS
#undef OPEN_STATIC_VARS
#endif // OPEN_STATIC_VARS

#define OPEN_STATIC_VARS true

#ifndef BR_CLOSE_IF_OPEN 
#define BR_CLOSE_IF_OPEN }
#endif // BR_CLOSE_IF_OPEN 

#ifndef BR_CLOSE_IF_NO_OPEN 
#define BR_CLOSE_IF_NO_OPEN
#endif // BR_CLOSE_IF_NO_OPEN

#include "OnMapBaseInt.h"
#include "DrawInt.h"
#include "MainInt.h"
#include "MobInt.h"
#include "OnMapInt.h"
#include "CAliveMob.h"
#include "CSmallItem.h"
#include "Firebullet.h"
#include "MonstrClass.h"
#include "Ork.h"
#include "Testmob.h"
#include "Turf.h"
#include "Liquid.h"
#include "LiquidHolder.h"
#include "Kivsjak.h"
#include "MessageReceiver.h"
#include "Teleportator.h"
#include "TileInt.h"
#include "ITurf.h"
#include "IMovable.h"
#include "SpaceTurf.h"
#include "MetalWall.h"
#include "Floor.h"
#include "Door.h"
#include "Grille.h"
#include "Glass.h"
#include "Item.h"
#include "Shard.h"
#include "Structure.h"

const int local_num = __COUNTER__; 
typedef Private::TypelistMaker<local_num - 1>::typelist ItemsTypelist;

const int ItemsTLLength = Loki::TL::Length<ItemsTypelist>::value;

