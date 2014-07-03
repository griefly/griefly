#include "Metadata.h"

#include <png.h>

const ImageMetadata::SpriteMetadata& 
    ImageMetadata::GetSpriteMetadata(const std::string& name)
{
    return metadata_[name];
}

void ImageMetadata::Init(const std::string& name)
{

}