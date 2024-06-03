#pragma once
#include "render_instance.h"

class screen : public render_instance
{
private:
    resolution res;
public:
    void init(renderer* renderer_ptr, resolution res);
    using render_instance::render_instance;
private:
    void finalize_cam() override;
};