#pragma once
#include <glm/glm.hpp>

class obj
{
private:
    unsigned int texture_id;
public:
    glm::vec3 pos;
    glm::vec3 size;
    glm::vec3 color;

    obj();
    bool collide_with(const obj& collide_obj);
    void load_texture(unsigned int id);
    glm::mat4 get_local_mat();
    unsigned int get_texture_id() const;
};