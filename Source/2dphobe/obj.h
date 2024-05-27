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
    obj(glm::vec3 pos, glm::vec3 size, glm::vec3 color);
    bool overlaps(const obj& o) const;

    /*
    * make the container bigger in order to remove 
    * ugly tile appearing/disappearing effect
    */
    bool contains_tile(const obj& o, float multiplier) const;
    bool contains(const obj& o) const;
    void load_texture(unsigned int id);
    glm::mat4 get_local_mat();
    unsigned int get_texture_id() const;
};