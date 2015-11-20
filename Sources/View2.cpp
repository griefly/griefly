#include "View2.h"

View2::FramesetState::FramesetState()
{
    sprite_ = nullptr;
    metadata_ = nullptr;
    image_state_ = 0;
    last_frame_tick_ = 0;
}

View2::View2()
{
    step_x_ = 0;
    step_y_ = 0;
}
