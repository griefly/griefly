#include "View2.h"

#include "SpriteHolder.h"
#include <QColor>
#include <cmath>

namespace
{

bool IsSameSprites(
    const kv::RawViewInfo::RawFramesetInfo& left,
    const kv::RawViewInfo::RawFramesetInfo& right)
{
    if (left.sprite_name != right.sprite_name)
    {
        return false;
    }
    if (left.state != right.state)
    {
        return false;
    }
    if (left.angle != right.angle)
    {
        return false;
    }
    if (left.shift_x != right.shift_x)
    {
        return false;
    }
    if (left.shift_y != right.shift_y)
    {
        return false;
    }
    return true;
}

bool IsSameFramesets(const kv::RawViewInfo& left, const kv::RawViewInfo& right)
{
    if (left.underlays.size() != right.underlays.size())
    {
        return false;
    }
    if (left.overlays.size() != right.overlays.size())
    {
        return false;
    }

    if (left.angle != right.angle)
    {
        return false;
    }

    if (left.transparency != right.transparency)
    {
        return false;
    }

    if (!IsSameSprites(left.base_frameset, right.base_frameset))
    {
        return false;
    }

    for (int i = 0; i < left.underlays.size(); ++i)
    {
        if (!IsSameSprites(left.underlays[i], right.underlays[i]))
        {
            return false;
        }
    }
    for (int i = 0; i < left.overlays.size(); ++i)
    {
        if (!IsSameSprites(left.overlays[i], right.overlays[i]))
        {
            return false;
        }
    }
    return true;
}

}

View2::FramesetState::FramesetState()
{
    Reset();
    last_frame_tick_.start();
}

void View2::FramesetState::LoadFramesetInfo(const kv::RawViewInfo::RawFramesetInfo& frameset_info)
{
    Reset();

    if (!IsSpriterValid())
    {
        return;
    }
    sprite_ = GetSpriter().GetSprite(frameset_info.sprite_name);
    if (sprite_ == nullptr)
    {
        return;
    }
    if (sprite_->Fail())
    {
        return;
    }
    if (!sprite_->GetMetadata().IsValidState(frameset_info.state))
    {
        return;
    }
    metadata_ = &sprite_->GetMetadata().GetSpriteMetadata(frameset_info.state);
}

bool View2::FramesetState::IsTransp(int x, int y, int shift, int angle) const
{
    if (!GetMetadata())
    {
        return true;
    }

    if (shift >= static_cast<int>(GetMetadata()->dirs))
    {
        shift = 0;
    }

    float true_angle = (angle * 3.14f) / 180;

    float sin_a = sin(static_cast<float>(1 * true_angle));
    float cos_a = cos(static_cast<float>(1 * true_angle));

    x -= GetSprite()->W() / 2;
    y -= GetSprite()->H() / 2;

    int new_x = static_cast<int>(     cos_a * x + sin_a * y);
    int new_y = static_cast<int>(-1 * sin_a * x + cos_a * y);

    x = new_x + GetSprite()->W() / 2;
    y = new_y + GetSprite()->H() / 2;

    if (   y >= GetSprite()->H()
        || x >= GetSprite()->W()
        || x < 0
        || y < 0)
    {
        return true;
    }

    //qDebug() << "Frameset: " << x << ", " << y;

    int current_frame = GetMetadata()->frames_sequence[image_state_];
    int current_frame_pos = GetMetadata()->first_frame_pos + current_frame * GetMetadata()->dirs + shift;

    int image_state_h_ = current_frame_pos / GetSprite()->FrameW();
    int image_state_w_ = current_frame_pos % GetSprite()->FrameW();

    QImage image = GetSprite()->GetFrames()[image_state_w_ * GetSprite()->FrameH() + image_state_h_];

    QRgb pixel = image.pixel(x, y);
    return qAlpha(pixel) < 1;
}

namespace
{
    const int ANIMATION_MUL = 100;
}

void View2::FramesetState::Draw(quint32 shift, int x, int y, int angle, int transparency)
{
    if (!GetSprite() || GetSprite()->Fail() || !GetMetadata())
    {
        return;
    }

    if (shift >= GetMetadata()->dirs)
    {
        shift = 0;
    }

    int current_frame = GetMetadata()->frames_sequence[image_state_];
    int current_frame_pos = GetMetadata()->first_frame_pos + current_frame * GetMetadata()->dirs + shift;

    int image_state_h_ = current_frame_pos / GetSprite()->FrameW();
    int image_state_w_ = current_frame_pos % GetSprite()->FrameW();

    GetScreen().Draw(
        GetSprite(), x, y,
        image_state_w_, image_state_h_,
        static_cast<float>(angle),
        static_cast<float>(transparency) / MAX_TRANSPARENCY);

    if (GetMetadata()->frames_sequence.size() == 1)
    {
        return;
    }
    if (GetMetadata()->frames_sequence
            [(image_state_ + 1) % GetMetadata()->frames_sequence.size()]
                == -1)
    {
        return;
    }

    int time_diff = last_frame_tick_.elapsed();

    int next_state = image_state_;
    while (true)
    {
        // TODO: lags when time_diff very big
        int frame = GetMetadata()->frames_sequence[next_state];
        time_diff -= GetMetadata()->frames_data[frame].delay * ANIMATION_MUL;
        if (time_diff <= 0)
        {
            if (image_state_ == next_state)
            {
                break;
            }
            image_state_ = next_state;
            last_frame_tick_.restart();
            break;
        }
        next_state = (next_state + 1) % GetMetadata()->frames_sequence.size();
    }
}

