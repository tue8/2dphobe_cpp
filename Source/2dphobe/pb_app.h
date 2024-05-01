#pragma once
#include "renderer.h"
#include "quad.h"

#define init_pb_app(a) a(unsigned int width, unsigned int height, const char *name) \
                            : pb_app(width, height, name) {}

typedef struct GLFWwindow GLFWwindow;

class pb_app
{
private:
    bool end_signal;
    bool created;
    unsigned int fps;
    float delta_time;
    int keys[1024];
    unsigned int width, height;
    double cursor_x, cursor_y;
    GLFWwindow *window;
    renderer m_renderer;
public:
    pb_app(unsigned int width, unsigned int height, const char *name);
    bool run();
    void end();
    void draw_quad(quad m_quad);
    bool load_texture(unsigned int &texture_id, const char *texture_dir) const;
    void debug_cam(float cam_speed);
    bool renderer_get_geometric_mode();
    void renderer_set_geometric_mode(bool g);
private:
    void m_draw_quad(quad m_quad);
    void g_draw_quad(quad g_quad);
public:
    virtual void init() {}
    virtual void process_input() {}
    virtual void update() {}
    virtual void render() {}
    virtual void cleanup() {}
};