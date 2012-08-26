#ifndef FIREBULLET_H
#define FIREBULLET_H

#include "CSmallItem.h"

class CFirebullet : public SmallItem
{
public:
    DECLARE_GET_TYPE_ITEM(firebullet)
    DECLARE_SAVED(CFirebullet, SmallItem);
    virtual void process();
    CFirebullet();
};

ADD_TO_TYPELIST( CFirebullet );

#endif