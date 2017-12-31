#include <cassert>

//#include "KvGlobals.h"

#include "GLSprite.h"

//#include "objects/Object.h"

#include "qt/qtopengl.h"

#include <QDebug>

GLSprite::GLSprite(const QString& name)
{
    MakeCurrentGLContext();

    InitMetadataAndFrames(name);
    InitTextures();
}

GLSprite::~GLSprite()
{
    MakeCurrentGLContext();
    for (quint32 i = 0; i < gl_sprites_.size(); ++i)
    {
        glDeleteTextures(gl_sprites_[i].size(), &gl_sprites_[i][0]);
    }
}

void GLSprite::InitMetadataAndFrames(const QString& path)
{
    QImage image;
    if (!image.load(path))
    {
        qFatal("%s", QString("Image load error: : %1").arg(path).toLatin1().data());
    }

    metadata_.Init(path, image.width(), image.height());
    if (!metadata_.Valid())
    {
        qFatal("%s", QString("Invalid metadata, aborting: %1").arg(path).toLatin1().data());
    }

    frames_w_ = image.width() / metadata_.GetW();
    frames_h_ = image.height() / metadata_.GetH();

    qDebug() << frames_w_ << "x" << frames_h_ << " - loaded " << path;
    frames_.resize(frames_w_ * frames_h_);

    for(int j = 0; j < frames_h_; ++j)
    {
        for(int i = 0; i < frames_w_; ++i)
        {
            frames_[i * frames_h_ + j]
                = image.copy(i * W(), j * H(), W(), H());
        }
    }
}

void GLSprite::InitTextures()
{
    fail_ = true;

    // Hack for map editor
    if (GetGLWidget() == nullptr)
    {
        GLint max_texture_size;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        if (   W() > max_texture_size
            || H() > max_texture_size)
        {
            qDebug() << "OpenGL texture load fail: texture too big, maximal allowed size is "
                     << max_texture_size;
            return;
        }
    }

    gl_sprites_.resize(frames_h_);
    for (int i = 0; i < frames_h_; ++i)
    {
        gl_sprites_[i].resize(frames_w_);
    }

    for (int i = 0; i < frames_h_; ++i)
    {
        glGenTextures(gl_sprites_[i].size(), &(gl_sprites_[i][0]));
        for (int j = 0; j < frames_w_; ++j)
        {
            glBindTexture(GL_TEXTURE_2D, gl_sprites_[i][j]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            QImage image = QGLWidget::convertToGLFormat(frames_[j * frames_h_ + i]);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                W(),
                H(),
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                image.bits());

            if (glGetError())
            {
                qDebug() << "Some OpenGL error has occured:" << glGetError();
            }
        }
    }

    fail_ = false;
}

unsigned int GLSprite::FrameW() const
{
    return frames_w_;
}

unsigned int GLSprite::FrameH() const
{
    return frames_h_;
}

int GLSprite::W() const
{
    return metadata_.GetW();
}

int GLSprite::H() const
{
    return metadata_.GetH();
}

const std::vector<GLuint>& GLSprite::operator[](quint32 num) const
{
    return gl_sprites_[num];
}

bool GLSprite::Fail() const
{
    return fail_;
}
