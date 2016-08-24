#include "Metadata.h"

#include <fstream>
#include <sstream>
#include <string>

#include <QDebug>

#include <png.h>

#include "KVAbort.h"

QDebug operator<<(QDebug debug, const std::string& string)
{
    debug << QString::fromStdString(string);
    return debug;
}

const ImageMetadata::SpriteMetadata& 
    ImageMetadata::GetSpriteMetadata(const std::string& name)
{
    return metadata_[name];
}

bool ImageMetadata::IsValidState(const std::string& name)
{
    if (!Valid())
    {
        return false;
    }
    return metadata_.find(name) != metadata_.end();
}

const int PNGSIGSIZE = 8;

void userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
    png_voidp a = png_get_io_ptr(pngPtr);
    (static_cast<std::istream*>(a))->
        read(reinterpret_cast<char*>(data), length);
}

void ImageMetadata::Init(const std::string& name, int width, int height)
{   
    qDebug() << "Begin to init metadata for " << name;
    png_byte pngsig[PNGSIGSIZE];

    width_ = width;
    height_ = height;

    std::ifstream source;
    source.open(name, std::fstream::binary);
    if (source.fail()) 
    {
        qDebug() << "Metadata error: Fail to open file";
        return;
    }

    source.read(reinterpret_cast<char*>(pngsig), PNGSIGSIZE);

    if (source.fail()) 
    {
        qDebug() << "Metadata error: Fail to read png signature";
        kv_abort();
        return;
    }

    int is_png = png_sig_cmp(pngsig, 0, PNGSIGSIZE);

    if (is_png)
    {
        qDebug() << "Metadata error: Data is not valid PNG-data: " << is_png;
        InitWithoutMetadata();
        return;
    }

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngPtr) 
    {
        qDebug() << "Metadata error: Couldn't initialize png read struct";
        return;
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr) 
    {
        qDebug() << "Metadata error: Couldn't initialize png info struct";
        png_destroy_read_struct(&pngPtr, static_cast<png_infopp>(0), static_cast<png_infopp>(0));
        return;
    }

    png_set_read_fn(pngPtr, static_cast<png_voidp>(&source), userReadData);

    png_set_sig_bytes(pngPtr, PNGSIGSIZE);
    png_read_info(pngPtr, infoPtr);

    png_get_IHDR(pngPtr, infoPtr, &total_width_, &total_height_, 0, 0, 0, 0, 0);


    png_textp text_ptr;
    int num_text;

    if (png_get_text(pngPtr, infoPtr, &text_ptr, &num_text) > 0)
    {
        for (int i = 0; i < num_text; ++i)
        {
           /* qDebug() << "BEGIN KEY " << i << std::endl;
            qDebug() << text_ptr[i].key << std::endl;
            qDebug() << "END KEY " << i << std::endl;
            qDebug() << "BEGIN TEXT " << i << std::endl;
            qDebug() << text_ptr[i].text << std::endl;
            qDebug() << "END TEXT " << i << std::endl;
            */
            std::string string_key = std::string(text_ptr[i].key);
            if (string_key == "Description")
            {
                std::stringstream string_text;
                string_text << std::string(text_ptr[i].text);
                ParseDescription(string_text);
                break;
            }
        }
    }
    //SDL_Delay(100000);
    png_destroy_read_struct(&pngPtr, &infoPtr, static_cast<png_infopp>(0));
    source.close();
    qDebug() << "End load metadata for " << name;

    if (!Valid())
    {
        InitWithoutMetadata();
    }
}

void ImageMetadata::InitWithoutMetadata()
{
    qDebug() << "Fail metadata load, try without it";
    metadata_[""].first_frame_pos = 0;

    metadata_[""].frames_sequence.push_back(0);
    valid_ = true;
}

