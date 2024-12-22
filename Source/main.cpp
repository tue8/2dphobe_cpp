/*
* ==============================================
* tue (@tue8),
* April, 2024
* 2dphobe engine remade in C++
* ==============================================
*/

#include <iostream>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <sstream>
#include "2dphobe/pb_app.h"
#include "2dphobe/utils.h"

constexpr unsigned int win_width = 800;
constexpr unsigned int win_height = 800;
constexpr unsigned int quad_size = 10;
constexpr unsigned int quad_count = (win_width * win_height) / (quad_size * quad_size);

class app : public pb_app
{
private:
    unsigned int default_texture;
    std::vector<obj> quads;
    float prerender = 2.f;
    std::vector<glm::vec3> marked_pos;
public:
    using pb_app::pb_app;

    void init() override
    {
        if (!load_texture(default_texture, utils::get_app_dir().append("Data\\Textures\\default.png").c_str()))
            end();

        for (float j = 0; j < win_width; j += quad_size)
        {
            for (float k = 0; k < win_height; k += quad_size)
            {
                obj quad;
                quad.size = glm::vec3(quad_size, quad_size, 0.f);
                quad.pos = glm::vec3(j, k, 0.f);
                quad.color = glm::vec3(0.5f, 0.f, 0.5f);
                quad.load_texture(default_texture);
                quads.push_back(quad);
            }
        }

        set_drag_zoom(true);
    }

    void update(float dt) override
    {
        //quads.clear();

        //world& m_world = get_world();
        //view_area m_view_area = m_world.get_view_area();

        //for (float j = test_size(m_view_area.pos.x - quad_size * prerender, quad_size);
        //    j < test_size(m_view_area.pos.x + m_view_area.size.x + quad_size * prerender, quad_size);
        //    j += quad_size)
        //{
        //    for (float k = test_size(m_view_area.pos.y - quad_size * prerender, quad_size);
        //        k < test_size(m_view_area.pos.y + m_view_area.size.y + quad_size * prerender, quad_size);
        //        k += quad_size)
        //    {
        //        obj quad;
        //        quad.size = glm::vec3(quad_size, quad_size, 0.f);
        //        quad.pos = glm::vec3(j, k, 0.f);
        //        quad.color = glm::vec3(0.5f, 0.f, 0.5f);
        //        //quad.load_texture(grid_texture);
        //        quads.push_back(quad);
        //    }
        //}
    }

    void process_input(int* key_input, GLFWwindow *window, float dt) override
    {
        //bool mouse_held = false;
        //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        //    mouse_held = true;
        //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        //    mouse_held = false;

        //glm::vec3 mouse_pos = get_mouse_pos();
        //mouse_pos.x = mouse_pos.x / get_zoom(0.25f) + get_world().get_view_area().pos.x;
        //mouse_pos.y = mouse_pos.y / get_zoom(0.25f) + get_world().get_view_area().pos.y;
        //mouse_pos.x = test_size(mouse_pos.x, quad_size);
        //mouse_pos.y = test_size(mouse_pos.y, quad_size);

        //if (mouse_held && std::count(marked_pos.begin(), marked_pos.end(), mouse_pos) <= 0)
        //{
        //    marked_pos.push_back(mouse_pos);
        //    obj quad;
        //    quad.size = glm::vec3(quad_size, quad_size, 0.f);
        //    quad.pos = mouse_pos;
        //    quads.push_back(quad);
        //}
    }

    float test_size(float size, float quad_size)
    {
        return std::round(size / quad_size) * quad_size;
    }

    void draw_text_shadow(std::string text, glm::vec3 pos)
    {
        draw_text(text, glm::vec3(0.f), glm::vec3(pos), 20.f);
        draw_text(text, glm::vec3(1.f), glm::vec3(pos - 5.f), 20.f);
    }

    void render() override
    {
        world& m_world = get_world();
        view_area m_view_area = m_world.get_view_area();
        obj view_area_obj(m_view_area.pos, m_view_area.size, glm::vec3(0));
        unsigned int quad_drawn_count = 0;

        for (obj& quad : quads)
        {
            if (view_area_obj.contains_tile(quad, prerender))
            {
                draw_quad(quad);
                quad_drawn_count++;
            }
        }
                
        std::stringstream stats;
        stats << "Quad count : " << quad_drawn_count << "\n"
              << "View x: " << get_world().get_view_area().pos.x
              <<     " y: " << get_world().get_view_area().pos.y << "\n"
              << "Zoom: " << get_zoom(0.25f);
        draw_text_shadow(stats.str(), glm::vec3(25.f, 25.f, 0.f));
    }
};

int main()
{
    app main_app(win_width, win_height, "2dphobe_cpp");
    if (!main_app.run())
        return -1;
    return 0;
}