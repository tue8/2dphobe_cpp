#pragma once
#include "render_instance.h"

struct view_area
{
    glm::vec3 pos;
    glm::vec3 size;
};

class world : public render_instance
{
private:
    glm::vec3 cam_view_pos;
    view_area m_view_area;
    float zoom_value;
    float zoom_point_x;
    float zoom_point_y;
    resolution res;
public:
    void init(renderer* renderer_ptr, resolution res);
    glm::vec3 get_view_pos();
    void set_view_pos(glm::vec3 pos);
    view_area get_view_area();
    void set_zoom_point(float x, float y);
    void set_zoom(float zoom);
private:
	void finalize_cam() override;
};