#include "obj.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

obj::obj() : texture_id(0)
{
    pos = glm::vec3(0.f);
    size = glm::vec3(100.f, 100.f, 0.f);
    color = glm::vec3(1.f);
}

bool obj::collide_with(const obj& collide_obj)
{
    glm::vec3 collide_pos = collide_obj.pos;
    glm::vec3 collide_size = collide_obj.size;

    bool x_overlap = (pos.x + size.x > collide_pos.x &&
                      collide_pos.x + collide_size.x > pos.x);

    bool y_overlap = (pos.y + size.y > collide_pos.y &&
                      collide_pos.y + collide_size.y > pos.y);

    return (x_overlap && y_overlap);
}

void obj::load_texture(unsigned int id)
{
    texture_id = id;
}

glm::mat4 obj::get_local_mat()
{
    glm::mat4 local_mat;
    local_mat = glm::translate(glm::mat4(1.f), pos);
    local_mat = glm::scale(local_mat, size);
    return local_mat;
}

unsigned int obj::get_texture_id() const
{
    return texture_id;
}