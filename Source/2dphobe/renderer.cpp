#include "renderer.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

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

void renderer::init()
{
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_textures);

    init_vao();
    init_ssbo();
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

void renderer::finalize_ssbo(const std::vector<glm::mat4> &mats) const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    0,
                    mats.size() * sizeof(glm::mat4),
                    mats.data());
}

void renderer::finalize_textures(const std::vector<unsigned int>& textures) const
{
    for (int i = 0; i < textures.size(); i++)
        glBindTextureUnit(i, textures.at(i));
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

void renderer::draw_vertices(std::vector<vertex> &vertices) const
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

int renderer::get_max_textures() const
{
    return max_textures;
}

renderer::~renderer()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}