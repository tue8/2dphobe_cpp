#include "quad.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

quad::quad() : texture_id(0)
{
    local_mat = glm::mat4(1.f);
    pos = glm::vec3(0.f);
    size = glm::vec3(100.f, 100.f, 0.f);
}

void quad::load_texture(unsigned int id)
{
    texture_id = id;
}

glm::mat4 quad::get_local_mat()
{
    local_mat = glm::translate(local_mat, pos);
    local_mat = glm::scale(local_mat, size);
    return local_mat;
}

unsigned int quad::get_texture_id() const
{
    return texture_id;
}