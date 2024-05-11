#pragma once
#include <glm/glm.hpp>

class obj
{
private:
    unsigned int texture_id;
    glm::mat4 local_mat;
public:
    glm::vec3 pos;
    glm::vec3 size;
    glm::vec3 color;

    obj();
    void load_texture(unsigned int id);

    /*
    * finalize and return local mat
    */
    glm::mat4 get_local_mat();
    unsigned int get_texture_id() const;
};