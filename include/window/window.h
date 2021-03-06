#ifndef WINDOW_WINDOW_H
#define WINDOW_WINDOW_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input;

class Window {
public:
    Window(
        const std::string &name,
        unsigned int width,
        unsigned int height,
        bool fullscreen
    );
    bool initialise();
    bool is_running();
    void update();

    const std::string name;
    unsigned int width;
    unsigned int height;
    bool fullscreen;
    GLFWwindow *window;
};

#endif
