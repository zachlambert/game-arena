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

    World world;

    Entity entity1;
    entity1.sprite_index = 0;
    entity1.depth = -1;
    entity1.orientation = 0.5;
    world.entities.push_back(entity1);

    Entity entity2;
    entity2.sprite_index = 3;
    entity2.pos.x = 200;
    entity2.pos.y = 100;
    entity2.orientation = -0.78;
    entity2.depth = -2;
    world.entities.push_back(entity2);

    while (window.is_running()) {
        renderer.render(world);
        window.update();
    }

    return 0;
}
