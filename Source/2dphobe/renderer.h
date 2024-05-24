#pragma once
#include "shader.h"
#include <vector>
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

class renderer
{
private:
    unsigned int width;
    unsigned int height;
    bool geometric;
    glm::vec3 cam_view_pos;
    float zoom_value;

    unsigned int ssbo;

    unsigned int m_vao;
    unsigned int m_vbo;

    unsigned int g_vao;
    unsigned int g_vbo;

    std::vector<vertex> vertices;
    std::vector<g_vertex> g_vertices;

    unsigned int *textures;
    unsigned int texture_count;
    int max_textures;

    shader m_shader;
    shader g_shader;

    std::vector<glm::mat4> local_mats;
public:
    unsigned int draw_call;
    ~renderer();
    void init(unsigned int width, unsigned int height);
    void draw();
    void push_vert(const vertex &vert);
    void g_push_vert(const g_vertex &vert);
    float get_texture_index(float texure_id);
    int push_local_mat(const glm::mat4 &local_mat);

    glm::vec3& get_view_pos();
    void set_zoom(float zoom);

    bool get_geometric_mode();
    void set_geometric_mode(bool g);
    void flush();
private:
    void m_draw();
    void g_draw();
    void init_vao();
    void g_init_vao();
    void init_ssbo();
    void finalize_samplers();
    void finalize_mvp(shader &shader);
    void finalize_ssbo() const;
    void finalize_textures() const;
    void update_vertices() const;
    void g_update_vertices() const;
};