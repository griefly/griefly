#include "Representation.h"

#include "constheader.h"
#include "helpers.h"

#include "Sound.h"

Representation::Representation()
{
    current_frame_ = &first_data_;
    new_frame_ = &second_data_;
    is_updated_ = false;
    current_frame_id_ = 1;
    pixel_movement_tick_ = SDL_GetTicks();
}

void Representation::AddToNewFrame(const Representation::Entity& ent)
{
    new_frame_->entities.push_back(ent);
}

void Representation::AddSoundToNewFrame(const std::string& sound)
{
    new_frame_->sounds.push_back(sound);
}

void Representation::SetMusicForNewFrame(const std::string& music, float volume)
{
    new_frame_->music = music;
    new_frame_->volume = volume;
    if (new_frame_->music == "")
    {
        new_frame_->music = STOP_MUSIC;
    }

    qDebug() << QString::fromStdString(music) << " " << volume;
}

void Representation::SetCameraForFrame(int pos_x, int pos_y)
{
    new_frame_->camera_pos_x = pos_x;
    new_frame_->camera_pos_y = pos_y;
}

void Representation::Swap()
{
    // TODO: mutex
    // Lock(frames_mutex_);
    std::swap(current_frame_, new_frame_);
    is_updated_ = true;
    new_frame_->entities.clear();
    new_frame_->sounds.clear();
}

Representation::Entity::Entity()
{
    id = 0;
    pos_x = 0;
    pos_y = 0;
    vlevel = 0;
    dir = D_DOWN;
}

void Representation::Process()
{
    // TODO: mutex
    // Lock(mutex_);
    SynchronizeViews();

    Draw();

    const int PIXEL_MOVEMENT_SPEED = 16;
    if ((SDL_GetTicks() - pixel_movement_tick_) > PIXEL_MOVEMENT_SPEED)
    {
        PerformPixelMovement();
        camera_.PerformPixelMovement();
        pixel_movement_tick_ = SDL_GetTicks();
    }
}

void Representation::Click(int x, int y)
{

}

void Representation::SynchronizeViews()
{
    if (is_updated_)
    {
        camera_.SetPos(current_frame_->camera_pos_x, current_frame_->camera_pos_y);

        for (auto it = current_frame_->entities.begin(); it != current_frame_->entities.end(); ++it)
        {
            auto& view_with_frame_id = views_[it->id];
            view_with_frame_id.view.LoadViewInfo(it->view);
            if (view_with_frame_id.frame_id != current_frame_id_)
            {
                view_with_frame_id.view.SetX(it->pos_x * 32);
                view_with_frame_id.view.SetY(it->pos_y * 32);
            }
            view_with_frame_id.frame_id = current_frame_id_ + 1;
        } 

        for (auto it = current_frame_->sounds.begin(); it != current_frame_->sounds.end(); ++it)
        {
            GetSoundPlayer().PlaySound(*it);
        }

        // TODO: when frame is dropped there is possibility that music will not be played
        if (current_frame_->music == STOP_MUSIC)
        {
            GetSoundPlayer().StopMusic();
        }
        else if (current_frame_->music != current_music_ && current_frame_->music != "")
        {
            GetSoundPlayer().PlayMusic(current_frame_->music, current_frame_->volume);
            current_music_ = current_frame_->music;
        }
        ++current_frame_id_;
        is_updated_ = false;
    }
}

int get_pixel_speed_for_distance(int distance)
{
    int sign = 0;
    if (distance > 0)
    {
        sign = 1;
    }
    if (distance < 0)
    {
        sign = -1;
    }

    if (sign == 0)
    {
        return 0;
    }

    distance = std::abs(distance);

    return sign * (((distance - 1) / 8) + 1);
}

void Representation::PerformPixelMovement()
{
    for (auto it = current_frame_->entities.begin(); it != current_frame_->entities.end(); ++it)
    {
        int pixel_x = it->pos_x * 32;
        int pixel_y = it->pos_y * 32;

        auto& view_with_frame_id = views_[it->id];
        int old_x = view_with_frame_id.view.GetX();
        int old_y = view_with_frame_id.view.GetY();
        if (old_x != pixel_x)
        {
            int diff_x = pixel_x - old_x;
            diff_x = get_pixel_speed_for_distance(diff_x);
            view_with_frame_id.view.SetX(old_x + diff_x);
        }
        if (old_y != pixel_y)
        {
            int diff_y = pixel_y - old_y;
            diff_y = get_pixel_speed_for_distance(diff_y);
            view_with_frame_id.view.SetY(old_y + diff_y);
        }
    }
}

void Representation::Draw()
{
    for (int vlevel = 0; vlevel < MAX_LEVEL; ++vlevel)
    {
        for (auto it = current_frame_->entities.begin(); it != current_frame_->entities.end(); ++it)
        {
            if (it->vlevel == vlevel)
            {
                views_[it->id].view.Draw(
                    camera_.GetFullShiftX(),
                    camera_.GetFullShiftY(),
                    helpers::dir_to_byond(it->dir));
            }
        }
    }
    for (auto it = current_frame_->entities.begin(); it != current_frame_->entities.end(); ++it)
    {
        if (it->vlevel >= MAX_LEVEL)
        {
            views_[it->id].view.Draw(
                camera_.GetFullShiftX(),
                camera_.GetFullShiftY(),
                helpers::dir_to_byond(it->dir));
        }
    }
}



Representation::Camera::Camera()
{
    pos_x = 0;
    pos_y = 0;
    pixel_shift_x_ = 0;
    pixel_shift_y_ = 0;
}

void Representation::Camera::SetPos(int new_pos_x, int new_pos_y)
{
    pixel_shift_x_ += (pos_x - new_pos_x) * 32;
    pixel_shift_y_ += (pos_y - new_pos_y) * 32;

    pos_x = new_pos_x;
    pos_y = new_pos_y;
}

void Representation::Camera::PerformPixelMovement()
{
    if (pixel_shift_x_ != 0)
    {
        int diff_x = get_pixel_speed_for_distance(pixel_shift_x_);
        pixel_shift_x_ -= diff_x;
    }
    if (pixel_shift_y_ != 0)
    {
        int diff_y = get_pixel_speed_for_distance(pixel_shift_y_);
        pixel_shift_y_ -= diff_y;
    }
}

int Representation::Camera::GetFullShiftX()
{
    return -1 * (pos_x * 32 + pixel_shift_x_) + (sizeW / 2) - 16;
}

int Representation::Camera::GetFullShiftY()
{
    return -1 * (pos_y * 32 + pixel_shift_y_) + (sizeH / 2) - 16;
}

Representation* g_r = nullptr;
Representation& GetRepresentation()
{
    return *g_r;
}

void SetRepresentation(Representation* new_g_r)
{
    g_r = new_g_r;
}
