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

    const auto& new_overlays = view_info.GetOverlays();


    info_ = view_info;
}
