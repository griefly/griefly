#include "View.h"

#include <SDL.h>

#include "ASpritesClass.h"
#include "StreamWrapper.h"

View::Frameset::Frameset()
{
    step_x_ = 0;
    step_y_ = 0;
    sprite_name_ = "";
    sprite_ = nullptr;

    state_ = "";
    metadata_ = nullptr;
    image_state_ = 0;
    last_frame_tick_ = SDL_GetTicks();
}

void View::Frameset::SetSprite(const std::string& name)
{
    sprite_name_ = name;
    if (!GetSpriter())
        return;
    sprite_ = GetSpriter()->returnSpr(name);
    SetState(state_);
}

const GLSprite* View::Frameset::GetSprite()
{
    if (sprite_ == nullptr)
        SetSprite(sprite_name_);
    return sprite_;
}

void View::Frameset::SetState(const std::string& name)
{
    state_ = name;
    if (!GetSprite())
        return;
    if (GetSprite()->Fail())
        return;
    if (!GetSprite()->GetSDLSprite()->metadata.IsValidState(state_))
        return;

    metadata_ = &GetSprite()->GetSDLSprite()->metadata.GetSpriteMetadata(state_);
    
    image_state_ = 0;
    last_frame_tick_ = SDL_GetTicks();
}

const ImageMetadata::SpriteMetadata* View::Frameset::GetMetadata()
{  
    if (!metadata_)
        SetState(state_);
    return metadata_;
}

const int ANIMATION_MUL = 100;

void View::Frameset::Draw(int shift, int x, int y)
{
    if (NODRAW)
        return;

    if (!GetSprite() || GetSprite()->Fail() || !GetMetadata())
        return;

    x += step_x_;
    y += step_y_;

    int current_frame = GetMetadata()->frames_sequence[image_state_];
    int current_frame_pos = GetMetadata()->first_frame_pos + current_frame * GetMetadata()->dirs + shift;

    int image_state_h_ = current_frame_pos / GetSprite()->FrameW();
    int image_state_w_ = current_frame_pos % GetSprite()->FrameW();

    GetScreen()->Draw(GetSprite(), 
                      x, y, 
                      image_state_w_, image_state_h_);

    if (GetMetadata()->frames_sequence.size() == 1)
        return;
    if (GetMetadata()->frames_sequence
            [(image_state_ + 1) % GetMetadata()->frames_sequence.size()]
                == -1)
                    return;

    int time_diff = SDL_GetTicks() - last_frame_tick_;
    
    int next_state = image_state_;
    while (true)
    {
        // TODO: lags when time_diff very big
        int frame = GetMetadata()->frames_sequence[next_state];
        time_diff -= GetMetadata()->frames_data[frame].delay * ANIMATION_MUL;
        if (time_diff <= 0)
        {
            if (image_state_ == next_state)
                break;
            image_state_ = next_state;
            last_frame_tick_ = SDL_GetTicks();
            break;
        }
        next_state = (next_state + 1) % GetMetadata()->frames_sequence.size();
    }
}

bool View::Frameset::IsTransp(int x, int y, int shift)
{
    if (NODRAW)
        return true;
    if (!GetMetadata())
        return true;

    x += step_x_;
    y += step_y_;

    const CSprite* loc = GetSprite()->GetSDLSprite();
    if (y >= loc->h || x >= loc->w || x < 0 || y < 0)
        return true;

    int current_frame = GetMetadata()->frames_sequence[image_state_];
    int current_frame_pos = GetMetadata()->first_frame_pos + current_frame * GetMetadata()->dirs + shift;

    int image_state_h_ = current_frame_pos / GetSprite()->FrameW();
    int image_state_w_ = current_frame_pos % GetSprite()->FrameW();

    SDL_Surface* surf = loc->frames[image_state_w_ * loc->numFrameH + image_state_h_];

    auto bpp = surf->format->BytesPerPixel;

    Uint8 un1, un2, un3, alpha;

    SDL_GetRGBA(static_cast<Uint32*>(surf->pixels)[y * surf->pitch / bpp + x], surf->format, &un1, &un2, &un3, &alpha);

    return alpha < 1;
}

