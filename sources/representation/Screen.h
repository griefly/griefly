#pragma once

#include "GLSprite.h"

const int AREA_SIZE_W = 512; // visible OpenGL area in pixels
const int AREA_SIZE_H = 512;

class Screen
{
public:
    Screen(unsigned int x, unsigned int y, bool fullscreen = false);
    void Draw(const GLSprite* sprite, int x, int y, int imageW, int imageH, float angle = 0.0f);
    void ResetScreen(int x, int y);
    void Clear();
    void Swap();
    bool Fail();

    void PerformSizeUpdate();

    void NormalizePixels(int* x, int* y)
    {
        *x = static_cast<int>
            (  static_cast<float>(*x)
            * (  static_cast<float>(AREA_SIZE_W)
               / static_cast<float>(GetWidth())
               ));
        *y = static_cast<int>
            (  static_cast<float>(*y)
            * (  static_cast<float>(AREA_SIZE_H)
               / static_cast<float>(GetHeight())
               ));
    }

    int GetWidth();
    int GetHeight();
private:
    bool fail_;
};

bool IsScreenValid();
Screen& GetScreen();
void SetScreen(Screen* scr);
