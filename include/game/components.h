#ifndef GAME_COMPONENTS_H
#define GAME_COMPONENTS_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "game/collision.h"
#include "setup/sprite_id.h"

enum class ComponentType {
    TRANSFORM,
    SPRITE,
    POLYGON,
    PHYSICS,
    GUN,
    ENEMY_SPAWNER,
    HITBOX,
};

struct ComponentReference {
    ComponentType type;
    int index;
};

namespace component {

struct BaseComponent {
    // If a component is moved, need to update the index in the reference
    int ref_id;
};

struct Transform: public BaseComponent {
    glm::vec2 pos;
    double orientation;
    glm::vec2 scale;
    Transform(): pos(0, 0), orientation(0), scale(1, 1) {}
};

struct Sprite: public BaseComponent {
    glm::mat4 model;
    SpriteId sprite_id;
    unsigned int depth;
    Sprite(): sprite_id(SpriteId::NONE) {}
};

struct Polygon: public BaseComponent {
    glm::mat4 model;
    // In future, change this to be more efficient, but doing something simple for now
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec4> colors;
    std::vector<unsigned int> indices;
    unsigned int vertices_offset;
    unsigned int indices_offset;
    unsigned int element_count;
    bool allocated;
    bool dirty;
    unsigned int depth;
    Polygon(): allocated(false), dirty(true) {}
};

struct Physics: public BaseComponent {
    glm::vec3 twist;
    Physics(): twist(0, 0, 0) {}
};

struct Gun: public BaseComponent {
    unsigned int mesh_index_aiming;
    unsigned int mesh_index_fired;
    double origin_offset;
    double focus;
    double max_focus;
    double focus_rate;
    double base_damage;
    glm::vec2 fire_point;
    bool fire_event;
    bool fire_visual_on;
    double fire_visual_timer;
    double fire_visual_timeout;
    Gun():
        origin_offset(150), focus(0), max_focus(1), focus_rate(0.5), base_damage(10),
        fire_event(false), fire_visual_on(false), fire_visual_timer(0), fire_visual_timeout(0.04) {}
};

struct EnemySpawner: public BaseComponent {
    glm::vec2 pos;
    double spawn_timer;
    double spawn_timeout;
    EnemySpawner(): pos(0, 0), spawn_timer(0), spawn_timeout(1) {}
};

// For sprites only at the moment.
struct Hitbox: public BaseComponent {
    BoundingBox original_box;
    BoundingBox box; // Transformed
    unsigned int edges_start;
    unsigned int edges_count;
    SpriteId sprite_id;
    Hitbox(): sprite_id(SpriteId::NONE) {}
};

} // namespace component

#endif
