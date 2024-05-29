#pragma once
#include "shader.h"
#include <vector>
#include <glm/glm.hpp>

constexpr int quad_vertices = 6;
constexpr int max_quad = 3000;

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
    vec2 texture_coords;
    float texture_index;
    float mat_index;
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

    std::vector<vertex> world_vertices;
    std::vector<vertex> screen_vertices;

    std::vector<unsigned int> world_textures;
    std::vector<unsigned int> screen_textures;

    int max_textures;

    shader world_shader;
    shader screen_shader;

    std::vector<glm::mat4> world_mats;
    std::vector<glm::mat4> screen_mats;

    float zoom_point_x;
    float zoom_point_y;
public:
    ~renderer();
    void init(unsigned int width, unsigned int height);
    void draw();
    void push_world_vert(const vertex &vert);
    void push_screen_vert(const vertex &vert);
    float get_screen_texture_index(const float texure_id);
    float get_world_texture_index(const float texure_id);
    int push_screen_mat(const glm::mat4 &screen_mat);
    int push_world_mat(const glm::mat4 &world_mat);

    glm::vec3& get_view_pos();
    view_area& get_view_area();
    void set_zoom_point(float x, float y);
    void set_zoom(float zoom);
private:
    void flush_world();
    void flush_screen();
    void draw_world();
    void draw_screen();
    void init_vao();
    void init_ssbo();
    float get_texture_index(std::vector<unsigned int>& t_arr, const float texture_id);
    void finalize_samplers(shader& shader);
    void finalize_world_mvp();
    void finalize_screen_mvp();
    void finalize_ssbo(const std::vector<glm::mat4>& mat) const;
    void finalize_textures(const std::vector<unsigned int>& texture_arr) const;
    void update_screen_vertices() const;
    void update_world_vertices() const;
    void update_vertices(const std::vector<vertex>& vertices) const;
};