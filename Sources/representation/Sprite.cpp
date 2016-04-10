#include "Sprite.h"

#include <QGLWidget>
#include <QDebug>

void SetMasks(Uint32* rmask, Uint32* gmask, Uint32* bmask, Uint32* amask)
{
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      *rmask = 0xff000000;
      *gmask = 0x00ff0000;
      *bmask = 0x0000ff00;
      *amask = 0x000000ff;
    #else
      *rmask = 0x000000ff;
      *gmask = 0x0000ff00;
      *bmask = 0x00ff0000;
      *amask = 0xff000000;
    #endif
}

bool CSprite::init(InitSprite data)
{
    QImage image;
    if (!image.load(QString::fromStdString(data.imgFile)))
    {
        qDebug() << "Image load error: " << QString::fromStdString(data.imgFile);
        return false;
    }

    metadata.Init(data.imgFile, image.width(), image.height());
    if (metadata.Valid())
    {
        numFrameW = image.width() / metadata.GetW();
        numFrameH = image.height() / metadata.GetH();
    }
    else
    {
        numFrameH = data.numFrameH;
        numFrameW = data.numFrameW;
    }
    if((numFrameH == 0) || (numFrameW == 0))
    {        
        w = image.width();
        h = image.height();
        numFrameW = 1;
        numFrameH = 1;
    }
    else
    {
        w = image.width() / numFrameW;
        h = image.height() / numFrameH;
    }
    SYSTEM_STREAM << numFrameW << "x" << numFrameH << " - loaded " << data.imgFile << std::endl;

    Uint32 rmask, gmask, bmask, amask;
    SetMasks(&rmask, &gmask, &bmask, &amask);

    int x = 0;
    int y = 0;
    frames.resize(numFrameW * numFrameH);

    for(int j = 0; j < numFrameH; ++j)
    {
        y = j * h;
        for(int i = 0; i < numFrameW; ++i)
        {
            x = i * w;
            // frames[i * numFrameH + j] = QGLWidget::convertToGLFormat(image.copy(x, y, w, h));
            frames[i * numFrameH + j] = image.copy(x, y, w, h);
        }
    }
    return true;
}

CSprite::CSprite()
{
    h = 0;
    w = 0;
    numFrameH = 0;
    numFrameW = 0;
}

CSprite::~CSprite()
{

}
