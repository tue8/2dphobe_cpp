#include "pb_app.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159265

float cam_scroll_offset = 0;


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
	app_name = std::string(name);
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

void pb_app::draw_quad(obj quad)
{
	if (m_renderer.get_geometric_mode())
		g_draw_quad(quad);
	else
		m_draw_quad(quad);
}

void pb_app::m_draw_quad(obj quad)
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

void pb_app::g_draw_quad(obj quad)
{
	float local_mat_index = (float)m_renderer.push_local_mat(quad.get_local_mat());

	m_renderer.g_push_vert({
		{-0.5f, -0.5f,  1.f},
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.g_push_vert({
		{0.5f, -0.5f,  1.f},
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.g_push_vert({
		{0.5f,  0.5f,  1.f},
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.g_push_vert({
		{-0.5f,  0.5f,  1.f},
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.g_push_vert({
		{-0.5f,  0.5f,  1.f},
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.g_push_vert({
		{-0.5f, -0.5f,  1.f},
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.g_push_vert({
		{0.5f, 0.5f,  1.f},
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});

	m_renderer.g_push_vert({
		{0.5f,  -0.5f,  1.f},
		local_mat_index,
		{quad.color.x, quad.color.y, quad.color.z},
		});
}

void pb_app::draw_tri(obj tri)
{
	if (m_renderer.get_geometric_mode())
		g_draw_tri(tri);
	else
		m_draw_tri(tri);
}

void pb_app::m_draw_tri(obj tri)
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

void pb_app::g_draw_tri(obj tri)
{
	float local_mat_index = (float)m_renderer.push_local_mat(tri.get_local_mat());

	m_renderer.g_push_vert({
		{-0.5f,  0.5f,  1.f},
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});

	m_renderer.g_push_vert({
		{0.5f,  0.5f,  1.f},
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});

	m_renderer.g_push_vert({
		{0.5f,  0.5f,  1.f},
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});

	m_renderer.g_push_vert({
		{0.f,  -0.5f,  1.f},
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});

	m_renderer.g_push_vert({
		{0.f,  -0.5f,  1.f},
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});

	m_renderer.g_push_vert({
		{-0.5f,  0.5f,  1.f},
		local_mat_index,
		{tri.color.x, tri.color.y, tri.color.z},
		});
}

void pb_app::draw_circle(obj circle, float angle)
{
	if (m_renderer.get_geometric_mode())
		g_draw_circle(circle, angle);
	else
		m_draw_circle(circle, angle);
}


void pb_app::m_draw_circle(obj circle, float angle)
{
	// nah
}


void pb_app::g_draw_circle(obj circle, float angle)
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

		m_renderer.g_push_vert({
			{curr_x, curr_y,  1.f},
			local_mat_index,
			{circle.color.x, circle.color.y, circle.color.z},
			});

		i += angle;
		count++;
	}

	m_renderer.g_push_vert({
		{(float)sin(angle * PI / 180), (float)cos(angle * PI / 180), 1.f},
		local_mat_index,
		{circle.color.x, circle.color.y, circle.color.z},
		});
}

glm::vec3& pb_app::get_cam_pos()
{
	return m_renderer.get_view_pos();
}

void scroll_offset(GLFWwindow* window, double xoffset, double yoffset)
{
	cam_scroll_offset += yoffset;
}

void pb_app::debug_cam(float cam_speed, float zoom_intensity, float dt)
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
		get_cam_pos() -= diff * dt * cam_speed;
		prev_diff = diff;
		hold = true;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		hold = false;

	m_renderer.set_zoom(1.f + cam_scroll_offset * zoom_intensity);
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
		glfwSetScrollCallback(window, scroll_offset);

		debug_cam(0.5f, 0.05f, delta_time);

		//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		//	__debugbreak();

		process_input();
		update();
		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render();

		if (m_renderer.get_geometric_mode())
			m_renderer.set_geometric_mode(false);
		m_renderer.draw();
		std::stringstream ss;
		ss << app_name << " | FPS: " << fps << " Draw calls: " << m_renderer.draw_call;
		glfwSetWindowTitle(window, ss.str().c_str());

		glfwSwapBuffers(window);
		m_renderer.draw_call = 0;
	}

	end();
	return true;
}
