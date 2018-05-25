#pragma once

#include <string>
#include <vector>

#include <QGLWidget>

// TODO: fix me
#include "platform/gl.h"

#include "Metadata.h"

class GLSprite
{
public:
    GLSprite(const QString& string);
    ~GLSprite();
    const QVector<GLuint>& operator[](qint32 num) const;
    bool Fail() const;

    // int becouse x or y could be negative, don't touch this
    int W() const;
    int H() const;

    const ImageMetadata& GetMetadata() const { return metadata_; }
    const QVector<QImage>& GetFrames() const { return frames_; }

    int FrameW() const;
    int FrameH() const;
private:
    void InitTextures();
    void InitMetadataAndFrames(const QString& path);
    int frames_w_;
    int frames_h_;
    QVector<QVector<GLuint>> gl_sprites_;
    ImageMetadata metadata_;
    QVector<QImage> frames_;
    bool fail_;
};
