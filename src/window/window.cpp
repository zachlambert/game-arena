#include "window/window.h"

#include <stdio.h>
#include <GLFW/glfw3.h>

Window::Window(const std::string &name, unsigned int width, unsigned int height):
    name(name), width(width), height(height), window(NULL) {}

bool Window::initialise()
{
    glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window); // Initialise GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialise GLEW.\n");
        return false;
    }

    // TODO: Provide configuration for this
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Disable vsync
    // glfwSwapInterval(0);

    return true;
}

bool Window::is_running()
{
    return !glfwWindowShouldClose(window);
}

void Window::update()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}
