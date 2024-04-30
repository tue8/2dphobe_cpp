#include "renderer.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


#define QUAD_VERT 6
#define MAX_QUAD 1000

void renderer::init_vao()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertex) * QUAD_VERT * MAX_QUAD,
        nullptr,
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void *)offsetof(vertex, ndc));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void *)offsetof(vertex, tex_coord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void *)offsetof(vertex, tex_index));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void *)offsetof(vertex, local_mat_index));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex),
        (void *)offsetof(vertex, color));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderer::init_g_vao()
{
    glGenVertexArrays(1, &g_vao);
    glGenBuffers(1, &g_vbo);
    glBindVertexArray(g_vao);

    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(g_vertex) * QUAD_VERT * MAX_QUAD,
        nullptr,
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(g_vertex),
        (void *)offsetof(g_vertex, ndc));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(g_vertex),
        (void *)offsetof(g_vertex, local_mat_index));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(g_vertex),
        (void *)offsetof(g_vertex, color));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderer::init_ssbo()
{
    glGenBuffers(1, &m_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        sizeof(glm::mat4) * MAX_QUAD,
        NULL,
        GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void renderer::init_g_ssbo()
{
    glGenBuffers(1, &g_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        sizeof(glm::mat4) * MAX_QUAD,
        NULL,
        GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void renderer::init(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;

    vertices.count = 0;
    texture_count = 0;
    local_mats.count = 0;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_textures);

    m_shader.init(max_textures);
    g_shader.init(-1);

    vertices.arr = new vertex[QUAD_VERT * MAX_QUAD];
    g_vertices.arr = new g_vertex[QUAD_VERT * MAX_QUAD];
    textures = new unsigned int[max_textures];
    local_mats.arr = new glm::mat4[MAX_QUAD];
    g_local_mats.arr = new glm::mat4[MAX_QUAD];

    init_vao();
    init_g_vao();
    init_ssbo();
    init_g_ssbo();

    cam_view_pos = glm::vec3(50.f, 50.f, 0.f);
}

/*
* We don't need to handle vertices exceed limit
* since push_local_mat() already did that.
*/
void renderer::push_vert(const vertex &vert)
{
    vertices.arr[vertices.count++] = vert;
}

void renderer::push_g_vert(const g_vertex &vert)
{
    g_vertices.arr[g_vertices.count++] = vert;
}


int renderer::push_local_mat(const glm::mat4 local_mat)
{
    if (local_mats.count + 1 >= MAX_QUAD)
        draw();
    local_mats.arr[local_mats.count] = local_mat;
    return local_mats.count++;
}

int renderer::push_g_local_mat(const glm::mat4 local_mat)
{
    if (g_local_mats.count + 1 >= MAX_QUAD)
        draw();
    g_local_mats.arr[g_local_mats.count] = local_mat;
    return g_local_mats.count++;
}

glm::vec3 &renderer::get_view_pos()
{
    return cam_view_pos;
}

void renderer::flush()
{
    memset(vertices.arr, 0, vertices.count * sizeof(vertex));
    memset(g_vertices.arr, 0, g_vertices.count * sizeof(g_vertex));
    memset(textures, 0, texture_count * sizeof(unsigned int));
    memset(local_mats.arr, 0, local_mats.count * sizeof(glm::mat4));
    memset(g_local_mats.arr, 0, g_local_mats.count * sizeof(glm::mat4));

    vertices.count = 0;
    g_vertices.count = 0;
    texture_count = 0;
    local_mats.count = 0;
    g_local_mats.count = 0;
}


float renderer::get_texture_index(float texure_id)
{
    float tex_index;
    bool tex_find = false;

    for (int i = 0; i < texture_count; i++)
    {
        if (textures[i] == texure_id)
        {
            tex_index = (float)i;
            tex_find = true;
            break;
        }
    }

    if (!tex_find)
    {
        if (texture_count > max_textures - 1)
            draw();
        textures[texture_count] = texure_id;
        tex_index = (float)texture_count++;
    }

    return tex_index;
}


void renderer::finalize_samplers()
{
    int *samplers = new int[max_textures];

    for (int i = 0; i < max_textures; i++)
        samplers[i] = i;

    m_shader.bind();
    int location = glGetUniformLocation(m_shader.get_id(),
                                        "texs");

    glUniform1iv(location, max_textures, samplers);
    m_shader.unbind();
    delete[] samplers;
}

void renderer::finalize_mvp(shader &shader)
{
    shader.bind();
    glm::mat4 view = glm::mat4(1.f),
              proj = glm::mat4(1.f);

    view = glm::translate(view, cam_view_pos);
    proj = glm::ortho(0.f, (float)width, (float)height, 0.f, -1.f, 1.f);

    shader.set_mat4("view", view);
    shader.set_mat4("proj", proj);
    shader.unbind();
}

void renderer::finalize_ssbo() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    0,
                    local_mats.count * sizeof(glm::mat4),
                    local_mats.arr);
}

void renderer::finalize_g_ssbo() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
        0,
        g_local_mats.count * sizeof(glm::mat4),
        g_local_mats.arr);
}

void renderer::finalize_textures() const
{
    for (int i = 0; i < texture_count; i++)
        glBindTextureUnit(i, textures[i]);
}

void renderer::update_vertices() const
{
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    vertices.count * sizeof(vertex),
                    vertices.arr);
    glBindVertexArray(0);
}

void renderer::update_g_vertices() const
{
    glBindVertexArray(g_vao);

    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
        0,
        g_vertices.count * sizeof(g_vertex),
        g_vertices.arr);
    glBindVertexArray(0);
}

void renderer::draw()
{
    finalize_mvp(m_shader);
    finalize_samplers();
    finalize_ssbo();
    finalize_textures();
    update_vertices();

    finalize_mvp(g_shader);
    finalize_g_ssbo();
    update_g_vertices();

    m_shader.bind();
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.count);

    g_shader.bind();
    glBindVertexArray(g_vao);
    glDrawArrays(GL_LINES, 0, g_vertices.count);

    g_shader.unbind();
    glBindVertexArray(0);
    flush();
}

renderer::~renderer()
{
    m_shader.destroy();
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &g_vbo);
    glDeleteVertexArrays(1, &g_vao);
    glDeleteBuffers(1, &g_vbo);
    delete[] textures;
    delete[] local_mats.arr;
    delete[] g_local_mats.arr;
    delete[] vertices.arr;
    delete[] g_vertices.arr;
} 