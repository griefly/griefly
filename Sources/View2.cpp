#include "View2.h"

#include "SpriteHolder.h"

View2::FramesetState::FramesetState()
{
    Reset();
}

void View2::FramesetState::LoadFramesetInfo(const ViewInfo::FramesetInfo& frameset_info)
{
    Reset();

    if (!IsSpriterValid())
    {
        return;
    }
    sprite_ = GetSpriter().GetSprite(frameset_info.GetSprite());
    if (sprite_ == nullptr)
    {
        return;
    }
    if (sprite_->Fail())
    {
        return;
    }
    if (sprite_->GetSDLSprite()->metadata.IsValidState(frameset_info.GetState()))
    {
        return;
    }
    metadata_ = &sprite_->GetSDLSprite()->metadata.GetSpriteMetadata(frameset_info.GetState());
}

void View2::FramesetState::Reset()
{
    sprite_ = nullptr;
    metadata_ = nullptr;
    image_state_ = 0;
    last_frame_tick_ = SDL_GetTicks();
}

View2::View2()
{
    step_x_ = 0;
    step_y_ = 0;
}

void View2::LoadViewInfo(const ViewInfo& view_info)
{
    if (ViewInfo::IsSameFramesets(view_info, info_))
    {
        return;
    }

    if (!ViewInfo::FramesetInfo::IsSameSprites(
            view_info.GetBaseFrameset(),
            info_.GetBaseFrameset()))
    {
        base_frameset_.LoadFramesetInfo(view_info.GetBaseFrameset());
    }

    {
        const auto& new_overlays = view_info.GetOverlays();
        overlays_.resize(new_overlays.size());
        size_t counter = 0;
        for (; counter < info_.GetOverlays().size(); ++counter)
        {
            if (!ViewInfo::FramesetInfo::IsSameSprites(
                    new_overlays[counter],
                    info_.GetOverlays()[counter]))
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
        const auto& new_underlays = view_info.GetUnderlays();
        underlays_.resize(new_underlays.size());
        size_t counter = 0;
        for (; counter < info_.GetUnderlays().size(); ++counter)
        {
            if (!ViewInfo::FramesetInfo::IsSameSprites(
                    new_underlays[counter],
                    info_.GetUnderlays()[counter]))
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
