#pragma once

struct texture_coords
{
	float start_x;
	float start_y;
	float offset_x;
	float offset_y;
};

struct font_coord
{
	char c;
	texture_coords t_coords;
};

void setup_font_coords();
texture_coords find_charcoords(char c);