#include "obj.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

obj::obj() : texture_id(0)
{
    local_mat = glm::mat4(1.f);
    pos = glm::vec3(0.f);
    size = glm::vec3(100.f, 100.f, 0.f);
    color = glm::vec3(0.f);
}

void obj::load_texture(unsigned int id)
{
    texture_id = id;
}

glm::mat4 obj::get_local_mat()
{
    local_mat = glm::translate(local_mat, pos);
    local_mat = glm::scale(local_mat, size);
    return local_mat;
}

unsigned int obj::get_texture_id() const
{
    return texture_id;
}