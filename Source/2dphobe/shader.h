#pragma once

#include <glm/glm.hpp>

enum shader_type
{
	DEFAULT_SHADER,
	SCREEN_SHADER,
};

class shader
{
private:
	unsigned int id;
	int link() const;
	int create(unsigned int shader, const char **src) const;
public:
	int init(enum shader_type type, int max_textures);
	unsigned int get_id() const;
	void bind();
	void unbind();
	void destroy();
	void set_mat4(const char *name, const glm::mat4 mat) const;
	void set_vec3(const char *name, const glm::vec3 v) const;
	void set_vec3(const char *name, float v1, float v2, float v3) const;
	void set_int(const char *name, int i) const;
	void set_float(const char *name, float f) const;
	void set_4f(const char *name, float f1, float f2, float f3, float f4) const;
};

