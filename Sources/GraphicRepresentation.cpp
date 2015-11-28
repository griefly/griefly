#include "GraphicRepresentation.h"


void GraphicRepresentation::Process()
{
    // TODO: mutex
    // Lock(mutex_);
    if (is_updated_)
    {

    }
}

void GraphicRepresentation::LoadViewsFromFrame()
{
    for (auto it = current_frame_->begin(); it != current_frame_->end(); ++it)
    {
        views_[it->id].LoadViewInfo(it->view);
    }
}
