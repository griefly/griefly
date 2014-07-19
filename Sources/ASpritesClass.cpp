#include "ASpritesClass.h"

GLSprite* ASprClass::returnSpr(const std::string& type)
{
    if (NODRAW)
        return nullptr;
    if(sprites.find(type) == sprites.end())
        sprites[type] = new GLSprite(type);
    return sprites[type];
}

ASprClass::ASprClass()
{
};

ASprClass* aspr_ = nullptr;
ASprClass* GetSpriter()
{
    return aspr_;
}
void SetSpriter(ASprClass* aspr)
{
    aspr_ = aspr;
}

void ASprClass::LoadImage(const std::string& image)
{
    sprites[image] = new GLSprite(image);
}