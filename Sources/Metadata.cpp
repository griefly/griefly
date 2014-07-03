#include "Metadata.h"

#include <fstream>

#include <png.h>

#include "constheader.h"

const ImageMetadata::SpriteMetadata& 
    ImageMetadata::GetSpriteMetadata(const std::string& name)
{
    return metadata_[name];
}

const int PNGSIGSIZE = 8;

void userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
    png_voidp a = png_get_io_ptr(pngPtr);
    (static_cast<std::istream*>(a))->
        read(reinterpret_cast<char*>(data), length);
}

void ImageMetadata::Init(const std::string& name)
{
    png_byte pngsig[PNGSIGSIZE];

    std::ifstream source;
    source.open(name, std::fstream::binary);
    if (source.fail()) 
    {
        SYSTEM_STREAM << "ERROR: Fail to open smth" << std::endl;
        SDL_Delay(10000);
        return;
    }

    source.read(reinterpret_cast<char*>(pngsig), PNGSIGSIZE);

    if (source.fail()) 
    {
        SYSTEM_STREAM << "ERROR: Fail to read smth" << std::endl;
        SYSTEM_STREAM << source.bad() << " " << source.fail() << " " << source.eof() << std::endl;
        SDL_Delay(10000);
        return;
    }

    int is_png = png_sig_cmp(pngsig, 0, PNGSIGSIZE);

    if (is_png) {
        SYSTEM_STREAM << "ERROR: Data is not valid PNG-data: " << is_png << std::endl;
        return;
    }

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngPtr) 
    {
        SYSTEM_STREAM << "ERROR: Couldn't initialize png read struct" << std::endl;
        return;
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr) 
    {
        std::cerr << "ERROR: Couldn't initialize png info struct" << std::endl;
        png_destroy_read_struct(&pngPtr, static_cast<png_infopp>(0), static_cast<png_infopp>(0));
        return;
    }

    png_set_read_fn(pngPtr, static_cast<png_voidp>(&source), userReadData);

    png_set_sig_bytes(pngPtr, PNGSIGSIZE);
    png_read_info(pngPtr, infoPtr);

    png_textp text_ptr;
    int num_text;

    if (png_get_text(pngPtr, infoPtr, &text_ptr, &num_text) > 0)
    {
        for (int i = 0; i < num_text; ++i)
        {
            SYSTEM_STREAM << "BEGIN KEY " << i << std::endl;
            SYSTEM_STREAM << text_ptr[i].key << std::endl;
            SYSTEM_STREAM << "END KEY " << i << std::endl;
            SYSTEM_STREAM << "BEGIN TEXT " << i << std::endl;
            SYSTEM_STREAM << text_ptr[i].text << std::endl;
            SYSTEM_STREAM << "END TEXT " << i << std::endl;
        }
    }

    png_destroy_read_struct(&pngPtr, &infoPtr, static_cast<png_infopp>(0));
    source.close();
}