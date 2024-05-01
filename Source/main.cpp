/*
* ==============================================
* ndtl28 (@Localtings),
* April, 2024
* 2dphobe engine remade in C++
* ==============================================
*/

#include <iostream>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "2dphobe/pb_app.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define QUAD_SIZE 100
#define QUAD_COUNT (WIN_WIDTH * WIN_HEIGHT) / (QUAD_SIZE * QUAD_SIZE)

class app : public pb_app
{
private:
    unsigned int quad_texture;
    quad *quads;
    quad g_quad;
public:
    init_pb_app(app);

    void init()
    {
        if (!load_texture(quad_texture, "\\Users\\admin\\Desktop\\2dphobe_cpp\\Data\\Textures\\box.png"))
            end();


        g_quad.size = glm::vec3(QUAD_SIZE, QUAD_SIZE, 0.f);
        g_quad.pos = glm::vec3(30.f, 30.f, 0.f);
        g_quad.load_texture(quad_texture);

        quads = new quad[QUAD_COUNT];

        int i = 0;
        for (float j = 0; j < WIN_WIDTH; j += QUAD_SIZE)
        {
            for (float k = 0; k < WIN_HEIGHT; k += QUAD_SIZE)
            {
                quads[i].size = glm::vec3(QUAD_SIZE, QUAD_SIZE, 0.f);
                quads[i].pos = glm::vec3(j, k, 0.f);
                quads[i].load_texture(quad_texture);
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
        renderer_set_geometric_mode(false);
        draw_quad(g_quad);
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