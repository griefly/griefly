#include <cassert>

#include "GLSprite.h"
#include "core/Constheader.h"

#include "core/objects/MainObject.h"

#include "qt/qtopengl.h"

unsigned int GLSprite::FrameW() const
{
    return gl_sprites_[0].size();
}

unsigned int GLSprite::FrameH() const
{
    return gl_sprites_.size();
}

int GLSprite::W() const
{
    return w_;
}

int GLSprite::H() const
{
    return h_;
}

const std::vector<GLuint>& GLSprite::operator[](size_t num) const
{
    return gl_sprites_[num];
}

void GLSprite::Init(CSprite* sprite)
{
    fail_ = true;

    // Hack for mapeditor
    bool force_no_fail = false;
    if (GetGLWidget() == nullptr)
    {
        force_no_fail = true;
    }

    GLint maxTexSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    if ((sprite->w > maxTexSize || sprite->h > maxTexSize) && !force_no_fail)
    {
        SYSTEM_STREAM << "OpenGL texture load fail: texture too big, maximal allowed size is " << maxTexSize << std::endl;
        return;
    }

    sdl_sprite_ = sprite;
    w_ = sprite->w;
    h_ = sprite->h;

    gl_sprites_.resize(sprite->numFrameH);
    for (int i = 0; i < sprite->numFrameH; ++i)
        gl_sprites_[i].resize(sprite->numFrameW);

    GLuint glFormat_text = GL_RGBA;
    GLuint glFormat_surf = GL_RGBA;

    for (int i = 0; i < sprite->numFrameH; ++i)
    {
        glGenTextures(gl_sprites_[i].size(), &(gl_sprites_[i][0]));
        for (int j = 0; j < sprite->numFrameW; ++j)
        {
            //SYSTEM_STREAM << i << " " << j << std::endl;

            glBindTexture(GL_TEXTURE_2D, gl_sprites_[i][j]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            QImage image = QGLWidget::convertToGLFormat(sprite->frames[j * sprite->numFrameH + i]);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                glFormat_text,
                sprite->w,
                sprite->h,
                0,
                glFormat_surf,
                GL_UNSIGNED_BYTE,
                image.bits());

            //SYSTEM_STREAM << gl_sprites_[i][j] << std::endl;

            if (glGetError())
                SYSTEM_STREAM << glGetError() << std::endl;
            //if (sprite->frames[j * sprite->numFrameH + i] == nullptr || sprite->frames[j * sprite->numFrameH + i]->pixels == nullptr)
            //    SYSTEM_STREAM << "nullptr pixels" << std::endl;
            //SYSTEM_STREAM << sprite->frames[j * sprite->numFrameH + i] << std::endl;
        }
    }

    fail_ = false;
}

GLSprite::GLSprite(const std::string& name)
{
    MakeCurrentGLContext();
    CSprite* sprite = new CSprite;

    InitSprite data;
    data.imgFile = name;
    data.numFrameH = 0;
    data.numFrameW = 0;
    fail_ = false;
    if (!sprite->init(data))
    {
        fail_ = true;
        SYSTEM_STREAM << "Fail to load sprite " << name << std::endl;
        return;
    }

    Init(sprite);
}

GLSprite::~GLSprite()
{
    MakeCurrentGLContext();
    for (size_t i = 0; i < gl_sprites_.size(); ++i)
        glDeleteTextures(gl_sprites_[i].size(), &gl_sprites_[i][0]);
    delete sdl_sprite_;
}

bool GLSprite::Fail() const
{
    return fail_;
}

CSprite* GLSprite::GetSDLSprite() const
{
    return sdl_sprite_;
}
