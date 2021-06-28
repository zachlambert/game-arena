#include "game/collision.h"
#include <stack>
#include <iostream>

#include "game/components.h"

BoundedEdge::BoundedEdge(const Edge &edge): edge(edge)
{
    if (edge.a.x < edge.b.x) {
        box.left = edge.a.x;
        box.right = edge.b.x;
    } else {
        box.left = edge.b.x;
        box.right = edge.a.x;
    }
    if (edge.a.y < edge.b.y) {
        box.bot = edge.a.y;
        box.top = edge.b.y;
    } else {
        box.bot = edge.b.y;
        box.top = edge.a.y;
    }
}

Octree::Octree(glm::vec2 parent_centre, const BoundingBox &parent_box, Quadrant quadrant) {
    std::fill_n(nodes, 4, nullptr);
    switch (quadrant) {
        case Quadrant::TOP_RIGHT:
            box.left = parent_centre.x;
            box.bot = parent_centre.y;
            box.right = parent_box.right;
            box.top = parent_box.top;
            break;
        case Quadrant::TOP_LEFT:
            box.right = parent_centre.x;
            box.bot = parent_centre.y;
            box.top = parent_box.top;
            box.left = parent_box.left;
            break;
        case Quadrant::BOT_LEFT:
            box.right = parent_centre.x;
            box.top = parent_centre.y;
            box.left = parent_box.left;
            box.bot = parent_box.bot;
            break;
        case Quadrant::BOT_RIGHT:
            box.left = parent_centre.x;
            box.top = parent_centre.y;
            box.right = parent_box.right;
            box.bot = parent_box.bot;
            break;
    }
    centre.x = 0.5*(box.left + box.right);
    centre.y = 0.5*(box.bot + box.top);
}

static void check_edge_edge(const Edge &edge1, const Edge &edge2, std::vector<Intersection> &intersections)
{
    glm::vec2 dif, perp, dif_a, dif_b;
    double dot_a, dot_b;

    // Check that the endpoints of edge2 are either side of the line of edge 1
    dif = edge1.b - edge1.a;
    perp.x = -dif.y;
    perp.y = dif.x;
    dif_a = edge2.a - edge1.a;
    dif_b = edge2.b - edge1.a;
    dot_a = glm::dot(perp, dif_a);
    dot_b = glm::dot(perp, dif_b);
    if (dot_a < 0 && dot_b < 0) return;
    if (dot_a > 0 && dot_b > 0) return;

    // Check that the endpoints of edge1 are either side of the line of edge 2
    dif = edge2.b - edge2.a;
    perp.x = -dif.y;
    perp.y = dif.x;
    dif_a = edge1.a - edge2.a;
    dif_b = edge1.b - edge2.a;
    dot_a = glm::dot(perp, dif_a);
    dot_b = glm::dot(perp, dif_b);
    if (dot_a < 0 && dot_b < 0) return;
    if (dot_a > 0 && dot_b > 0) return;

    // The edges intersect, so find the intersection of the two lines
    Intersection intersection;
    // using dif = edge2.b - edge2.a
    dif /= dif.length();
    perp /= perp.length();
    intersection.pos = edge2.a + dif * glm::dot(edge2.a - edge1.a, perp) / glm::dot(edge1.b - edge1.a, perp);
    intersections.push_back(intersection);
}

static bool check_box_box(const BoundingBox &box1, const BoundingBox &box2)
{
    if (box1.right < box2.left) return false;
    if (box1.left > box2.right) return false;
    if (box1.top < box2.bot) return false;
    if (box1.bot > box2.top) return false;
    return true;
}

void CollisionManager::add_terrain_edge(const BoundedEdge &edge)
{
    Octree *node = root;
    while (true) {
        if ((edge.box.right > node->centre.x && edge.box.left < node->centre.y)
            || (edge.box.top > node->centre.y && edge.box.bot < node->centre.y))
        {
            node->edges.push_back(edge);
            return;
        }
        Quadrant quadrant;
        if (edge.box.bot > node->centre.y) {
            if (edge.box.left > node->centre.x) {
                quadrant = Quadrant::TOP_RIGHT;
            } else {
                quadrant = Quadrant::TOP_LEFT;
            }
        } else {
            if (edge.box.left > node->centre.x) {
                quadrant = Quadrant::BOT_RIGHT;
            } else {
                quadrant = Quadrant::BOT_LEFT;
            }
        }
        if (node->nodes[(int)quadrant] == nullptr) {
            node->nodes[(int)quadrant] = new Octree(node->centre, node->box, quadrant);
        }
        node = node->nodes[(int)quadrant];
    }
}

