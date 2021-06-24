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
        unsigned int height
    );
    bool initialise();
    bool is_running();
    void update();
private:
    const std::string name;
    unsigned int width;
    unsigned int height;
    GLFWwindow *window;

    friend class Input;
};

#endif
