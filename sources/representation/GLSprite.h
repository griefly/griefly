#pragma once

#include <string>
#include <vector>

#include <QGLWidget>

#include "platform/gl.h"

#include "Metadata.h"

class GLSprite
{
public:
    GLSprite(const QString& string);
    ~GLSprite();
    const std::vector<GLuint>& operator[](quint32 num) const;
    bool Fail() const;

    // int becouse x or y could be negative, don't touch this
    int W() const;
    int H() const;

    const ImageMetadata& GetMetadata() const { return metadata_; }
    const std::vector<QImage>& GetFrames() const { return frames_; }

    unsigned int FrameW() const;
    unsigned int FrameH() const;
private:
    void InitTextures();
    void InitMetadataAndFrames(const QString& path);
    int frames_w_;
    int frames_h_;
    std::vector<std::vector<GLuint>> gl_sprites_;
    ImageMetadata metadata_;
    std::vector<QImage> frames_;
    bool fail_;
};
