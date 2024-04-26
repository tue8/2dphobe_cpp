#pragma once
#include "shader.h"

#include <glm/glm.hpp>

struct vec2
{
    float x, y;
};

struct vec3
{
    float x, y, z;
};


struct vertex
{
    struct vec3 ndc;
    struct vec2 tex_coord;
    float tex_index;
    float local_mat_index;
    struct vec3 color;
};

class renderer
{
private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ssbo;

    vertex *vertices;
    unsigned int vert_count;


    unsigned int *textures;
    unsigned int texture_count;
    int max_textures;

    shader default_shader;

    unsigned int width;
    unsigned int height;

    glm::vec3 cam_view_pos;

    glm::mat4 *local_mats;
    int local_mats_count;
public:
    ~renderer();
    void init(unsigned int width, unsigned int height);
    void draw();
    void push_vert(const vertex &vert);
    float get_texture_index(float texure_id);
    int push_local_mat(const glm::mat4 local_mat);
    glm::vec3& get_view_pos();
private:
    void init_vao();
    void init_ssbo();
    void finalize_samplers();
    void finalize_mvp();
    void finalize_ssbo() const;
    void finalize_textures() const;
    void update_vertices() const;
    void flush();
};