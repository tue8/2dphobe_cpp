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

void renderer::g_init_vao()
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
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        sizeof(glm::mat4) * MAX_QUAD,
        NULL,
        GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void renderer::init(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;

    geometric = false;

    vertices.count = 0;
    texture_count = 0;
    local_mats.count = 0;

    draw_call = 0;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_textures);

    m_shader.init(max_textures);
    g_shader.init(0);

    vertices.arr = new vertex[QUAD_VERT * MAX_QUAD];
    g_vertices.arr = new g_vertex[QUAD_VERT * MAX_QUAD];
    textures = new unsigned int[max_textures];
    local_mats.arr = new glm::mat4[MAX_QUAD];

    init_vao();
    g_init_vao();
    init_ssbo();

    cam_view_pos = glm::vec3(50.f, 50.f, 0.f);
}

void renderer::set_geometric_mode_nocheck(bool g)
{
    geometric = g;
}

void renderer::set_geometric_mode(bool g)
{
    if (geometric != g)
        draw();
    geometric = g;
}

bool renderer::get_geometric_mode()
{
    return geometric;
}

/*
* We don't need to handle vertices exceed limit
* since push_local_mat() already did that.
*/
void renderer::push_vert(const vertex &vert)
{
    vertices.arr[vertices.count++] = vert;
}

void renderer::g_push_vert(const g_vertex &vert)
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

glm::vec3 &renderer::get_view_pos()
{
    return cam_view_pos;
}

void renderer::flush()
{
    memset(vertices.arr, 0, vertices.count * sizeof(vertex));
    memset(textures, 0, texture_count * sizeof(unsigned int));
    vertices.count = 0;
    texture_count = 0;

    memset(g_vertices.arr, 0, g_vertices.count * sizeof(g_vertex));
    g_vertices.count = 0;
    memset(local_mats.arr, 0, local_mats.count * sizeof(glm::mat4));

    local_mats.count = 0;
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
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    0,
                    local_mats.count * sizeof(glm::mat4),
                    local_mats.arr);
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

void renderer::g_update_vertices() const
{
    glBindVertexArray(g_vao);

    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
        0,
        g_vertices.count * sizeof(g_vertex),
        g_vertices.arr);
    glBindVertexArray(0);
}

void renderer::m_draw()
{
    draw_call++;
    finalize_mvp(m_shader);
    finalize_samplers();
    finalize_ssbo();
    finalize_textures();
    update_vertices();
    m_shader.bind();
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.count);
    flush();
}

void renderer::g_draw()
{
    draw_call++;
    finalize_mvp(g_shader);
    finalize_ssbo();
    g_update_vertices();
    g_shader.bind();
    glBindVertexArray(g_vao);
    glDrawArrays(GL_LINES, 0, g_vertices.count);
    flush();
}

void renderer::draw()
{
    if (!geometric && vertices.count > 0)
        m_draw();
    else if (geometric && g_vertices.count > 0)
        g_draw();
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
    delete[] vertices.arr;
    delete[] g_vertices.arr;
} 