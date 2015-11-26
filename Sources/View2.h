#pragma once

#include <vector>

#include "ViewInfo.h"
#include "Metadata.h"
#include "GLSprite.h"

class View2
{
public:
    class FramesetState
    {
    public:
        FramesetState();

        void LoadFramesetInfo(const ViewInfo::FramesetInfo& frameset_info);

        const GLSprite* GetSprite() { return sprite_; }
        const ImageMetadata::SpriteMetadata* GetMetadata() { return metadata_; }

        bool IsTransp(int x, int y, int shift, int angle);
        void Draw(int shift, int x, int y, int angle = 0);
    private:
        void Reset();

        const GLSprite* sprite_;
        const ImageMetadata::SpriteMetadata* metadata_;
        int image_state_;
        int last_frame_tick_;
    };
    View2();

    void SetStepX(int new_step_x)
    {
        step_x_ = new_step_x;
    }
    void SetStepY(int new_step_y)
    {
        step_y_ = new_step_y;
    }
    int GetStepX() const { return step_x_; }
    int GetStepY() const { return step_y_; }

    bool IsTransp(int x, int y, int shift);
    void Draw(int shift, int x, int y);

    void LoadViewInfo(const ViewInfo& view_info);
private:
    FramesetState base_frameset_;
    std::vector<FramesetState> underlays_;
    std::vector<FramesetState> overlays_;

    int step_x_;
    int step_y_;

    ViewInfo info_;
};
