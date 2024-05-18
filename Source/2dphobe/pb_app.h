#pragma once
#include "renderer.h"
#include "obj.h"
#include <string>

#define init_pb_app(a) a(unsigned int width, unsigned int height, const char *name) \
                            : pb_app(width, height, name) {}

typedef struct GLFWwindow GLFWwindow;

class pb_app
{
private:
    bool end_signal;
    bool created;
    double fps;
    float delta_time;
    int keys[1024];
    std::string app_name;
    unsigned int width, height;
    double cursor_x, cursor_y;
    GLFWwindow *window;
    renderer m_renderer;
public:
    pb_app(unsigned int width, unsigned int height, const char *name);
    bool run();
    void end();
    void draw_quad(obj quad);
    void draw_tri(obj tri);
    void draw_circle(obj circle, float angle);
    bool load_texture(unsigned int &texture_id, const char *texture_dir) const;
    bool renderer_get_geometric_mode();
    void renderer_set_geometric_mode(bool g);
private:
    void m_draw_quad(obj quad);
    void g_draw_quad(obj quad);
    void m_draw_circle(obj circle, float angle);
    void g_draw_circle(obj circle, float angle);
    void m_draw_tri(obj tri);
    void g_draw_tri(obj tri);
    void debug_cam(float sens, float zoom_intensity, float dt);
public:
    virtual void init() {}
    virtual void process_input() {}
    virtual void update() {}
    virtual void render() {}
    virtual void cleanup() {}
};