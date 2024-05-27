#pragma once
#include "renderer.h"
#include "obj.h"
#include "text.h"
#include <string>

#define init_pb_app(a) a(unsigned int width, unsigned int height, const char *name) \
                            : pb_app(width, height, name) {}

typedef struct GLFWwindow GLFWwindow;

class pb_app
{
private:
    bool end_signal;
    bool created;
    bool b_drag_zoom;
    double fps;
    float delta_time;
    std::string app_name;
    unsigned int width, height;
    double cursor_x, cursor_y;
    GLFWwindow *window;
    renderer m_renderer;
    text text_handler;
public:
    pb_app(unsigned int width, unsigned int height, const char *name);
    bool run();
    void end();
    void draw_quad(obj &quad);
    void draw_tri(obj &tri);
    void draw_circle(obj &circle, float angle);
    void draw_text(std::string msg, glm::vec3 color, glm::vec3 pos, float scale);
    bool load_texture(unsigned int &texture_id, const char *texture_dir);
    void set_drag_zoom(bool dc);
    renderer& get_renderer();
    float get_fps();
private:
    void draw_char(obj& char_obj, texture_coords tc);
    void drag_zoom(float zoom_intensity);
public:
    virtual void init() {}
    virtual void update(float dt) {}
    virtual void process_input(int *key_input, float dt) {}
    virtual void render() {}
    virtual void cleanup() {}
};