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
#include <windows.h>
#include "2dphobe/pb_app.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 800
#define QUAD_SIZE 100
#define QUAD_COUNT (WIN_WIDTH * WIN_HEIGHT) / (QUAD_SIZE * QUAD_SIZE)

std::string get_exe_path()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string f(buffer);
    return f.substr(0, f.find_last_of("\\/"));
}

class app : public pb_app
{
private:
    unsigned int box_texture;
    obj *quads;
    obj circle;
    obj tri;
public:
    init_pb_app(app);

    void init()
    {
        std::string path = get_exe_path();
        path = path.substr(0, path.length() - strlen("Binary"));
        path.append("Data\\Textures\\box.png");

        if (!load_texture(box_texture, path.c_str()))
            end();

        circle.size = glm::vec3(QUAD_SIZE, QUAD_SIZE, 0.f);
        circle.pos = glm::vec3(50.f, 50.f, 0.f);
        //circle.load_texture(box_texture);

        tri.size = glm::vec3(QUAD_SIZE * 2, QUAD_SIZE * 2, 0.f);
        tri.pos = glm::vec3(250.f, 50.f, 0.f);
        //tri.load_texture(box_texture);

        quads = new obj[QUAD_COUNT];

        int i = 0;
        for (float j = 0; j < WIN_WIDTH; j += QUAD_SIZE)
        {
            for (float k = 0; k < WIN_HEIGHT; k += QUAD_SIZE)
            {
                quads[i].size = glm::vec3(QUAD_SIZE, QUAD_SIZE, 0.f);
                quads[i].pos = glm::vec3(j, k, 0.f);
                //quads[i].load_texture(box_texture);
                i++;
            }
        }
        
    }

    void render()
    {
        renderer_set_geometric_mode(true);
        for (int i = 0; i < QUAD_COUNT; i++)
        {
            draw_quad(quads[i]);
        }
        draw_tri(tri);
        draw_circle(circle, 10);
    }

    void cleanup()
    {
        delete[] quads;
    }
};

int main()
{
    app main_app(WIN_WIDTH, WIN_HEIGHT, "2dphobe_cpp");
    if (!main_app.run())
        return -1;
    return 0;
}