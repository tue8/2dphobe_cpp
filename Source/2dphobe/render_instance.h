#pragma once
#include "renderer.h"

struct resolution
{
	unsigned int width;
	unsigned int height;
};

class render_instance
{
private:
	renderer* renderer_ptr;
	std::vector<vertex> vertices;
	std::vector<unsigned int> textures;
	shader our_shader;
	std::vector<glm::mat4> mats;
public:
	void init(renderer* renderer_ptr, render_type r_type);
	~render_instance();
	void push_vertex(const vertex& vert);
	int push_mat(const glm::mat4& mat);
	float get_texture_index(const float texture_id);
	void draw();
	void set_vp(const glm::mat4& v, const glm::mat4& p);
	
private:
	virtual void finalize_cam() = 0;
	void flush();
};