#include "world.h"
#include <glm/gtc/matrix_transform.hpp>

void world::init(renderer* renderer_ptr, resolution res)
{
    this->res = res;

    cam_view_pos = glm::vec3(0.f, 0.f, 1.f);
    m_view_area.pos = glm::vec3(0.f);
    m_view_area.size = glm::vec3(res.width, res.height, 0.f);

    zoom_point_x = res.width / 2;
    zoom_point_y = res.height / 2;

    render_instance::init(renderer_ptr, WORLD);
}

glm::vec3 world::get_view_pos()
{
    return cam_view_pos;
}

void world::set_view_pos(glm::vec3 pos)
{
    cam_view_pos = pos;
}

view_area world::get_view_area()
{
    return m_view_area;
}

void world::set_zoom_point(float x, float y)
{
    zoom_point_x = x;
    zoom_point_y = y;
}

void world::set_zoom(float zoom)
{
    zoom_value = zoom;
}

void world::finalize_cam()
{
    glm::mat4 view = glm::mat4(1.f),
        proj = glm::mat4(1.f),
        zoom = glm::mat4(1.f);

    view = glm::translate(view, cam_view_pos);

    static float left = 0.f;
    static float right = res.width;
    static float bottom = res.height;
    static float top = 0.f;


    left = (0 - zoom_point_x) / zoom_value + zoom_point_x;
    right = (res.width - zoom_point_x) / zoom_value + zoom_point_x;
    bottom = (res.height - zoom_point_y) / zoom_value + zoom_point_y;
    top = (0 - zoom_point_y) / zoom_value + zoom_point_y;


    m_view_area.pos.x = left - cam_view_pos.x;
    m_view_area.pos.y = top - cam_view_pos.y;
    m_view_area.size.x = (right - cam_view_pos.x) - m_view_area.pos.x;
    m_view_area.size.y = (bottom - cam_view_pos.y) - m_view_area.pos.y;
    proj = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    render_instance::set_vp(view, proj);
}