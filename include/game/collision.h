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

struct BoundingBox {
    double left, bot, right, top;
};

// If defining the edge of a solid, the solid is to the right of the edge,
// from the first vertex "a" to second vertex "b".
struct Edge {
    glm::vec2 a;
    glm::vec2 b;
    int index;

    // Need to record some information to access vertices
    const std::vector<glm::vec2> *vertices;
    int vertex_index; // Index of a vertex. b will be the next index

    Edge(): a(0, 0), b(0, 0) {}
    Edge(glm::vec2 a, glm::vec2 b, int index): a(a), b(b), index(index) {}
};

struct BoundedEdge {
    Edge edge;
    BoundingBox box;
    BoundedEdge(const Edge &edge);
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

struct EdgeBlock {
    unsigned int edges_start;
    unsigned int edges_count;
    BoundingBox original_box;
};

struct EdgePair {
    const Edge original;
    Edge transformed;
    EdgePair(const Edge &edge): original(edge), transformed() {}
};

// Individual edge intersections
enum class IntersectionType {
    ENTITY_1_ENTERING, // And entity 2 is leaving
    ENTITY_2_ENTERING // And entity 1 is leaving
};
struct Intersection {
    glm::vec2 pos;
    const Edge *entity_1_edge;
    const Edge *entity_2_edge;
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
    EdgeBlock load_polygon(const std::vector<glm::vec2> &vertices);
    void add_terrain_edge(const BoundedEdge &edge);
    void transform_entity_edges(const component::Transform &transform, const component::Hitbox &hitbox);

    glm::vec2 centre, size;

    Octree *root;

    // Store collision meshes for entities as a listof edges.
    // Use EdgePairs such that each edge stores an original edge and a transformed
    // edge which is updated from the position and orientation of the current edge.
    std::vector<EdgePair> entity_edges;

    // Edge blocks for sprites, indexed by sprite_id
    std::vector<EdgeBlock>  sprite_edge_blocks;

    friend class CollisionRenderer;
};

#endif
