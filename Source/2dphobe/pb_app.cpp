#include "pb_app.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include "utils.h"

#define PI 3.14159265

float cam_scroll_offset = 0;
int keys[1024];

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}


pb_app::pb_app(unsigned int width,
			   unsigned int height,
               const char *name) : width(width), height(height),
								   delta_time(0.f), fps(0),
								   end_signal(false),
								   cursor_x(0), cursor_y(0),
								   app_name(std::string(name)), b_drag_zoom(false)
{
	created = true;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_true);
#endif
    window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
		created = false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
		created = false;
    }

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetKeyCallback(window, key_callback);

	unsigned int font_id = 0;
	if (!load_texture(font_id, utils::get_app_dir().append("Data\\Fonts\\font.png").c_str()))
		created = false;
	text_handler.init(&m_renderer, (float)font_id);
}

void pb_app::end()
{
	cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
	end_signal = true;
}

bool pb_app::load_texture(unsigned int &texture_id, const char *texture_dir)
{
	int width, height, color_channel;
	unsigned char *data;
	GLenum format;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load(texture_dir, &width, &height, &color_channel, 0);
	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
		return false;
	}

	switch (color_channel)
	{
	case 1: format = GL_RED;    break;
	case 3: format = GL_RGB;    break;
	case 4: format = GL_RGBA;   break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return true;
}

void scroll_offset(GLFWwindow* window, double xoffset, double yoffset)
{
	cam_scroll_offset += yoffset;
}

renderer& pb_app::get_renderer()
{
	return m_renderer;
}

void pb_app::set_drag_zoom(bool dz)
{
	b_drag_zoom = dz;
}

float pb_app::get_fps()
{
	return fps;
}

