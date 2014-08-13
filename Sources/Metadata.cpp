#include "Metadata.h"

#include <fstream>

#include <png.h>

#include "constheader.h"

const ImageMetadata::SpriteMetadata& 
    ImageMetadata::GetSpriteMetadata(const std::string& name)
{
    return metadata_[name];
}

bool ImageMetadata::IsValidState(const std::string& name)
{
    if (!Valid())
        return false;
    return metadata_.find(name) != metadata_.end();
}

const int PNGSIGSIZE = 8;

void userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
    png_voidp a = png_get_io_ptr(pngPtr);
    (static_cast<std::istream*>(a))->
        read(reinterpret_cast<char*>(data), length);
}

void ImageMetadata::Init(const std::string& name)
{
    SYSTEM_STREAM << "Begin to init metadata for " << name << std::endl;
    png_byte pngsig[PNGSIGSIZE];

    std::ifstream source;
    source.open(name, std::fstream::binary);
    if (source.fail()) 
    {
        SYSTEM_STREAM << "ERROR: Fail to open smth" << std::endl;
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

    png_get_IHDR(pngPtr, infoPtr, &total_width_, &total_height_, 0, 0, 0, 0, 0);


    png_textp text_ptr;
    int num_text;

    if (png_get_text(pngPtr, infoPtr, &text_ptr, &num_text) > 0)
    {
        for (int i = 0; i < num_text; ++i)
        {
           /* SYSTEM_STREAM << "BEGIN KEY " << i << std::endl;
            SYSTEM_STREAM << text_ptr[i].key << std::endl;
            SYSTEM_STREAM << "END KEY " << i << std::endl;
            SYSTEM_STREAM << "BEGIN TEXT " << i << std::endl;
            SYSTEM_STREAM << text_ptr[i].text << std::endl;
            SYSTEM_STREAM << "END TEXT " << i << std::endl;
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
    SYSTEM_STREAM << "End load metadata for " << name << std::endl;
}

bool ImageMetadata::ParseDescription(std::stringstream& desc)
{
    std::string loc;
    desc >> loc;
    if (loc != "#")
    {
        SYSTEM_STREAM << "Fail to read '#' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "BEGIN")
    {
        SYSTEM_STREAM << "Fail to read 'BEGIN' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "DMI")
    {
        SYSTEM_STREAM << "Fail to read 'DMI' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "version")
    {
        SYSTEM_STREAM << "Fail to read 'version' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "=")
    {
        SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    ////////////
    desc >> dmi_version_;
    SYSTEM_STREAM << "Read version: " << dmi_version_ << std::endl;
    ////////////
    desc >> loc;
    if (loc != "width")
    {
        SYSTEM_STREAM << "Fail to read 'width' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "=")
    {
        SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    
    //////////////
    desc >> width_;
    SYSTEM_STREAM << "Read width: " << width_ << std::endl;
    /////////////

    desc >> loc;
    if (loc != "height")
    {
        SYSTEM_STREAM << "Fail to read 'height' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "=")
    {
        SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    
    /////////////
    desc >> height_;
    SYSTEM_STREAM << "Read height: " << height_ << std::endl;
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
                SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
                return false;
            }
            loc.clear();
            desc >> loc;
           // SYSTEM_STREAM << "New state: " << loc << std::endl;
            current_state = loc.substr(1, loc.length() - 2);
            metadata_[current_state].first_frame_pos = first_frame_pos;
          //  SYSTEM_STREAM << "First frame position: " << first_frame_pos << std::endl;
        }
        else if (loc == "dirs")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
                return false;
            }
            loc.clear();

            size_t dirs;
            desc >> dirs;
      //      SYSTEM_STREAM << "Dirs: " << dirs << std::endl;

            if (current_state == "###")
            {
                SYSTEM_STREAM << "Dirs without state" << std::endl;
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
                SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
                return false;
            }
            loc.clear();

            size_t frames;
            desc >> frames;
      //      SYSTEM_STREAM << "Frames: " << frames << std::endl;

            if (current_state == "###")
            {
                SYSTEM_STREAM << "Frames without state" << std::endl;
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
                SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
                return false;
            }
            loc.clear();

            if (current_state == "###")
            {
                SYSTEM_STREAM << "Delay without state" << std::endl;
                return false;
            }

            for (size_t i = 0; i < metadata_[current_state].frames_data.size() - 1; ++i)
            {
                size_t value;
                desc >> value;
         //       SYSTEM_STREAM << "Delay " << i << " is: " << value << std::endl;
                metadata_[current_state].frames_data[i].delay = value;

                char comma;
                desc >> comma;
                if (comma != ',')
                {
                    SYSTEM_STREAM << "Fail to read ',' from .dmi file" << std::endl;
                    return false;
                }
            }
            size_t value;
            desc >> value;
     //       SYSTEM_STREAM << "Last delay is: " << value << std::endl;
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
                SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
                return false;
            }
            loc.clear();

            if (current_state == "###")
            {
                SYSTEM_STREAM << "Rewind without state" << std::endl;
                return false;
            }

            size_t rewind;
            desc >> rewind;
     //       SYSTEM_STREAM << "Rewind is: " << rewind << std::endl;
            metadata_[current_state].rewind = rewind ? true : false;
        }
        else if (loc == "loop")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
                return false;
            }
            loc.clear();

            if (current_state == "###")
            {
                SYSTEM_STREAM << "Loop without state" << std::endl;
                return false;
            }

            int loop;
            desc >> loop;
       //     SYSTEM_STREAM << "Loop is: " << loop << std::endl;
            metadata_[current_state].loop = loop;
        }
        else if (loc == "hotspot")
        {
            loc.clear();
            desc >> loc;
            if (loc != "=")
            {
                SYSTEM_STREAM << "Fail to read '=' from .dmi file" << std::endl;
                return false;
            }
            loc.clear();

            if (current_state == "###")
            {
                SYSTEM_STREAM << "Hotspot without state" << std::endl;
                return false;
            }
            
            int num;
            desc >> num;
            metadata_[current_state].hotspot[0] = num;

            char comma;
            desc >> comma;
            if (comma != ',')
            {
                SYSTEM_STREAM << "Fail to read ',' from .dmi file" << std::endl;
                return false;
            }

            desc >> num;
            metadata_[current_state].hotspot[1] = num;

            desc >> comma;
            if (comma != ',')
            {
                SYSTEM_STREAM << "Fail to read ',' from .dmi file" << std::endl;
                return false;
            }

            desc >> num;
            metadata_[current_state].hotspot[2] = num;

        }
        else
        {
            SYSTEM_STREAM << "Unknown param: " << loc << std::endl;
            SDL_Delay(100000);
            return false;
        }
        loc.clear();
        desc >> loc;
    }

    SYSTEM_STREAM << "End of states" << std::endl;

    loc.clear();
    desc >> loc;
    if (loc != "END")
    {
        SYSTEM_STREAM << "Fail to read 'END' from .dmi file" << std::endl;
        return false;
    }
    loc.clear();
    desc >> loc;
    if (loc != "DMI")
    {
        SYSTEM_STREAM << "Fail to read 'DMI' from .dmi file" << std::endl;
        return false;
    }

    SYSTEM_STREAM << "Begin make sequence" << std::endl;

    MakeSequence();

    SYSTEM_STREAM << "End make sequence" << std::endl;

    valid_ = true;
    return true;
}

void ImageMetadata::MakeSequence()
{
    for (auto it = metadata_.begin(); it != metadata_.end(); ++it)
    {
       // SYSTEM_STREAM << "Sequence for: " << it->first << std::endl;
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