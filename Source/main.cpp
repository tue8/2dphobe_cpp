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
    unsigned int box_texture;
    std::vector<obj> quads;
    obj circle;
    obj tri;
public:
    using pb_app::pb_app;

    void init() override
    {
        std::string path = utils::get_app_dir();
        path.append("Data\\Textures\\box.png");

        if (!load_texture(box_texture, path.c_str()))
            end();

        circle.size = glm::vec3(quad_size, quad_size, 0.f);
        circle.pos = glm::vec3(50.f, 50.f, 0.f);
        circle.load_texture(box_texture);

        tri.size = glm::vec3(quad_size * 2, quad_size * 2, 0.f);
        tri.pos = glm::vec3(250.f, 50.f, 0.f);
        tri.load_texture(box_texture);

        for (float j = 0; j < win_width; j += quad_size)
        {
            for (float k = 0; k < win_height; k += quad_size)
            {
                obj quad;
                quad.size = glm::vec3(quad_size, quad_size, 0.f);
                quad.pos = glm::vec3(j, k, 0.f);
                quad.load_texture(box_texture);
                quads.push_back(quad);
            }
        }
        
        set_drag_zoom(true);
    }

    void process_input(int* key_input, float dt) override
    {
        float x = 0;
        float y = 0;
        float speed = 100.f * dt;

        if (key_input[GLFW_KEY_W])
            y -= speed;
        if (key_input[GLFW_KEY_A])
            x -= speed;
        if (key_input[GLFW_KEY_S])
            y += speed;
        if (key_input[GLFW_KEY_D])
            x += speed;

        quads.at(0).pos += glm::vec3(x, y, 0.f);
    }

    void render() override
    {
        world& m_world = get_world();
        view_area m_view_area = m_world.get_view_area();
        obj view_area_obj(m_view_area.pos, m_view_area.size, glm::vec3(1.f));

        int obj_count = 0;
        for (obj& quad : quads)
        {
            if (view_area_obj.contains_tile(quad, 2.f))
            {
                draw_quad(quad);
                obj_count++;
            }
        }

        std::stringstream obj_count_str;
        obj_count_str << "Rendering " << obj_count << " out of " << quads.size() << " quads";
        draw_text(obj_count_str.str(), glm::vec3(0.f), glm::vec3(25.f, 25.f, 0.f), 20.f);
        draw_text(obj_count_str.str(), glm::vec3(1.f), glm::vec3(20.f, 20.f, 0.f), 20.f);
    }
};

int main()
{
    app main_app(win_width, win_height, "2dphobe_cpp");
    if (!main_app.run())
        return -1;
    return 0;
}