bool ImageMetadata::ParseDescription(std::stringstream& desc)
{
    std::string loc;
    desc >> loc;
    if (loc != "#")
    {
        qDebug() << "Fail to read '#' from .dmi file";
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "BEGIN")
    {
        qDebug() << "Fail to read 'BEGIN' from .dmi file";
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "DMI")
    {
        qDebug() << "Fail to read 'DMI' from .dmi file";
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "version")
    {
        qDebug() << "Fail to read 'version' from .dmi file";
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "=")
    {
        qDebug() << "Fail to read '=' from .dmi file";
        return false;
    }
    loc.clear();
    ////////////
    desc >> dmi_version_;
    qDebug() << "Read version: " << dmi_version_;
    ////////////
    desc >> loc;
    if (loc != "width")
    {
        qDebug() << "Fail to read 'width' from .dmi file";
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "=")
    {
        qDebug() << "Fail to read '=' from .dmi file";
        return false;
    }
    loc.clear();
    
    //////////////
    desc >> width_;
    qDebug() << "Read width: " << width_;
    /////////////

    desc >> loc;
    if (loc != "height")
    {
        qDebug() << "Fail to read 'height' from .dmi file";
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "=")
    {
        qDebug() << "Fail to read '=' from .dmi file";
        return false;
    }
    loc.clear();
    
    /////////////
    desc >> height_;
    qDebug() << "Read height: " << height_;
    ////////////
    
    desc >> loc;
    std::string current_state = "###";
    size_t first_frame_pos = 0;
    while (loc != "#")
    {
        if (loc == "state")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                qDebug() << "Fail to read '=' from .dmi file";
                return false;
            }
            std::string whole_str = "";
               
            loc.clear();
            desc >> loc;
            whole_str += loc;
            while (loc[loc.length() - 1] != '"')
            {
                loc.clear();
                desc >> loc;
                whole_str += " ";
                whole_str += loc;
            }
            loc = whole_str;
            qDebug() << "New state: " << loc;
            current_state = loc.substr(1, loc.length() - 2);
            metadata_[current_state].first_frame_pos = first_frame_pos;
          //  qDebug() << "First frame position: " << first_frame_pos << std::endl;
        }
        else if (loc == "dirs")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                qDebug() << "Fail to read '=' from .dmi file";
                return false;
            }
            loc.clear();

            size_t dirs;
            desc >> dirs;
      //      qDebug() << "Dirs: " << dirs << std::endl;

            if (current_state == "###")
            {
                qDebug() << "Dirs without state";
                return false;
            }
            metadata_[current_state].dirs = dirs;
        }
        else if (loc == "frames")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                qDebug() << "Fail to read '=' from .dmi file";
                return false;
            }
            loc.clear();

            size_t frames;
            desc >> frames;
      //      qDebug() << "Frames: " << frames << std::endl;

            if (current_state == "###")
            {
                qDebug() << "Frames without state";
                return false;
            }
            metadata_[current_state].frames_data.resize(frames);
            first_frame_pos += frames * metadata_[current_state].dirs;
        }
        else if (loc == "delay")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                qDebug() << "Fail to read '=' from .dmi file";
                return false;
            }
            loc.clear();

            if (current_state == "###")
            {
                qDebug() << "Delay without state";
                return false;
            }

            for (size_t i = 0; i < metadata_[current_state].frames_data.size() - 1; ++i)
            {
                size_t value;
                desc >> value;
         //       qDebug() << "Delay " << i << " is: " << value << std::endl;
                metadata_[current_state].frames_data[i].delay = value;

                char comma;
                desc >> comma;
                if (comma != ',')
                {
                    qDebug() << "Fail to read ',' from .dmi file";
                    return false;
                }
            }
            size_t value;
            desc >> value;
     //       qDebug() << "Last delay is: " << value << std::endl;
            metadata_[current_state].frames_data
                [metadata_[current_state].frames_data.size() - 1]
                    .delay = value;
        }
        else if (loc == "rewind")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                qDebug() << "Fail to read '=' from .dmi file";
                return false;
            }
            loc.clear();

            if (current_state == "###")
            {
                qDebug() << "Rewind without state";
                return false;
            }

            size_t rewind;
            desc >> rewind;
     //       qDebug() << "Rewind is: " << rewind << std::endl;
            metadata_[current_state].rewind = rewind ? true : false;
        }
        else if (loc == "loop")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                qDebug() << "Fail to read '=' from .dmi file";
                return false;
            }
            loc.clear();

            if (current_state == "###")
            {
                qDebug() << "Loop without state";
                return false;
            }

            int loop;
            desc >> loop;
       //     qDebug() << "Loop is: " << loop << std::endl;
            metadata_[current_state].loop = loop;
        }
        else if (loc == "hotspot")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                qDebug() << "Fail to read '=' from .dmi file";
                return false;
            }
            loc.clear();

            if (current_state == "###")
            {
                qDebug() << "Hotspot without state";
                return false;
            }
            
            int num;
            desc >> num;
            metadata_[current_state].hotspot[0] = num;

            char comma;
            desc >> comma;
            if (comma != ',')
            {
                qDebug() << "Fail to read ',' from .dmi file";
                return false;
            }

            desc >> num;
            metadata_[current_state].hotspot[1] = num;

            desc >> comma;
            if (comma != ',')
            {
                qDebug() << "Fail to read ',' from .dmi file";
                return false;
            }

            desc >> num;
            metadata_[current_state].hotspot[2] = num;

        }
        else
        {
            qDebug() << "Unknown param: " << loc;
            kv_abort();
            return false;
        }
        loc.clear();
        desc >> loc;
    }

    qDebug() << "End of states";

    loc.clear();
    desc >> loc;
    if (loc != "END")
    {
        qDebug() << "Fail to read 'END' from .dmi file";
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "DMI")
    {
        qDebug() << "Fail to read 'DMI' from .dmi file";
        return false;
    }

    qDebug() << "Begin make sequence";

    MakeSequence();

    qDebug() << "End make sequence";

    valid_ = true;
    return true;
}

void ImageMetadata::MakeSequence()
{
    for (auto it = metadata_.begin(); it != metadata_.end(); ++it)
    {
       // qDebug() << "Sequence for: " << it->first << std::endl;
        auto& metadata = it->second.frames_data;
        auto& sequence = it->second.frames_sequence;
        int local_loop = it->second.loop;
        if (it->second.loop == -1 || it->second.loop == 0)
            local_loop = 1;

        for (int loop_i = 0; loop_i < local_loop; ++loop_i)
        {
            for (size_t i = 0; i < metadata.size(); ++i)
            {
                sequence.push_back(i);
            }
            if (it->second.rewind)
            {
                int from = metadata.size() - 2;
                if (from < 0)
                    from = 0;
                for (size_t i = from; i > 0; --i)
                {
                    sequence.push_back(i);
                }
            }
        }
        if (!(it->second.loop == -1 || it->second.loop == 0))
            sequence.push_back(-1);
    }
}
