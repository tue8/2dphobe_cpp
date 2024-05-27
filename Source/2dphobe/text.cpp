#include "text.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define CHAR_COUNT 84
#define CHAR_OFFSET 18.f
#define FONT_TEX_SIZE_X 574.f
#define FONT_TEX_SIZE_Y 55.f

#define COORDS_OFFSET { CHAR_OFFSET / FONT_TEX_SIZE_X, CHAR_OFFSET / FONT_TEX_SIZE_Y }
#define COORDS_START(c_x, c_y) { c_x / FONT_TEX_SIZE_X, c_y / FONT_TEX_SIZE_Y }

text_coord text_coords[CHAR_COUNT];

static void setup_row(int* i, char c, int char_range, float x_offset, float y_offset)
{
	int i_range;

	i_range = *i + char_range;
	for (; *i <= i_range; (*i)++)
	{
		text_coord d;
		d.c = c;
		d.t_coords.start = COORDS_START(x_offset, y_offset);
		d.t_coords.offset = COORDS_OFFSET;
		memcpy(text_coords + (*i), &d, sizeof(text_coord));
		c++;
		x_offset += CHAR_OFFSET;
	}
}

static void setup_char(int* i, char c, float x_offset, float y_offset)
{
	text_coord d;
	d.c = c;
	d.t_coords.start = COORDS_START(x_offset, y_offset);
	d.t_coords.offset = COORDS_OFFSET;
	memcpy(text_coords + (*i), &d, sizeof(text_coord));
	(*i)++;
}

void setup_text_coords()
{
	int row_index = 0;
	setup_row(&row_index, 'a', (int)('z' - 'a'), 0.f, 0.f);
	setup_row(&row_index, 'A', (int)('Z' - 'A'), 0.f, 19.f);
	setup_row(&row_index, '0', (int)('9' - '0'), 0.f, 37.f);

	setup_char(&row_index, ' ', 556.f, 0.f);
	setup_char(&row_index, '.', 182.f, 37.f);
	setup_char(&row_index, ':', 198.f, 37.f);
	setup_char(&row_index, ',', 216.f, 37.f);
	setup_char(&row_index, ';', 234.f, 37.f);
	setup_char(&row_index, '(', 252.f, 37.f);
	setup_char(&row_index, '*', 271.f, 37.f);
	setup_char(&row_index, '!', 288.f, 37.f);
	setup_char(&row_index, '?', 306.f, 37.f);
	setup_char(&row_index, ')', 378.f, 37.f);
	setup_char(&row_index, '#', 396.f, 37.f);
	setup_char(&row_index, '$', 414.f, 37.f);
	setup_char(&row_index, '%', 468.f, 37.f);
	setup_char(&row_index, '&', 504.f, 37.f);
	setup_char(&row_index, '-', 524.f, 37.f);
	setup_char(&row_index, '+', 540.f, 37.f);
	setup_char(&row_index, '@', 556.f, 37.f);
}

void text::init(renderer* m_renderer, float font_texture)
{
	this->m_renderer = m_renderer;
	this->font_texture = font_texture;
	setup_text_coords();
}

float text::get_font_texture()
{
	return font_texture;
}

texture_coords text::find_charcoords(char c)
{
	for (int i = 0; i < CHAR_COUNT; i++)
		if (text_coords[i].c == c)
			return text_coords[i].t_coords;
	return {};
}