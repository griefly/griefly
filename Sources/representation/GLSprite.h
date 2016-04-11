#pragma once

#include <string>
#include <vector>

#include <QGLWidget>

#include "platform/gl.h"

#include "Sprite.h"

class GLSprite
{
public:
    GLSprite(const std::string& string);
    ~GLSprite();
    const std::vector<GLuint>& operator[](size_t num) const;
    bool Fail() const;

    // int becouse x or y could be negative, don't touch this
    int W() const;
    int H() const;

    CSprite* GetSDLSprite() const;

    unsigned int FrameW() const;
    unsigned int FrameH() const;
private:
    void Init(CSprite* sprite);
    int w_;
    int h_;
    std::vector<std::vector<GLuint>> gl_sprites_;
    CSprite* sdl_sprite_;
    bool fail_;
};
