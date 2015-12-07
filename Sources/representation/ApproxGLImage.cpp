#include "ApproxGLImage.h"

#include "constheader.h"
#include "sprite.h"

int ApproxGLImage::GetNext2Power(int value)
{
    int retval = 1;
    while (value > retval)
        retval *= 2;

    return retval;
}

float ApproxGLImage::GetX() const
{
    return texture_real_x_;
}

int ApproxGLImage::GetXSize() const
{
    return x_size_;
}

int ApproxGLImage::GetYSize() const
{
    return y_size_;
}

float ApproxGLImage::GetY() const
{
    return texture_real_y_;
}

GLuint ApproxGLImage::GetText() const
{
    return texture_;
}

bool ApproxGLImage::Fail() const
{
    return fail_;
}

ApproxGLImage::ApproxGLImage(SDL_Surface* image)
{
    fail_ = true;
    image_ = nullptr;

    GLint maxTexSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    if(image->w > maxTexSize || image->h > maxTexSize)
    {
        SYSTEM_STREAM << "OpenGL texture load fail: texture too big, maximal allowed size is " << maxTexSize << std::endl;
        return;
    }

    // Convert our image to n ^ 2 image
    SDL_Surface* temp;

    int w = GetNext2Power(image->w);
    int h = GetNext2Power(image->h);

    Uint32 rmask, gmask, bmask, amask;
    SetMasks(&rmask, &gmask, &bmask, &amask);

    temp = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, w, h, 32, bmask, gmask, rmask, amask);

    auto temp_flags = image->flags;

    if (image->flags & SDL_SRCALPHA)
        image->flags = image->flags ^ SDL_SRCALPHA;//for alpha canal

    SDL_BlitSurface(image, nullptr, temp, nullptr);

    image->flags = temp_flags;
    //
    image_ = image;

    glGenTextures(1, &texture_);

    glBindTexture(GL_TEXTURE_2D, texture_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    GLuint glFormat_text = GL_RGBA;
    GLuint glFormat_surf = GL_BGRA;

    glTexImage2D(GL_TEXTURE_2D, 0, glFormat_text, w, h,
                 0, glFormat_surf, GL_UNSIGNED_BYTE, temp->pixels);

    SDL_FreeSurface(temp);

    texture_real_x_ = static_cast<float>((image->w * 1.0) /  (w * 1.0));
    texture_real_y_ = static_cast<float>((image->h * 1.0) /  (h * 1.0));

    x_size_ = image_->w;
    y_size_ = image_->h;

    fail_ = false;
}

ApproxGLImage::~ApproxGLImage()
{
    glDeleteTextures(1, &texture_);
    if (image_)
        SDL_FreeSurface(image_);
}