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
    dif /= hypot(dif.x, dif.y);
    perp /= hypot(perp.x, perp.y);
    intersection.pos =
        edge1.a - perp * glm::dot(edge1.a - edge2.a, perp)
        + dif * glm::dot(edge1.b - edge1.a, dif) * glm::dot(edge2.a - edge1.a, perp) / glm::dot(edge1.b - edge1.a, perp);

    intersection.entity_1_edge = &edge1;
    intersection.entity_2_edge = &edge2;

    if (glm::dot(edge1.b - edge1.a, perp) > 0) {
        intersection.type = IntersectionType::ENTITY_2_ENTERING;
    } else {
        intersection.type = IntersectionType::ENTITY_1_ENTERING;
    }

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

static void find_collisions(
    std::vector<Intersection> &intersections,
    std::vector<Collision> &collisions)
{
    if (intersections.size() == 0) return;

    // 1. Sort in-place by entity 1 edge index

    // Just doing bubble sort for now since it is simple.
    // Plus, there probably aren't that many intersections (< 10) so sorting
    // performance isn't critical

    for (int i = 0; i < (int)intersections.size()-1; i++) {
        for (int j = 0; j < (int)intersections.size()-1; j++) {
            if (intersections[j].entity_1_edge->index > intersections[j+1].entity_1_edge->index) {
                std::swap(intersections[j], intersections[j+1]);
            }
        }
    }

    // 2. Make sure the first intersection is ENTITY_1_ENTERING

    if (intersections[0].type != IntersectionType::ENTITY_1_ENTERING) {
        for (int i = intersections.size()-1; i > 0; i--) {
            std::swap(intersections[0], intersections[i]);
        }
    }

    // 3. Create collision for each pair
    
    std::cout << intersections.size() << std::endl;
    Collision collision;
    std::cout << intersections.size() << std::endl;
    glm::vec2 dir;
    for (int i = 0; i < intersections.size(); i+=2) {
        Intersection &inter1 = intersections[i];
        Intersection &inter2 = intersections[i+1];
        // Both intersections should have same vertices for the same entity
        const std::vector<glm::vec2> &vertices1 = *inter1.entity_1_edge->vertices;
        const std::vector<glm::vec2> &vertices2 = *inter1.entity_2_edge->vertices;

        collision.pos = 0.5f*(inter1.pos + inter2.pos);
        dir = inter2.pos - inter1.pos;
        // Normalise
        dir /= hypot(dir.x, dir.y);
        // Rotate 90 degrees
        collision.normal = {-dir.y, dir.x};

        if (inter1.entity_1_edge->index == inter2.entity_1_edge->index)
            collision.slide = true;
        if (inter1.entity_2_edge->index == inter2.entity_2_edge->index)
            collision.slide = true;

        // Now, hard bit is finding the intersection depth...
        // Let the indexes wrap around, but modulo when accessing element
        int i1, i2;
        i1 = inter1.entity_1_edge->vertex_index+1;
        i2 = inter2.entity_2_edge->vertex_index;
        double s1, s2, s_prev; // Lengths along collision surface
        double depth;
        double max_depth = INFINITY;
        std::cout << "i1 = " << i1 << " -> " << inter2.entity_1_edge->vertex_index+1 << std::endl;
        std::cout << "i2 = " << i2 << " -> " << inter1.entity_2_edge->vertex_index << std::endl;
        while (i1 < inter2.entity_1_edge->vertex_index+1 && i2 > inter1.entity_2_edge->vertex_index) {
            s1 = glm::dot(dir, vertices1[i1 % vertices1.size()]);
            s2 = glm::dot(dir, vertices2[i2 % vertices2.size()]);
            if (s1 > s2) {
                const glm::vec2 &v1a = vertices1[(i1-1) & vertices1.size()];
                const glm::vec2 &v1b = vertices1[i1 & vertices1.size()];
                const glm::vec2 &v2 = vertices2[i2 & vertices2.size()];
                s_prev = glm::dot(dir, v1a);
                depth = glm::dot(collision.normal, v1a - v2);
                depth += s2 / (s1 - s_prev) * glm::dot(collision.normal, v1b - v1a);
                i2--;
            } else {
                const glm::vec2 &v2a = vertices2[(i2+1) & vertices2.size()];
                const glm::vec2 &v2b = vertices2[i2 & vertices2.size()];
                const glm::vec2 &v1 = vertices1[i1 & vertices1.size()];
                s_prev = glm::dot(dir, v2a);
                depth = glm::dot(collision.normal, v2a - v1);
                depth += s1 / (s2 - s_prev) * glm::dot(collision.normal, v2b - v2a);
                i1++;
            }
            if (depth < max_depth) max_depth = depth;
        }

        collision.depth = max_depth;
        collisions.push_back(collision);
    }
}

void CollisionManager::add_terrain_edge(const BoundedEdge &edge)
{
    Octree *node = root;
    while (true) {
        if ((edge.box.right > node->centre.x && edge.box.left < node->centre.x)
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
    int edge_index = 0;
    for (const auto &element: terrain.elements) {
        // *** Assumes that terrain element remains in the same place in memory ***
        edge.vertices = &element.vertices;
        for (int i = 0; i < element.vertices.size()-1; i++) {
            edge.a = element.vertices[i] + element.pos;
            edge.b = element.vertices[i+1] + element.pos;
            edge.index = edge_index;
            edge_index++;
            edge.vertex_index = i;
            add_terrain_edge(BoundedEdge(edge));
        }
        edge.a = element.vertices.back() + element.pos;
        edge.b = element.vertices[0] + element.pos;
        edge.index = edge_index;
        edge_index++;
        edge.vertex_index = element.vertices.size() - 1;
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

    Edge edge;
    edge.vertices = &vertices;
    for (int i = 0; i < vertices.size()-1; i++) {
        edge.a = vertices[i];
        edge.b = vertices[i+1];
        edge.index = i;
        edge.vertex_index = i;
        entity_edges.push_back(edge);
    }
    edge.a = vertices.back();
    edge.b = vertices[0];
    edge.index = vertices.size()-1;
    edge.vertex_index = vertices.size()-1;
    entity_edges.push_back(edge);

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
        entity_edges[i].transformed.index = entity_edges[i].original.index;
        entity_edges[i].transformed.vertex_index = entity_edges[i].original.vertex_index;
        entity_edges[i].transformed.vertices = entity_edges[i].original.vertices;
    }
}

void CollisionManager::check_terrain_entity(
    const component::Transform &transform,
    const component::Hitbox &hitbox,
    std::vector<Collision> &collisions)
{
    // Assumes that the bounding box has previously been updated, which should be done
    // by the collision system prior to collision detection.

    bool transformed_edges = false;

    std::vector<Intersection> intersections;

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

    find_collisions(intersections, collisions);
}
