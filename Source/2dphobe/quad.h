#pragma once
#include <glm/glm.hpp>

class quad
{
private:
    unsigned int texture_id;
    glm::mat4 local_mat;
public:
    glm::vec3 pos;
    glm::vec3 size;

    quad();
    void load_texture(unsigned int id);

    /*
    * finalize and return local mat
    */
    glm::mat4 get_local_mat();
    unsigned int get_texture_id() const;
};