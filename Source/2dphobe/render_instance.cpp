#include "render_instance.h"

void render_instance::init(renderer* renderer_ptr, render_type r_type)
{
    this->renderer_ptr = renderer_ptr;
	our_shader.init(r_type, renderer_ptr->get_max_textures());
}
    
/*
* We don't need to handle vertices exceed limit
* since push_vertex() already did that.
*/
void render_instance::push_vertex(const vertex& vert)
{
	vertices.push_back(vert);
}

int render_instance::push_mat(const glm::mat4& mat)
{
	if (mats.size() + 1 >= max_quad)
		draw();
	mats.push_back(mat);
	return mats.size() - 1;
}

float render_instance::get_texture_index(const float texture_id)
{
    float tex_index;
    bool tex_find = false;

    for (int i = 0; i < textures.size(); i++)
    {
        if (textures.at(i) == texture_id)
        {
            tex_index = (float)i;
            tex_find = true;
            break;
        }
    }

    if (!tex_find)
    {
        if (textures.size() > renderer_ptr->get_max_textures() - 1)
            draw();
        textures.push_back((unsigned int)texture_id);
        tex_index = (float)textures.size() - 1;
    }

    return tex_index;
}

void render_instance::flush()
{
    if (!vertices.empty()) vertices.clear();
    if (!mats.empty()) mats.clear();
    if (!textures.empty()) textures.clear();
}

void render_instance::draw()
{
    if (vertices.size() <= 0)
        return;
    finalize_cam();
    renderer_ptr->finalize_samplers(our_shader);
    renderer_ptr->finalize_ssbo(mats);
    renderer_ptr->finalize_textures(textures);
    renderer_ptr->update_vertices(vertices);
    our_shader.bind();
    renderer_ptr->draw_vertices(vertices);
    flush();
}

void render_instance::set_vp(const glm::mat4& v, const glm::mat4& p)
{
    our_shader.bind();
    our_shader.set_mat4("view", v);
    our_shader.set_mat4("proj", p);
    our_shader.unbind();
}

render_instance::~render_instance()
{
    our_shader.destroy();
}