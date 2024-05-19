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
    bool debug_cam_on;
    double fps;
    float delta_time;
    int key_input[1024];
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
    void set_debug_cam(bool dc);
    bool renderer_get_geometric_mode();
    void renderer_set_geometric_mode(bool g);
    int* get_key_input();
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
    virtual void update(float dt) {}
    virtual void process_input(int *key_input, float dt) {}
    virtual void render() {}
    virtual void cleanup() {}
};