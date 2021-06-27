#include "setup/setup_renderer.h"
#include "render/sprite_renderer.h"
#include "render/mesh_renderer.h"

static void setup_sprite_renderer(SpriteRenderer &renderer, const TextureManager &texture_manager)
{
    SpriteConfig config;
    config.diffuse_texture = texture_manager.get_texture("spritesheet1_diffuse");
    config.offset = glm::vec2(0, 0);

    config.pos = glm::vec2(0, 0);
    config.size = glm::vec2(256, 256);
    renderer.load_sprite(config);

    config.pos = glm::vec2(256, 0);
    renderer.load_sprite(config);

    config.pos = glm::vec2(384, 0);
    renderer.load_sprite(config);

    config.pos = glm::vec2(0, 256);
    config.size = glm::vec2(512, 256);
    config.offset = glm::vec2(-128, 0);
    renderer.load_sprite(config);

    config.pos = glm::vec2(512, 256);
    config.size = glm::vec2(256, 256);
    config.offset = glm::vec2(0, 0);
    renderer.load_sprite(config);

    // renderer.initialise(shaders.sprite_program_id);
}

static void setup_mesh_renderer(MeshRenderer &renderer)
{
    // Remember: Define meshes clockwise !

    // Terrain element
    {
    std::vector<glm::vec2> vertices;
    glm::vec4 color;
    vertices.push_back(glm::vec2(-100, 0));
    vertices.push_back(glm::vec2(-80, 200));
    vertices.push_back(glm::vec2(20, 240));
    vertices.push_back(glm::vec2(0, -150));
    color = glm::vec4(0, 0, 0.1, 1);
    renderer.load_mesh(triangulate_mesh(vertices, color));
    }

    // Normalised gun ray (aiming)
    // x_scale = length
    // y_scale = tan(theta/2)*length
    {
    MeshConfig config;
    config.colors.push_back(glm::vec4(0.7, 0.7, 0.7, 1));
    config.colors.push_back(glm::vec4(0.7, 0.7, 0.7, -1)); // Negative opacity means it faded to 0 more quickly
    config.colors.push_back(glm::vec4(0.7, 0.7, 0.7, -1));
    config.vertices.push_back(glm::vec2(0, 0));
    config.vertices.push_back(glm::vec2(1, 1));
    config.vertices.push_back(glm::vec2(1, -1));
    config.indices.push_back(0);
    config.indices.push_back(1);
    config.indices.push_back(2);
    renderer.load_mesh(config);
    }

    // Normalised gun ray (fired)
    {
    MeshConfig config;
    config.colors.push_back(glm::vec4(0, 0, 0, 1)); // Darker
    config.colors.push_back(glm::vec4(0, 0, 0, -1));
    config.colors.push_back(glm::vec4(0, 0, 0, -1));
    config.vertices.push_back(glm::vec2(0, 0)); // Thinner
    config.vertices.push_back(glm::vec2(1, 0.8));
    config.vertices.push_back(glm::vec2(1, -0.8));
    config.indices.push_back(0);
    config.indices.push_back(1);
    config.indices.push_back(2);
    renderer.load_mesh(config);
    }

    // renderer.initialise(shaders.mesh_program_id);
}

void setup_renderer(Renderer &renderer, const TextureManager &texture_manager)
{
    setup_sprite_renderer(renderer.sprite_renderer, texture_manager);
    setup_mesh_renderer(renderer.mesh_renderer);
    // No setup for terrain renderer.
    // Load the terrain separately, as part of creating the world.
}
