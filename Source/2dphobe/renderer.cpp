#include "renderer.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


#define QUAD_VERT 6
#define MAX_QUAD 1000

void renderer::init_vao()
{
    //vertex vertices_i[] = {
    //    /* ----- NDC ---------- TEX COORDS -- */
    //    {{-0.5f, -0.5f,  1.f},     {0.0f, 0.0f}},
    //    {{0.5f, -0.5f,  1.f},      {1.0f, 0.0f}},
    //                                          
    //    {{0.5f,  0.5f,  1.f},      {1.0f, 1.0f}},
    //    {{-0.5f,  0.5f,  1.f},     {0.0f, 1.0f}},
    //                                          
    //    {{-0.5f,  0.5f,  1.f},     {0.0f, 1.0f}},
    //    {{-0.5f, -0.5f,  1.f},     {0.0f, 0.0f}},
    //                                          
    //    {{0.5f,  0.5f,  1.f},      {1.0f, 1.0f}},
    //    {{0.5f, -0.5f,  1.f},      {1.0f, 0.0f}},
    //};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

    vert_count = 0;
    texture_count = 0;
    local_mats_count = 0;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_textures);

    default_shader.init(max_textures);

    vertices = new vertex[QUAD_VERT * MAX_QUAD];
    textures = new unsigned int[max_textures];
    local_mats = new glm::mat4[MAX_QUAD];

    init_vao();
    init_ssbo();

    cam_view_pos = glm::vec3(50.f, 50.f, 0.f);
}

void renderer::push_vert(const vertex &vert)
{
    vertices[vert_count++] = vert;
}

int renderer::push_local_mat(const glm::mat4 local_mat)
{
    if (local_mats_count + 1 >= MAX_QUAD)
        draw();
    local_mats[local_mats_count] = local_mat;
    return local_mats_count++;
}

glm::vec3 &renderer::get_view_pos()
{
    return cam_view_pos;
}

void renderer::flush()
{
    memset(vertices, 0, vert_count * sizeof(vertex));
    memset(textures, 0, texture_count * sizeof(unsigned int));
    memset(local_mats, 0, local_mats_count * sizeof(glm::mat4));

    vert_count = 0;
    texture_count = 0;
    local_mats_count = 0;
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

    default_shader.bind();
    int location = glGetUniformLocation(default_shader.get_id(),
                                        "texs");

    glUniform1iv(location, max_textures, samplers);
    default_shader.unbind();
    delete[] samplers;
}


void renderer::finalize_mvp()
{
    default_shader.bind();
    glm::mat4 view = glm::mat4(1.f),
              proj = glm::mat4(1.f);

    view = glm::translate(view, cam_view_pos);
    proj = glm::ortho(0.f, (float)width, (float)height, 0.f, -1.f, 1.f);

    default_shader.set_mat4("view", view);
    default_shader.set_mat4("proj", proj);
    default_shader.unbind();
}

void renderer::finalize_ssbo() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    0,
                    local_mats_count * sizeof(glm::mat4),
                    local_mats);
}

void renderer::finalize_textures() const
{
    for (int i = 0; i < texture_count; i++)
        glBindTextureUnit(i, textures[i]);
}

void renderer::update_vertices() const
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    vert_count * sizeof(vertex),
                    vertices);
    glBindVertexArray(0);
}

void renderer::draw()
{
    finalize_mvp();
    finalize_samplers();
    finalize_ssbo();
    finalize_textures();
    update_vertices();

    default_shader.bind();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vert_count);

    flush();
}

renderer::~renderer()
{
    default_shader.destroy();
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    delete[] textures;
    delete[] local_mats;
    delete[] vertices;
} 