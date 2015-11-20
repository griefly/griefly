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

        bool IsTransp(int x, int y, int shift, int angle);
        const GLSprite* GetSprite();
        const ImageMetadata::SpriteMetadata* GetMetadata();
        void Draw(int shift, int x, int y, int angle = 0);
    private:
        const GLSprite* sprite_;
        const ImageMetadata::SpriteMetadata* metadata_;
        int image_state_;
        int last_frame_tick_;
    };
    View2();

    void LoadViewInfo(const ViewInfo& view_info);
private:
    FramesetState base_frameset_;
    std::vector<FramesetState> underlays_;
    std::vector<FramesetState> overlays_;

    int step_x_;
    int step_y_;

    ViewInfo info_;
};
