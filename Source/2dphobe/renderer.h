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

class renderer
{
private:
    unsigned int width;
    unsigned int height;

    unsigned int ssbo;

    unsigned int vao;
    unsigned int vbo;

    int max_textures;
public:
    void init();
    ~renderer();
    void draw_vertices(std::vector<vertex>& vertices) const;
    int get_max_textures() const;
    void finalize_samplers(shader& shader);
    void finalize_ssbo(const std::vector<glm::mat4>& mat) const;
    void finalize_textures(const std::vector<unsigned int>& texture_arr) const;
    void update_vertices(const std::vector<vertex>& vertices) const;
private:
    void init_vao();
    void init_ssbo();
};