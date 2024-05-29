#include "renderer.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

/* Camera */
glm::vec3& renderer::get_view_pos()
{
    return cam_view_pos;
}

view_area &renderer::get_view_area()
{
    return m_view_area;
}

void renderer::set_zoom_point(float x, float y)
{
    zoom_point_x = x;
    zoom_point_y = y;
}

void renderer::set_zoom(float zoom)
{
    zoom_value = zoom;
}

/**/

void renderer::init_vao()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertex) * quad_vertices * max_quad,
        nullptr,
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void*)offsetof(vertex, ndc));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void*)offsetof(vertex, texture_coords));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void*)offsetof(vertex, texture_index));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void*)offsetof(vertex, mat_index));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void*)offsetof(vertex, color));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderer::init_ssbo()
{
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        sizeof(glm::mat4) * max_quad,
        NULL,
        GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void renderer::init(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_textures);

    world_shader.init_world_shader(max_textures);
    screen_shader.init_screen_shader(max_textures);

    init_vao();
    init_ssbo();

    cam_view_pos = glm::vec3(0.f, 0.f, 1.f);
    m_view_area.pos = glm::vec3(0.f);
    m_view_area.size = glm::vec3(width, height, 0.f);

    zoom_point_x = width / 2;
    zoom_point_y = height / 2;
}

float renderer::get_texture_index(std::vector<unsigned int> &t_arr, float texture_id)
{
    float tex_index;
    bool tex_find = false;

    for (int i = 0; i < t_arr.size(); i++)
    {
        if (t_arr.at(i) == texture_id)
        {
            tex_index = (float)i;
            tex_find = true;
            break;
        }
    }

    if (!tex_find)
    {
        if (t_arr.size() > max_textures - 1)
            draw();
        t_arr.push_back((unsigned int)texture_id);
        tex_index = (float)t_arr.size() - 1;
    }

    return tex_index;
}

void renderer::finalize_samplers(shader &shader)
{
    int *samplers = new int[max_textures];

    for (int i = 0; i < max_textures; i++)
        samplers[i] = i;

    shader.bind();
    int location = glGetUniformLocation(shader.get_id(),
                                        "texs");

    glUniform1iv(location, max_textures, samplers);
    shader.unbind();
    delete[] samplers;
}

void renderer::finalize_ssbo(const std::vector<glm::mat4> &mat) const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    0,
                    mat.size() * sizeof(glm::mat4),
                    mat.data());
}

void renderer::finalize_textures(const std::vector<unsigned int>& t_arr) const
{
    for (int i = 0; i < t_arr.size(); i++)
        glBindTextureUnit(i, t_arr.at(i));
}

void renderer::update_vertices(const std::vector<vertex>& vertices) const
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    vertices.size() * sizeof(vertex),
                    vertices.data());
    glBindVertexArray(0);
}

/*
* We don't need to handle vertices exceed limit
* since push_world_mat() already did that.
*/
void renderer::push_world_vert(const vertex& vert)
{
    world_vertices.push_back(vert);
}

void renderer::push_screen_vert(const vertex& vert)
{
    screen_vertices.push_back(vert);
}

int renderer::push_world_mat(const glm::mat4& world_mat)
{
    if (world_mats.size() + 1 >= max_quad)
        draw_world();
    world_mats.push_back(world_mat);
    return world_mats.size() - 1;
}

int renderer::push_screen_mat(const glm::mat4& screen_mat)
{
    if (screen_mats.size() + 1 >= max_quad)
        draw_screen();
    screen_mats.push_back(screen_mat);
    return screen_mats.size() - 1;
}

float renderer::get_world_texture_index(const float texture_id)
{
    return get_texture_index(world_textures, texture_id);
}

float renderer::get_screen_texture_index(const float texture_id)
{
    return get_texture_index(screen_textures, texture_id);
}

void renderer::update_world_vertices() const
{
    update_vertices(world_vertices);
}

void renderer::update_screen_vertices() const
{
    update_vertices(screen_vertices);
}

void renderer::finalize_world_mvp()
{
    world_shader.bind();
    glm::mat4 view = glm::mat4(1.f),
        proj = glm::mat4(1.f),
        zoom = glm::mat4(1.f);

    view = glm::translate(view, cam_view_pos);

    float zoom_point_x = width / 2.f;
    float zoom_point_y = height / 2.f;

    static float left = 0.f;
    static float right = width;
    static float bottom = height;
    static float top = 0.f;

    if (zoom_value > 0)
    {
        left = (0 - zoom_point_x) / zoom_value + zoom_point_x;
        right = (width - zoom_point_x) / zoom_value + zoom_point_x;
        bottom = (height - zoom_point_y) / zoom_value + zoom_point_y;
        top = (0 - zoom_point_y) / zoom_value + zoom_point_y;
    }

    m_view_area.pos.x = left - cam_view_pos.x;
    m_view_area.pos.y = top - cam_view_pos.y;
    m_view_area.size.x = (right - cam_view_pos.x) - m_view_area.pos.x;
    m_view_area.size.y = (bottom - cam_view_pos.y) - m_view_area.pos.y;
    proj = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    world_shader.set_mat4("view", view);
    world_shader.set_mat4("proj", proj);
    world_shader.unbind();
}

void renderer::finalize_screen_mvp()
{
    screen_shader.bind();
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 proj = glm::ortho(0.f, (float)width, (float)height, 0.f, -1.f, 1.f);
    screen_shader.set_mat4("view", view);
    screen_shader.set_mat4("proj", proj);
    screen_shader.unbind();
}

void renderer::flush_world()
{
    if (!world_vertices.empty()) world_vertices.clear();
    if (!world_mats.empty()) world_mats.clear();
    if (!world_textures.empty()) world_textures.clear();
}

void renderer::flush_screen()
{
    if (!screen_vertices.empty()) screen_vertices.clear();
    if (!screen_mats.empty()) screen_mats.clear();
    if (!screen_textures.empty()) screen_textures.clear();
}

void renderer::draw_world()
{
    finalize_world_mvp();
    finalize_samplers(world_shader);
    finalize_ssbo(world_mats);
    finalize_textures(world_textures);
    update_world_vertices();
    world_shader.bind();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, world_vertices.size());
    flush_world();
}

void renderer::draw_screen()
{
    finalize_screen_mvp();
    finalize_samplers(screen_shader);
    finalize_ssbo(screen_mats);
    finalize_textures(screen_textures);
    update_screen_vertices();
    screen_shader.bind();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, screen_vertices.size());
    flush_screen();
}

void renderer::draw()
{
    if (world_vertices.size() > 0) draw_world();
    if (screen_vertices.size() > 0) draw_screen();
}

renderer::~renderer()
{
    world_shader.destroy();
    screen_shader.destroy();
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}