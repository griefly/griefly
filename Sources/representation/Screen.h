#pragma once

#include "GLSprite.h"

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

    int w();
    int h();
private:
    bool fail_;
};

bool IsScreenValid();
Screen& GetScreen();
void SetScreen(Screen* scr);
