#include "shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

const char *world_vert_src =
"#version 430 core\n"
"layout (location = 0) in vec3 vpos;\n"
"layout (location = 1) in vec2 vtexcoord;\n"
"layout (location = 2) in float vtexid;\n"
"layout (location = 3) in float vlocal_mat_index;\n"
"layout (location = 4) in vec3 vcolor;\n"

"layout (std430, binding = 0) buffer vlocal_mats\n"
"{\n"
"    mat4 models[];\n"
"};\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"

"out vec2 ftexcoord;\n"
"out float ftexid;\n"
"out vec3 fcolor;\n"

"void main()\n"
"{\n"
"   int index = int(vlocal_mat_index);\n"
"   gl_Position = proj * view * models[index] * vec4(vpos, 1.0);\n"
"   ftexcoord = vtexcoord;\n"
"   ftexid = vtexid;\n"
"   fcolor = vcolor;\n"
"}\0";

const char * world_frag_src =
"#version 430 core\n"
"in vec2 ftexcoord;\n"
"in float ftexid;"
"in vec3 fcolor;"

"uniform sampler2D texs[%d];\n"

"out vec4 fo;\n"

"void main()\n"
"{\n"
"   int index = int(ftexid);\n"
"   fo = vec4(fcolor, 1.f) * texture(texs[index], ftexcoord);\n"
"}\n\0";

const char* screen_vert_src =
"#version 430 core\n"
"layout (location = 0) in vec3 vpos;\n"
"layout (location = 1) in vec2 vtexcoord;\n"
"layout (location = 2) in float vtexid;\n"
"layout (location = 3) in float vlocal_mat_index;\n"
"layout (location = 4) in vec3 vcolor;\n"

"layout (std430, binding = 0) buffer vlocal_mats\n"
"{\n"
"    mat4 models[];\n"
"};\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"

"out vec2 ftexcoord;\n"
"out float ftexid;\n"
"out vec3 fcolor;\n"

"void main()\n"
"{\n"
"   int index = int(vlocal_mat_index);\n"
"   gl_Position = proj * view * models[index] * vec4(vpos, 1.0);\n"
"   ftexcoord = vtexcoord;\n"
"   ftexid = vtexid;\n"
"   fcolor = vcolor;\n"
"}\0";

const char* screen_frag_src =
"#version 430 core\n"
"in vec2 ftexcoord;\n"
"in float ftexid;"
"in vec3 fcolor;"

"uniform sampler2D texs[%d];\n"

"out vec4 fo;\n"

"void main()\n"
"{\n"
"   int index = int(ftexid);\n"
"   fo = vec4(fcolor, 1.f) * texture(texs[index], ftexcoord);\n"
"}\n\0";

int shader::create(unsigned int shader, const char **src) const
{
    int success;
    glShaderSource(shader, 1, src, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    return success;
}

int shader::link() const
{
    int success;
    char info_log[512];
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, sizeof(info_log), nullptr, info_log);
        std::cout << "shader link error: " << info_log << std::endl;
    }
    return success;
}

bool shader::init(render_type type, int max_textures)
{
    switch (type)
    {
    case WORLD: 
        init_shader(world_vert_src,
                    world_frag_src,
                    max_textures); 
        break;
    case SCREEN:
        init_shader(screen_vert_src,
                    screen_frag_src,
                    max_textures);
        break;
    }

    return false;
}

bool shader::init_shader(const char* vert_csrc, const char* frag_csrc, int max_textures)
{
    unsigned int vertshader, fragshader;
    char* new_frag_src = (char*)malloc(strlen(frag_csrc) + 1);
    sprintf(new_frag_src, frag_csrc, max_textures);

    vertshader = glCreateShader(GL_VERTEX_SHADER);
    create(vertshader, (const char**)&vert_csrc);

    fragshader = glCreateShader(GL_FRAGMENT_SHADER);
    create(fragshader, (const char**)&new_frag_src);
    free(new_frag_src);

    id = glCreateProgram();
    glAttachShader(id, vertshader);
    glAttachShader(id, fragshader);
    if (!link())
        return false;
    glDeleteShader(vertshader);
    glDeleteShader(fragshader);
    return true;
}

void shader::bind()
{
    glUseProgram(id);
}

void shader::unbind()
{
    glUseProgram(0);
}

void shader::destroy()
{
    glDeleteProgram(id);
}

void shader::set_mat4(const char *name, const glm::mat4 mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void shader::set_vec3(const char *name, const glm::vec3 v) const
{
    glUniform3f(glGetUniformLocation(id, name), v[0], v[1], v[2]);
}

void shader::set_vec3(const char *name, float v1, float v2, float v3) const
{
    glUniform3f(glGetUniformLocation(id, name), v1, v2, v3);
}

void shader::set_int(const char *name, int i) const
{
    glUniform1i(glGetUniformLocation(id, name), i);
}

void shader::set_float(const char *name, float f) const
{
    glUniform1f(glGetUniformLocation(id, name), f);
}

void shader::set_4f(const char *name, float f1, float f2, float f3, float f4) const
{
    glUniform4f(glGetUniformLocation(id, name), f1, f2, f3, f4);
}

unsigned int shader::get_id() const
{
    return id;
}