#include <stdlib.h>
#include <stdio.h>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "window/window.h"
#include "render/renderer.h"

int main()
{
    Window window("Arena", 1080, 720);
    if (!window.initialise()) {
        return 1;
    }

    Renderer renderer;
    renderer.initialise();

    while (window.is_running()) {
        renderer.render();
        window.update();
    }

    return 0;
}
