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
#include "2dphobe/text.h"
#include "2dphobe/utils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 800
#define QUAD_SIZE 10
#define QUAD_COUNT (WIN_WIDTH * WIN_HEIGHT) / (QUAD_SIZE * QUAD_SIZE)

class app : public pb_app
{
private:
    unsigned int box_texture;
    std::vector<obj> quads;
    obj circle;
    obj tri;
public:
    init_pb_app(app);

    void init()
    {
        std::string path = utils::get_app_dir();
        path.append("Data\\Textures\\box.png");

        if (!load_texture(box_texture, path.c_str()))
            end();

        circle.size = glm::vec3(QUAD_SIZE, QUAD_SIZE, 0.f);
        circle.pos = glm::vec3(50.f, 50.f, 0.f);
        circle.load_texture(box_texture);

        tri.size = glm::vec3(QUAD_SIZE * 2, QUAD_SIZE * 2, 0.f);
        tri.pos = glm::vec3(250.f, 50.f, 0.f);
        tri.load_texture(box_texture);

        for (float j = 0; j < WIN_WIDTH; j += QUAD_SIZE)
        {
            for (float k = 0; k < WIN_HEIGHT; k += QUAD_SIZE)
            {
                obj quad;
                quad.size = glm::vec3(QUAD_SIZE, QUAD_SIZE, 0.f);
                quad.pos = glm::vec3(j, k, 0.f);
                quad.load_texture(box_texture);
                quads.push_back(quad);
            }
        }
        
        std::cout << "QUAD COUNT: " << quads.size() << std::endl;
        set_drag_zoom(true);
    }

    void process_input(int* key_input, float dt)
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

        circle.pos += glm::vec3(x, y, 0.f);
    }

    void render()
    {
        renderer& m_renderer = get_renderer();
        view_area m_view_area = m_renderer.get_view_area();
        obj view_area_obj(m_view_area.pos, m_view_area.size, glm::vec3(1.f));

        int obj_count = 0;
        for (obj& quad : quads)
        {
            if (view_area_obj.collide_with(quad))
            {
                draw_quad(quad);
                obj_count++;
            }
        }

        std::stringstream obj_count_str;
        obj_count_str << "Object count: " << obj_count;
        draw_text(obj_count_str.str(), glm::vec3(0.f), glm::vec3(25.f, 25.f, 0.f), 30.f);
        draw_text(obj_count_str.str(), glm::vec3(1.f), glm::vec3(20.f, 20.f, 0.f), 30.f);
    }
};

int main()
{
    app main_app(WIN_WIDTH, WIN_HEIGHT, "2dphobe_cpp");
    if (!main_app.run())
        return -1;
    return 0;
}