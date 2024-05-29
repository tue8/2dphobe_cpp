#pragma once

#include <glm/glm.hpp>

class shader
{
private:
	unsigned int id;
	int link() const;
	int create(unsigned int shader, const char **src) const;
	bool init_shader(const char* vert_csrc, const char* frag_csrc, int max_textures);
public:
	bool init_world_shader(int max_textures);
	bool init_screen_shader(int max_textures);
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

