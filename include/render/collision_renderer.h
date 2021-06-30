#ifndef RENDER_COLLISION_RENDERER_H
#define RENDER_COLLISION_RENDERER_H

#include <vector>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "setup/sprite_config.h"
#include "game/components.h"
#include "game/collision.h"

// Stored information for sprite polygon
struct SpritePolygon {
    unsigned int vertices_offset;
    unsigned int vertex_count;
    unsigned int box_vertices_offset;
};

struct CollisionVertex {
    glm::vec<3, GLfloat> position;
    glm::vec<4, GLfloat> color; // with opacity
    CollisionVertex(): position(0, 0, 0), color(0, 0, 0, 0) {}
    CollisionVertex(glm::vec3 position, glm::vec4 color): position(position), color(color) {}
};


class CollisionRenderer {
public:
    CollisionRenderer() {}

    void load_spritesheets(const std::string &base_dir, const std::vector<SpritesheetConfig> spritesheets);
    void load_terrain_collision_data(const CollisionManager &collision_manager);
    void initialise(unsigned int program_id_1, unsigned int program_id_2);

    void enable_terrain(const glm::mat4 &view);
    void render_terrain();

    void enable_sprite_polygons(const glm::mat4 &view);
    void render_sprite_polygon(const component::Sprite &sprite);

private:
    // OpenGL data
    std::vector<CollisionVertex> static_vertices;
    std::vector<unsigned short> static_indices;
    unsigned int static_VAO, static_VBO, static_EBO;

    // For terrain collision shader (has no M transform)
    unsigned int program_id_1;
    unsigned int v_loc_1;

    // For terrain sprite polygon shader
    unsigned int program_id_2;
    unsigned int v_loc_2;
    unsigned int m_loc_2;

    unsigned int terrain_vertices_offset;
    unsigned int terrain_vertex_count;
    std::vector<SpritePolygon> sprite_polygons;
};

#endif
