#include "pb_app.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <sstream>

#define PI 3.14159265

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

pb_app::pb_app(unsigned int width,
			   unsigned int height,
               const char *name) : width(width), height(height),
								   delta_time(0.f), fps(0),
								   end_signal(false),
								   cursor_x(0), cursor_y(0)
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

	stbi_set_flip_vertically_on_load(true);
	created = true;

	glfwSwapInterval(1);
}

void pb_app::end()
{
	cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
	end_signal = true;
}

bool pb_app::load_texture(unsigned int &texture_id, const char *texture_dir) const
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

void pb_app::renderer_set_geometric_mode(bool g)
{
	m_renderer.set_geometric_mode(g);
}

bool pb_app::renderer_get_geometric_mode()
{
	return m_renderer.get_geometric_mode();
}

void pb_app::draw_quad(obj m_quad)
{
	if (m_renderer.get_geometric_mode())
		g_draw_quad(m_quad);
	else
		m_draw_quad(m_quad);
}

void pb_app::m_draw_quad(obj m_quad)
{
	float local_mat_index = (float)m_renderer.push_local_mat(m_quad.get_local_mat());
	float texture_index = m_renderer.get_texture_index((float)m_quad.get_texture_id());

	m_renderer.push_vert({
		{-0.5f, -0.5f,  1.f},
		{0.0f, 0.0f},
		texture_index,
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.push_vert({
		{0.5f, -0.5f,  1.f},
		{1.0f, 0.0f},
		texture_index,
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.push_vert({
		{0.5f,  0.5f,  1.f},
		{1.0f, 1.0f},
		texture_index,
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.push_vert({
		{0.5f,  0.5f,  1.f},
		{1.0f, 1.0f},
		texture_index,
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.push_vert({
		{-0.5f,  0.5f,  1.f},
		{0.0f, 1.0f},
		texture_index,
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.push_vert({
		{-0.5f, -0.5f,  1.f},
		{0.0f, 0.0f},
		texture_index,
		local_mat_index,
		{1.f, 1.f, 1.f},
		});
}

void pb_app::g_draw_circle(obj g_cirlce, unsigned int angle)
{
	float local_mat_index = (float)m_renderer.push_local_mat(g_cirlce.get_local_mat());
	float last_x = -1.f;
	float last_y = -1.f;

	int i = 0;
	int count = 1;
	float curr_x = sin(angle * PI / 180);
	float curr_y = cos(angle * PI / 180);

	while (i <= 360)
	{
		if (count % 2 == 0)
		{
			curr_x = sin(i * PI / 180);
			curr_y = cos(i * PI / 180);
		}

		m_renderer.g_push_vert({
			{curr_x, curr_y,  1.f},
			local_mat_index,
			{1.f, 1.f, 1.f},
		});

		i += angle;
		count++;
	}

	m_renderer.g_push_vert({
		{(float)sin(angle * PI / 180), (float)cos(angle * PI / 180), 1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
	});
}

void pb_app::g_draw_quad(obj g_quad)
{
	float local_mat_index = (float)m_renderer.push_local_mat(g_quad.get_local_mat());

	m_renderer.g_push_vert({
		{-0.5f, -0.5f,  1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.g_push_vert({
		{0.5f, -0.5f,  1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.g_push_vert({
		{0.5f,  0.5f,  1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.g_push_vert({
		{-0.5f,  0.5f,  1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.g_push_vert({
		{-0.5f,  0.5f,  1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.g_push_vert({
		{-0.5f, -0.5f,  1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.g_push_vert({
		{0.5f, 0.5f,  1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
		});

	m_renderer.g_push_vert({
		{0.5f,  -0.5f,  1.f},
		local_mat_index,
		{1.f, 1.f, 1.f},
		});
}

void pb_app::debug_cam(float cam_speed, float dt)
{
	static bool hold = false;
	static glm::vec3 origin;
	static glm::vec3 prev_diff = glm::vec3(0.f);

	if (!hold)
		origin = glm::vec3(cursor_x, cursor_y, 0.f);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glm::vec3 cursor_pos(cursor_x, cursor_y, 0.f);
		glm::vec3 diff = origin - cursor_pos;
		if (prev_diff == diff)
			origin = glm::vec3(cursor_x, cursor_y, 0.f);
		m_renderer.get_view_pos() -= diff * dt * cam_speed;
		prev_diff = diff;
		hold = true;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		hold = false;
	}
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

		glfwPollEvents();
		glfwGetCursorPos(window, &cursor_x, &cursor_y);

		debug_cam(5.f, delta_time);

		process_input();
		update();
		glClearColor(0.f, 0.f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render();

		if (m_renderer.get_geometric_mode())
			m_renderer.set_geometric_mode(false);
		m_renderer.draw();
		std::stringstream ss;
		ss << "FPS: " << fps << " Draw calls: " << m_renderer.draw_call;
		glfwSetWindowTitle(window, ss.str().c_str());
		glfwSwapBuffers(window);
		m_renderer.draw_call = 0;
	}

	end();
	return true;
}