void pb_app::drag_zoom(float zoom_intensity)
{
	static bool hold = false;
	static glm::vec3 origin;
	float zoom_value = 1.f + cam_scroll_offset * zoom_intensity;
	m_renderer.set_zoom(zoom_value);

	if (!hold)
	{
		origin = glm::vec3(cursor_x, cursor_y, 0.f);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		hold = true;
		glm::vec3 cursor_pos(cursor_x, cursor_y, 0.f);
		glm::vec3 diff = cursor_pos - origin;
		m_renderer.get_view_pos() += diff / zoom_value;
		origin = glm::vec3(cursor_x, cursor_y, 0.f);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		hold = false;
}

bool pb_app::run()
{
	float last_frame = 0.f;
	float last_sec_frame = (float)glfwGetTime();
	int frame = 0;

	if (!created)
	{
		std::cout << "Couldn't run app since it was not"
					 "successfully created." << std::endl;
		return false;
	}

	m_renderer.init(width, height);
	init();

	while (!glfwWindowShouldClose(window) && !end_signal)
	{
		float curr_frame = (float)glfwGetTime();
		frame++;

		delta_time = curr_frame - last_frame;
		last_frame = curr_frame;

		if (curr_frame - last_sec_frame >= 1.f)
		{
			fps = double(frame) / (curr_frame - last_sec_frame);
			frame = 0;
			last_sec_frame = curr_frame;
		}

		update(delta_time);

		glfwPollEvents();
		glfwGetCursorPos(window, &cursor_x, &cursor_y);
		glfwSetScrollCallback(window, scroll_offset);
		process_input(keys, delta_time);

		if (b_drag_zoom)
			drag_zoom(0.25f);

		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render();

		m_renderer.draw();
		std::stringstream ss;
		ss << app_name << " | FPS: " << fps;
		glfwSetWindowTitle(window, ss.str().c_str());

		glfwSwapBuffers(window);
	}

	end();
	return true;
}


/*
* draw functions
*/

void pb_app::draw_quad(obj& quad)
{
	float local_mat_index = (float)m_renderer.push_local_mat(quad.get_local_mat());
	float texture_index = m_renderer.get_texture_index((float)quad.get_texture_id());

	m_renderer.push_vert({
		{-0.5f, -0.5f,  1.f},
		{0.0f, 0.0f},
		texture_index,
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.push_vert({
		{0.5f, -0.5f,  1.f},
		{1.0f, 0.0f},
		texture_index,
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.push_vert({
		{0.5f,  0.5f,  1.f},
		{1.0f, 1.0f},
		texture_index,
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.push_vert({
		{0.5f,  0.5f,  1.f},
		{1.0f, 1.0f},
		texture_index,
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.push_vert({
		{-0.5f,  0.5f,  1.f},
		{0.0f, 1.0f},
		texture_index,
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.push_vert({
		{-0.5f, -0.5f,  1.f},
		{0.0f, 0.0f},
		texture_index,
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});
}

void pb_app::draw_tri(obj &tri)
{
	float local_mat_index = (float)m_renderer.push_local_mat(tri.get_local_mat());
	float texture_index = m_renderer.get_texture_index((float)tri.get_texture_id());

	m_renderer.push_vert({
		{-0.5f,  0.5f,  1.f},
		{0.f, 0.f},
		texture_index,
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});


	m_renderer.push_vert({
		{0.5f,  0.5f,  1.f},
		{1.f, 0.f},
		texture_index,
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});

	m_renderer.push_vert({
		{0.f,  -0.5f,  1.f},
		{0.5f, 1.f},
		texture_index,
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});
}

void pb_app::draw_circle(obj &circle, float angle)
{
	float local_mat_index = (float)m_renderer.push_local_mat(circle.get_local_mat());
	float last_x = -1.f;
	float last_y = -1.f;

	int i = 0;
	int count = 1;
	float curr_x = (float)sin(angle * PI / 180);
	float curr_y = (float)cos(angle * PI / 180);

	while (i <= 360)
	{
		if (count % 2 == 0)
		{
			curr_x = (float)sin(i * PI / 180);
			curr_y = (float)cos(i * PI / 180);
		}

		m_renderer.push_vert({
			{curr_x, curr_y,  1.f},
			{0.f, 0.f},
			0,
			local_mat_index,
			{circle.color.x, circle.color.y, circle.color.z},
			});

		if (count % 2 == 0)
		{
			m_renderer.push_vert({
				{0.f, 0.f,  1.f},
				{0.f, 0.f},
				0,
				local_mat_index,
				{circle.color.x, circle.color.y, circle.color.z},
				});
		}

		i += angle;
		count++;
	}

	m_renderer.push_vert({
		{(float)sin(angle * PI / 180), (float)cos(angle * PI / 180), 1.f},
		{0.f, 0.f},
		0,
		local_mat_index,
		{circle.color.x, circle.color.y, circle.color.z},
		});

	m_renderer.push_vert({
		{0.f, 0.f,  1.f},
		{0.f, 0.f},
		0,
		local_mat_index,
		{circle.color.x, circle.color.y, circle.color.z},
		});
}

void pb_app::draw_char(obj& char_obj, texture_coords tc)
{
	float local_mat_index = (float)m_renderer.push_screen_mat(char_obj.get_local_mat());
	float texture_index = m_renderer.get_screen_texture_index(char_obj.get_texture_id());

	vec2 start = tc.start;
	vec2 offset = tc.offset;

	m_renderer.push_screen_vert({
		{-0.5f, -0.5f,  0.f},
		{start.x , start.y},
		texture_index,
		local_mat_index,
		{char_obj.color.x, char_obj.color.y, char_obj.color.z},
		});

	m_renderer.push_screen_vert({
		{0.5f, -0.5f,  0.f},
		{start.x + offset.x, start.y},
		texture_index,
		local_mat_index,
		{char_obj.color.x, char_obj.color.y, char_obj.color.z},
		});

	m_renderer.push_screen_vert({
		{0.5f,  0.5f,  0.f},
		{start.x + offset.x, start.y + offset.y},
		texture_index,
		local_mat_index,
		{char_obj.color.x, char_obj.color.y, char_obj.color.z},
		});

	m_renderer.push_screen_vert({
		{0.5f,  0.5f,  0.f},
		{start.x + offset.x, start.y + offset.y},
		texture_index,
		local_mat_index,
		{char_obj.color.x, char_obj.color.y, char_obj.color.z},
		});

	m_renderer.push_screen_vert({
		{-0.5f,  0.5f,  0.f},
		{start.x, start.y + offset.y},
		texture_index,
		local_mat_index,
		{char_obj.color.x, char_obj.color.y, char_obj.color.z},
		});

	m_renderer.push_screen_vert({
		{-0.5f, -0.5f,  0.f},
		{start.x, start.y},
		texture_index,
		local_mat_index,
		{char_obj.color.x, char_obj.color.y, char_obj.color.z},
		});
}

void pb_app::draw_text(std::string msg, glm::vec3 color, glm::vec3 pos, float scale)
{
	for (size_t i = 0; i < msg.length(); ++i)
	{
		obj char_obj(glm::vec3(pos.x + scale * i, pos.y, 0.f), glm::vec3(scale), color);
		char_obj.load_texture((unsigned int)text_handler.get_font_texture());
		draw_char(char_obj, text_handler.find_charcoords(msg.at(i)));
	}
}