void CollisionManager::initialise_terrain(const Terrain &terrain)
{
    centre = terrain.centre;
    size = terrain.size;

    // TODO: If allowing re-initialisation with new terrain, will need to delete previous octree

    root = new Octree();
    root->box.left = centre.x - size.x/2;
    root->box.right = centre.x + size.x/2;
    root->box.bot = centre.y - size.y/2;
    root->box.top = centre.y + size.y/2;
    root->centre = centre;

    Edge edge;
    for (const auto &element: terrain.elements) {
        for (int i = 0; i < element.vertices.size()-1; i++) {
            edge.a = element.vertices[i] + element.pos;
            edge.b = element.vertices[i+1] + element.pos;
            add_terrain_edge(BoundedEdge(edge));
        }
        edge.a = element.vertices.back() + element.pos;
        edge.b = element.vertices[0] + element.pos;
        add_terrain_edge(BoundedEdge(edge));
    }
}

void CollisionManager::load_sprite_polygons(const std::vector<SpritesheetConfig> &spritesheets)
{
    EdgeBlock edge_block;
    for (const auto &spritesheet: spritesheets) {
        for (const auto &sprite: spritesheet.sprites) {
            edge_block = load_polygon(sprite.collision_polygon);
            if ((int)sprite.id >= sprite_edge_blocks.size()) {
                sprite_edge_blocks.resize((int)sprite.id+1);
            }
            sprite_edge_blocks[(int)sprite.id] = edge_block;
        }
    }
}

EdgeBlock CollisionManager::load_polygon(const std::vector<glm::vec2> &vertices)
{
    EdgeBlock edge_block;
    edge_block.edges_start = entity_edges.size();
    edge_block.edges_count = vertices.size()-1;
    for (int i = 0; i < vertices.size()-1; i++) {
        entity_edges.push_back(Edge(vertices[i], vertices[i+1]));
    }
    entity_edges.push_back(Edge(vertices.back(), vertices[0]));
    edge_block.edges_count++;

    // Also find the bounding box, which bounds the mesh over all orientations
    double max_dist = hypot(vertices[0].x, vertices[1].x);
    double dist;
    for (int i = 1; i < vertices.size(); i++) {
        dist = hypot(vertices[i].x, vertices[i].y);
        if (dist > max_dist) max_dist = dist;
    }
    edge_block.original_box.right = max_dist;
    edge_block.original_box.top = max_dist;
    edge_block.original_box.bot = -max_dist;
    edge_block.original_box.left = -max_dist;

    return edge_block;
}

void CollisionManager::get_sprite_hitbox(SpriteId sprite_id, component::Hitbox &hitbox)const
{
    hitbox.edges_start = sprite_edge_blocks[(int)sprite_id].edges_start;
    hitbox.edges_count = sprite_edge_blocks[(int)sprite_id].edges_count;
    hitbox.original_box = sprite_edge_blocks[(int)sprite_id].original_box;
}

static void transform_point(const component::Transform &transform, const glm::vec2 &original, glm::vec2 &transformed)
{
    // Could use a matrix, but easier to just do manually
    transformed.x =
        transform.pos.x
        + transform.scale.x * original.x * cos(transform.orientation)
        - transform.scale.y * original.y * sin(transform.orientation);
    transformed.y =
        transform.pos.y
        + transform.scale.x * original.x * sin(transform.orientation)
        + transform.scale.y * original.y * cos(transform.orientation);
}

void CollisionManager::transform_entity_edges(const component::Transform &transform, const component::Hitbox &hitbox)
{
    for (unsigned int i = hitbox.edges_start; i < hitbox.edges_start + hitbox.edges_count; i++) {
        transform_point(transform, entity_edges[i].original.a, entity_edges[i].transformed.a);
        transform_point(transform, entity_edges[i].original.b, entity_edges[i].transformed.b);
    }
}

void CollisionManager::check_terrain_entity(
    const component::Transform &transform,
    const component::Hitbox &hitbox,
    std::vector<Intersection> &intersections)
{
    // Assumes that the bounding box has previously been updated, which should be done
    // by the collision system prior to collision detection.

    bool transformed_edges = false;

    std::stack<Octree*> nodes;
    nodes.push(root);
    while (!nodes.empty()) {
        // 1. Check edges at node
        Octree* node = nodes.top();
        nodes.pop();
        for (const auto &bounded_edge: node->edges) {
            // 1a. Check that bounding boxes collide
            if (!check_box_box(bounded_edge.box, hitbox.box)) continue;
            // 1b. If the bounding mesh hasn't been transformed yet, transform it
            if (!transformed_edges) {
                transform_entity_edges(transform, hitbox);
                transformed_edges = true;
            }
            // 1c. Check for intersections between all entity edges with the terrain edge
            for (unsigned int i = hitbox.edges_start; i < hitbox.edges_start + hitbox.edges_count; i++) {
                check_edge_edge(entity_edges[i].transformed, bounded_edge.edge, intersections);
            }
        }
        // 2. Check for intersection with sub-regions (nodes)
        for (int i = 0; i < 4; i++) {
            if (node->nodes[i] == nullptr) continue;
            if (!check_box_box(node->nodes[i]->box, hitbox.box)) continue;
            nodes.push(node->nodes[i]);
        }
    }
}