void View2::FramesetState::Reset()
{
    sprite_ = nullptr;
    metadata_ = nullptr;
    image_state_ = 0;
    last_frame_tick_.restart();
}

View2::View2()
{
    pixel_x_ = 0;
    pixel_y_ = 0;
}

bool View2::IsTransp(int x, int y, quint32 shift) const
{
    //qDebug() << GetX() << "," << GetY();
    for (int i = overlays_.size() - 1; i >= 0; --i)
    {
        int sum_angle = info_.angle + info_.overlays[i].angle;
        if (!overlays_[i].IsTransp(x - GetX(), y - GetY(), shift, sum_angle))
        {
            return false;
        }
    }
    {
        int sum_angle = info_.angle + info_.base_frameset.angle;
        if (!base_frameset_.IsTransp(x - GetX(), y - GetY(), shift, sum_angle))
        {
            return false;
        }
    }
    for (int i = 0; i < static_cast<int>(underlays_.size()); ++i)
    {
        int sum_angle = info_.angle + info_.underlays[i].angle;
        if (!underlays_[i].IsTransp(x - GetX(), y - GetY(), shift, sum_angle))
        {
            return false;
        }
    }
    return true;
}

void View2::Draw(int x_shift, int y_shift, quint32 shift)
{
    int transparency = info_.transparency;
    for (int i = underlays_.size() - 1; i >= 0; --i)
    {
        const auto& underlay = info_.underlays[i];
        int sum_angle = info_.angle + underlay.angle;
        int sum_x = GetX() + x_shift + underlay.shift_x;
        int sum_y = GetY() + y_shift + underlay.shift_y;
        underlays_[i].Draw(shift, sum_x, sum_y, sum_angle, transparency);
    }
    {
        int sum_angle = info_.angle + info_.base_frameset.angle;
        base_frameset_.Draw(shift, GetX() + x_shift, GetY() + y_shift, sum_angle, transparency);
    }
    for (int i = 0; i < static_cast<int>(overlays_.size()); ++i)
    {
        const auto& overlay = info_.overlays[i];
        int sum_angle = info_.angle + overlay.angle;
        int sum_x = GetX() + x_shift + overlay.shift_x;
        int sum_y = GetY() + y_shift + overlay.shift_y;
        overlays_[i].Draw(shift, sum_x, sum_y, sum_angle, transparency);
    }
}

void View2::LoadViewInfo(const kv::RawViewInfo& view_info)
{
    if (IsSameFramesets(view_info, info_))
    {
        return;
    }

    //qDebug() << "Not same framesets "
    //         << QString::fromStdString(view_info.GetBaseFrameset().GetSprite());

    if (!IsSameSprites(
            view_info.base_frameset,
            info_.base_frameset))
    {
        base_frameset_.LoadFramesetInfo(view_info.base_frameset);
    }

    {
        const auto& new_overlays = view_info.overlays;
        overlays_.resize(new_overlays.size());
        unsigned int counter = 0;
        unsigned int intermediate_size = std::min(info_.overlays.size(), new_overlays.size());
        for (; counter < intermediate_size; ++counter)
        {
            if (!IsSameSprites(
                    new_overlays[counter],
                    info_.overlays[counter]))
            {
                overlays_[counter].LoadFramesetInfo(new_overlays[counter]);
            }
        }
        for (; counter < new_overlays.size(); ++counter)
        {
            overlays_[counter].LoadFramesetInfo(new_overlays[counter]);
        }
    }

    {
        const auto& new_underlays = view_info.underlays;
        underlays_.resize(new_underlays.size());
        unsigned int counter = 0;
        unsigned int intermediate_size = std::min(info_.underlays.size(), new_underlays.size());
        for (; counter < intermediate_size; ++counter)
        {
            if (!IsSameSprites(
                    new_underlays[counter],
                    info_.underlays[counter]))
            {
                underlays_[counter].LoadFramesetInfo(new_underlays[counter]);
            }
        }
        for (; counter < new_underlays.size(); ++counter)
        {
            underlays_[counter].LoadFramesetInfo(new_underlays[counter]);
        }
    }

    info_ = view_info;
}
