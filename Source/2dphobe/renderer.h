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
    vec3 ndc;
    vec2 tex_coord;
    float tex_index;
    float local_mat_index;
    vec3 color;
};

struct view_area
{
    glm::vec3 pos;
    glm::vec3 size;
};

class renderer
{
private:
    unsigned int width;
    unsigned int height;

    glm::vec3 cam_view_pos;
    view_area m_view_area;
    float zoom_value;

    unsigned int ssbo;

    unsigned int vao;
    unsigned int vbo;

    std::vector<vertex> vertices;
    std::vector<vertex> screen_vertices;

    unsigned int *textures;
    unsigned int texture_count;
    int max_textures;

    unsigned int* screen_textures;
    unsigned int screen_texture_count;

    shader m_shader;
    shader screen_shader;

    std::vector<glm::mat4> local_mats;
    std::vector<glm::mat4> screen_mats;

    float zoom_point_x;
    float zoom_point_y;
public:
    ~renderer();
    void init(unsigned int width, unsigned int height);
    void draw();
    void push_vert(const vertex& vert);
    void push_screen_vert(const vertex &vert);
    float get_screen_texture_index(float texure_id);
    float get_texture_index(float texure_id);
    int push_screen_mat(const glm::mat4& screen_mats);
    int push_local_mat(const glm::mat4 &local_mat);

    glm::vec3& get_view_pos();
    view_area& get_view_area();
    void set_zoom_point(float x, float y);
    void set_zoom(float zoom);
private:
    void flush(std::vector<vertex>& vert, std::vector<glm::mat4>& mats,
               unsigned int*& t_arr, unsigned int& t_count);
    void m_draw_world();
    void draw_screen();
    void init_vao();
    void init_ssbo();
    void finalize_samplers(shader& shader);
    void finalize_mvp(shader &shader);
    void finalize_screen_mvp();
    void finalize_ssbo(std::vector<glm::mat4>& mat) const;
    void finalize_textures(unsigned int* texture_arr, unsigned int count) const;
    void update_screen_vertices() const;
    void update_vertices() const;
};