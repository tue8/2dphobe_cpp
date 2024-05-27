#pragma once

#include "renderer.h"
#include "obj.h"

struct texture_coords
{
	vec2 start;
	vec2 offset;
};

struct text_coord
{
	char c;
	texture_coords t_coords;
};

class text
{
private:
	renderer *m_renderer;
	float font_texture;
public:
	void init(renderer *m_renderer, float font_texture);
	float get_font_texture();
	texture_coords find_charcoords(char c);
};