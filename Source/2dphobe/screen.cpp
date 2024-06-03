#include "screen.h"
#include <glm/gtc/matrix_transform.hpp>

void screen::init(renderer* renderer_ptr, resolution res)
{
    this->res = res;
    render_instance::init(renderer_ptr, WORLD);
}

void screen::finalize_cam()
{
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 proj = glm::ortho(0.f, (float)res.width, (float)res.height, 0.f, -1.f, 1.f);
    render_instance::set_vp(view, proj);
}