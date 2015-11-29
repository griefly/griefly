#include "GraphicRepresentation.h"


void GraphicRepresentation::Draw()
{
    // TODO: mutex
    // Lock(mutex_);

    SynchronizeViews();

}

void GraphicRepresentation::SynchronizeViews()
{
    if (is_updated_)
    {
        for (auto it = current_frame_->begin(); it != current_frame_->end(); ++it)
        {
            auto& view_with_frame_id = views_[it->id];
            view_with_frame_id.view.LoadViewInfo(it->view);
            if (view_with_frame_id.frame_id == current_frame_id_)
            {
                // TODO: check for pixel movement
            }
        } 
        ++current_frame_id_;
    }
}
