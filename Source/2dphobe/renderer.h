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

struct g_vertex
{
    struct vec3 ndc;
    float local_mat_index;
    struct vec3 color;
};

template <typename T>
struct simple_arr
{
    T *arr;
    unsigned int count;
};

class renderer
{
private:
    bool geometric;

    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ssbo;

    unsigned int g_vao;
    unsigned int g_vbo;
    unsigned int g_ssbo;

    simple_arr<vertex> vertices;
    simple_arr<g_vertex> g_vertices;

    unsigned int *textures;
    unsigned int texture_count;
    int max_textures;

    shader m_shader;
    shader g_shader;

    unsigned int width;
    unsigned int height;

    glm::vec3 cam_view_pos;

    simple_arr<glm::mat4> local_mats;
    simple_arr<glm::mat4> g_local_mats;
public:
    ~renderer();
    void init(unsigned int width, unsigned int height);
    void draw();
    void flush(bool reset_geometric_mode);
    void push_vert(const vertex &vert);
    void push_g_vert(const g_vertex &vert);
    float get_texture_index(float texure_id);
    int push_local_mat(const glm::mat4 local_mat);
    int push_g_local_mat(const glm::mat4 local_mat);
    glm::vec3& get_view_pos();
    bool get_geometric_mode();
    void set_geometric_mode(bool g);
private:
    void init_vao();
    void init_g_vao();
    void init_ssbo();
    void init_g_ssbo();
    void finalize_samplers();
    void finalize_mvp(shader &shader);
    void finalize_ssbo() const;
    void finalize_g_ssbo() const;
    void finalize_textures() const;
    void update_vertices() const;
    void update_g_vertices() const;
};