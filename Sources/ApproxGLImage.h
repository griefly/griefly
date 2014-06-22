#pragma once

#include <SDL.h>
#include <SDL_opengl.h>

#include "platform/gl.h"

class ApproxGLImage
{
public:
    ApproxGLImage(SDL_Surface* image);
    ~ApproxGLImage();
    GLuint GetText() const;
    float GetX() const;
    float GetY() const;
    int GetXSize() const;
    int GetYSize() const;
    bool Fail() const;
private:
    static int GetNext2Power(int value);

    SDL_Surface* image_;

    bool fail_;
    int x_size_;
    int y_size_;
    float texture_real_x_;
    float texture_real_y_;
    GLuint texture_;
};