View::View()
{
    step_x_ = 0;
    step_y_ = 0;
}

bool View::IsTransp(int x, int y, int shift) 
{
    for (auto it = overlays_.rbegin(); it != overlays_.rend(); ++it)
        if (!it->IsTransp(x + GetStepX(), y + GetStepY(), shift))
            return false;
    if (!GetBaseFrameset()->IsTransp(x + GetStepX(), y + GetStepY(), shift))
        return false;
    for (auto it = underlays_.begin(); it != underlays_.end(); ++it)
        if (!it->IsTransp(x + GetStepX(), y + GetStepY(), shift))
            return false;
    return true;
}
void View::Draw(int shift, int x, int y) 
{ 
    for (auto it = underlays_.rbegin(); it != underlays_.rend(); ++it)
        it->Draw(shift, x + GetStepX(), y + GetStepY());
    GetBaseFrameset()->Draw(shift, x + GetStepX(), y + GetStepY());
    for (auto it = overlays_.begin(); it != overlays_.end(); ++it)
        it->Draw(shift, x + GetStepX(), y + GetStepY());
}

void View::AddOverlay(const std::string& sprite, const std::string& state)
{
    Frameset f;
    f.SetSprite(sprite);
    f.SetState(state);
    overlays_.push_back(f);
}
void View::AddUnderlay(const std::string& sprite, const std::string& state)
{
    Frameset f;
    f.SetSprite(sprite);
    f.SetState(state);
    underlays_.push_back(f);
}

void View::RemoveOverlays()
{
    overlays_.clear();
}
void View::RemoveUnderlays()
{
    underlays_.clear();
}

std::ostream& operator<<(std::stringstream& file, View& view)
{
    WrapWriteMessage(file, *view.GetBaseFrameset());
    file << " ";

    file << view.underlays_.size();
    file << " ";
    for (auto it = view.underlays_.begin(); it != view.underlays_.end(); ++it)
    {
            WrapWriteMessage(file, *it);
            file << " ";
    }

    file << view.overlays_.size();
    file << " ";
    for (auto it = view.overlays_.begin(); it != view.overlays_.end(); ++it)
    {
            WrapWriteMessage(file, *it);
            file << " ";
    }

    return file;
}
std::istream& operator>>(std::stringstream& file, View& view)
{
    WrapReadMessage(file, *view.GetBaseFrameset());

    size_t u_size;
    file >> u_size;
    view.underlays_.resize(u_size);
    for (size_t i = 0; i < u_size; ++i)
    {
        View::Frameset f;
        WrapReadMessage(file, f);
        view.underlays_[i] = f;
    }

    size_t o_size;
    file >> o_size;
    view.overlays_.resize(o_size);
    for (size_t i = 0; i < o_size; ++i)
    {
        View::Frameset f;
        WrapReadMessage(file, f);
        view.overlays_[i] = f;
    }

    view.step_x_ = 0;
    view.step_y_ = 0;

    return file;
}

std::ostream& operator<<(std::stringstream& file, View::Frameset& frameset)
{
    WrapWriteMessage(file, frameset.sprite_name_);
    file << " ";
    WrapWriteMessage(file, frameset.state_);
    file << " ";
    return file;
}
std::istream& operator>>(std::stringstream& file, View::Frameset& frameset)
{
    frameset.step_x_ = 0;
    frameset.step_y_ = 0;
    frameset.sprite_ = nullptr;

    frameset.metadata_ = nullptr;
    frameset.image_state_ = 0;
    frameset.last_frame_tick_ = SDL_GetTicks();

    WrapReadMessage(file, frameset.sprite_name_);
    WrapReadMessage(file, frameset.state_);
    return file;
}