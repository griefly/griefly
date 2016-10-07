#pragma once

#include "Sprite.h"
#include <map>
#include <string>

#include "GLSprite.h"
#include "Screen.h"

class SpriteHolder
{
public:
    SpriteHolder();
    GLSprite* GetSprite(const std::string& type);
    void LoadImage(const std::string& image);
private:
    std::map<std::string, GLSprite*> sprites;
};

SpriteHolder& GetSpriter();
bool IsSpriterValid();
void SetSpriter(SpriteHolder* aspr);
