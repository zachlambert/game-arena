#ifndef WINDOW_WINDOW_H
#define WINDOW_WINDOW_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window/keys.h"

class Window {
public:
    Window(
        const std::string &name,
        unsigned int width,
        unsigned int height
    );
    bool initialise();
    bool is_running();
    void update();

    void cursor_get_pos(double *mx, double *my);
    void cursor_set_pos(double mx, double my);

    bool key_is_pressed(Key key);

    unsigned int get_width() { return width; }
    unsigned int get_height() { return height; }
private:
    const std::string name;
    unsigned int width;
    unsigned int height;
    GLFWwindow *window;
};

#endif
