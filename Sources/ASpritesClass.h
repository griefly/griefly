#ifndef ASPRITESCLASS_H
#define ASPRITESCLASS_H

#include "sprite.h"
#include <map>
#include <string>

#include "GLSprite.h"
#include "Screen.h"

class ASprClass
{
public:
    ASprClass();
    GLSprite* returnSpr(std::string type);//use it for ptr to sprite(try load sprite if isnt exist)
    TTF_Font* font;
private:
    std::map<std::string, GLSprite*> sprites;
};

#endif