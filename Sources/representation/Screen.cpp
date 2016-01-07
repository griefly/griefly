#include "Screen.h"

#include <assert.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "Params.h"
#include "qt/qtopengl.h"

Screen::Screen(unsigned int x, unsigned int y, bool fullscreen)
{
    fail_ = true;
    ResetScreen(x, y, 32, 0);
    //if (SDL_Init(SDL_INIT_VIDEO) != 0)
    //    return;

    /*auto flag = SDL_FULLSCREEN;
    if(!fullscreen)
        flag = 0;
    ResetScreen(x, y, 32, SDL_OPENGL | SDL_RESIZABLE | flag);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrtho(0, x, y, 0, 0, 1); // TODO: check
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
  
    glClearColor(0, 0, 0, 0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    if (!GetParamsHolder().GetParamBool("-novsync"))
    {
#ifdef WIN32
        typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);
        PFNWGLSWAPINTERVALEXTPROC wglSwapInterval = nullptr;

        wglSwapInterval = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
        if (wglSwapInterval) 
            wglSwapInterval(1);
#endif
    }*/
    fail_ = false;
}

void Screen::PerformSizeUpdate()
{
    glViewport(0, 0, GetGLWidget()->width(), GetGLWidget()->height());
}

void Screen::ResetScreen(int x, int y, int bpp, Uint32 flags)
{
    GetGLWidget()->resize(x, y);
}

void Screen::Swap()
{
    GetGLWidget()->swapBuffers();
}

void Screen::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Screen::Draw(const ApproxGLImage* sprite, int x_ul, int y_ul, int x_dr, int y_dr)
{
    if (sprite == nullptr)
        return;

    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, sprite->GetText());
    if (glGetError())
        SYSTEM_STREAM << glGetError() << std::endl; 

    float text_x = sprite->GetX();
    float text_y = sprite->GetY();

    glBegin(GL_QUADS);
        glTexCoord2f(0.0,       0.0);     glVertex3f(static_cast<float>(x_ul), static_cast<float>(y_ul), 0.0f);
        glTexCoord2f(0.0,    text_y);     glVertex3f(static_cast<float>(x_ul), static_cast<float>(y_dr), 0.0f);
        glTexCoord2f(text_x, text_y);     glVertex3f(static_cast<float>(x_dr), static_cast<float>(y_dr), 0.0f);
        glTexCoord2f(text_x,    0.0);     glVertex3f(static_cast<float>(x_dr), static_cast<float>(y_ul), 0.0f);
    glEnd();
    if (glGetError())
        SYSTEM_STREAM << glGetError() << std::endl; 
}

void Screen::Draw(const GLSprite* sprite_in, int x, int y, int imageW, int imageH, float angle)
{
    if (sprite_in == nullptr)
        return;

    const GLSprite& sprite = *sprite_in;

    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, sprite[imageH][imageW]);
    if (glGetError())
        SYSTEM_STREAM << glGetError() << std::endl; 

    glTranslatef(     x + sprite.W() / 2.0f,      y + sprite.H() / 2.0f, 0.0f); 
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-1 * x - sprite.W() / 2.0f, -1 * y - sprite.H() / 2.0f, 0.0f); 

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);     glVertex3f(static_cast<float>(x),              static_cast<float>(y), 0.0f);
        glTexCoord2f(0.0, 1.0);     glVertex3f(static_cast<float>(x),              static_cast<float>(y + sprite.H()), 0.0f);
        glTexCoord2f(1.0, 1.0);     glVertex3f(static_cast<float>(x + sprite.W()), static_cast<float>(y + sprite.H()), 0.0f);
        glTexCoord2f(1.0, 0.0);     glVertex3f(static_cast<float>(x + sprite.W()), static_cast<float>(y), 0.0f);
    glEnd();
    glLoadIdentity();

    if (glGetError())
        SYSTEM_STREAM << glGetError() << std::endl; 

}

int Screen::w()
{
    return GetGLWidget()->width();
}
int Screen::h()
{
    return GetGLWidget()->height();
}

Screen* screen = nullptr;
void SetScreen(Screen* scr)
{
    screen = scr;
}
Screen& GetScreen()
{
    return *screen;
}


bool IsScreenValid()
{
    return screen != nullptr;
}
