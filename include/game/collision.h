#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "setup/sprite_config.h"
#include "game/terrain.h"

/* In future, may be worth with using more memory-efficient structures
 * (ie: using contiguous memory), but for now a simple implementation is fine.
 *
 * One octree will be used for checking entity-terrain collisions.
 * The octree is initialised once with the terrain and doesn't change
 * until the terrain changes.
 *
 * Another octree will be used for checking entity-entity collisions, which
 * will need some more work, but can leave for now.
 *
 * For the terrain (static) octree, the tree can store terrain edges.
 * Make more sense than
 */

struct CollisionPolygon {
    std::vector<glm::vec2> vertices;
};

struct BoundingBox {
    double left, bot, right, top;
};

// Solid to the right of the edge, if going from first to second.
struct Edge {
    glm::vec2 a; // First
    glm::vec2 b; // Second
    Edge(): a(0, 0), b(0, 0) {}
};

// Used in the octree to store terrain edges
struct BoundedEdge {
    Edge edge; // For quick access to edge
    BoundingBox box;
    int polygon_index; // For accessing all vertices in polygon
    int vertex_index;
    void compute_box();
};

enum class Quadrant {
    TOP_RIGHT,
    TOP_LEFT,
    BOT_LEFT,
    BOT_RIGHT
};

struct Octree {
    glm::vec2 centre;
    BoundingBox box;
    std::vector<BoundedEdge> edges;
    Octree *nodes[4];
    Octree() { std::fill_n(nodes, 4, nullptr); }
    Octree(glm::vec2 parent_centre, const BoundingBox &parent_box, Quadrant quadrant);
};

// Individual edge intersections
enum class IntersectionType {
    ENTITY_1_ENTERING, // And entity 2 is leaving
    ENTITY_2_ENTERING // And entity 1 is leaving
};
struct Intersection {
    glm::vec2 pos;
    int polygon_1_index;
    int vertex_1_index;
    int polygon_2_index;
    int vertex_2_index;
    IntersectionType type;
};

// Pair of edge intersections indicate overlapping solid
struct Collision {
    glm::vec2 pos;
    glm::vec2 normal;
    double depth;
    bool slide; 
    // Two entities need to move away by a distance "depth" along normal.
    // If two intersections occur on the same edge, can slide perpendicular to normal.
};

// Forward declare components, since components.h includes this header
namespace component {
    class Transform;
    class Hitbox;
};

class CollisionManager {
public:
    void load_sprite_polygons(const std::vector<SpritesheetConfig> &spritesheets);
    void get_sprite_hitbox(SpriteId sprite_id, component::Hitbox &hitbox)const;

    void initialise_terrain(const Terrain &terrain);
    void check_terrain_entity(const component::Transform &transform, const component::Hitbox &hitbox, std::vector<Collision> &collisions);

private:
    void add_terrain_edge(const BoundedEdge &edge);
    void transform_sprite_polygon(const component::Transform &transform, const component::Hitbox &hitbox);

    glm::vec2 centre, size;

    Octree *root;

    std::vector<CollisionPolygon> polygons;
    std::vector<BoundingBox> sprite_boxes;

    friend class CollisionRenderer;
};

#endif
