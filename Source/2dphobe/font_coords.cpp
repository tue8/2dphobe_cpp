#include "font_coords.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

constexpr int char_count = 84;
constexpr float char_offset = 18.f;
constexpr float texture_size_x = 574.f;
constexpr float texture_size_y = 55.f;

constexpr void fill_font_coord(font_coord &d, char c, float x_offset, float y_offset)
{
	d.c = c;
	d.t_coords.start_x = x_offset / texture_size_x;
	d.t_coords.start_y = y_offset / texture_size_y;
	d.t_coords.offset_x = char_offset / texture_size_x;
	d.t_coords.offset_y = char_offset / texture_size_y;
}

font_coord font_coords[char_count];

static void setup_row(int* i, char c, int char_range, float x_offset, float y_offset)
{
	int i_range;

	i_range = *i + char_range;
	for (; *i <= i_range; (*i)++)
	{
		font_coord d;
		fill_font_coord(d, c, x_offset, y_offset);
		memcpy(font_coords + (*i), &d, sizeof(font_coord));
		c++;
		x_offset += char_offset;
	}
}

static void setup_char(int* i, char c, float x_offset, float y_offset)
{
	font_coord d;
	fill_font_coord(d, c, x_offset, y_offset);
	memcpy(font_coords + (*i), &d, sizeof(font_coord));
	(*i)++;
}

void setup_font_coords()
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

texture_coords find_charcoords(char c)
{
	for (int i = 0; i < char_count; i++)
		if (font_coords[i].c == c)
			return font_coords[i].t_coords;
	return {};
}