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

obj::obj(glm::vec3 pos = glm::vec3(0.f),
         glm::vec3 size = glm::vec3(100.f, 100.f, 0.f),
         glm::vec3 color = glm::vec3(1.f)) : texture_id(0)
{
    this->pos = pos;
    this->size = size;
    this->color = color;
}

bool obj::overlaps(const obj& o) const
{
    bool x_overlap = (pos.x + size.x >= o.pos.x) &&
                     (o.pos.x + o.size.x > pos.x);

    bool y_overlap = (pos.y + size.y >= o.pos.y) &&
                     (o.pos.y + o.size.y > pos.y);

    return (x_overlap && y_overlap);
}

/*
* make the container bigger in order to remove
* ugly tile appearing/disappearing effect
*/
bool obj::contains_tile(const obj& o, float multiplier) const
{
    bool x_overlap = (pos.x + size.x + (o.size.x * multiplier) >= o.pos.x + o.size.x) &&
                     (pos.x - (o.size.x * multiplier) <= o.pos.x);

    bool y_overlap = (pos.y + size.y + (o.size.y * multiplier) >= o.pos.y + o.size.y) &&
                     (pos.y - (o.size.y * multiplier) <= o.pos.y);

    return (x_overlap && y_overlap);
}

bool obj::contains(const obj& o) const
{
    bool x_overlap = (pos.x + size.x >= o.pos.x + o.size.x) &&
                     (pos.x <= o.pos.x);

    bool y_overlap = (pos.y + size.y >= o.pos.y + o.size.y) &&
                     (pos.y <= o.pos.y);

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