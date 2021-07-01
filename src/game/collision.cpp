#include "game/collision.h"
#include <stack>
#include <iostream>

#include "game/components.h"

void BoundedEdge::compute_box()
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

static bool check_edge_edge(const Edge &edge1, const Edge &edge2, Intersection &intersection)
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
    if (dot_a < 0 && dot_b < 0) return false;
    if (dot_a > 0 && dot_b > 0) return false;

    // Check that the endpoints of edge1 are either side of the line of edge 2
    dif = edge2.b - edge2.a;
    perp.x = -dif.y;
    perp.y = dif.x;
    dif_a = edge1.a - edge2.a;
    dif_b = edge1.b - edge2.a;
    dot_a = glm::dot(perp, dif_a);
    dot_b = glm::dot(perp, dif_b);
    if (dot_a < 0 && dot_b < 0) return false;
    if (dot_a > 0 && dot_b > 0) return false;

    // The edges intersect, so find the intersection of the two lines
    // using dif = edge2.b - edge2.a
    dif /= hypot(dif.x, dif.y);
    perp /= hypot(perp.x, perp.y);
    intersection.pos =
        edge1.a - perp * glm::dot(edge1.a - edge2.a, perp)
        + dif * glm::dot(edge1.b - edge1.a, dif) * glm::dot(edge2.a - edge1.a, perp) / glm::dot(edge1.b - edge1.a, perp);

    if (glm::dot(edge1.b - edge1.a, perp) > 0) {
        intersection.type = IntersectionType::ENTITY_2_ENTERING;
    } else {
        intersection.type = IntersectionType::ENTITY_1_ENTERING;
    }

    // Outside code responsible for fully defining intersection, with
    // polygon and vertex indices
    return true;
}

static bool check_box_box(const BoundingBox &box1, const BoundingBox &box2)
{
    if (box1.right < box2.left) return false;
    if (box1.left > box2.right) return false;
    if (box1.top < box2.bot) return false;
    if (box1.bot > box2.top) return false;
    return true;
}

void CollisionManager::find_collisions(
    std::vector<Intersection> &intersections,
    std::vector<Collision> &collisions)
{
    if (intersections.size() == 0) return;

    // 1. Sort in-place by vertex_1_index

    // Just doing bubble sort for now since it is simple.
    // Plus, there probably aren't that many intersections (< 10) so sorting
    // performance isn't critical

    for (int i = 0; i < (int)intersections.size()-1; i++) {
        for (int j = 0; j < (int)intersections.size()-1; j++) {
            if (intersections[j].vertex_1_index > intersections[j+1].vertex_1_index) {
                std::swap(intersections[j], intersections[j+1]);
                continue;
            }
            if (intersections[j].vertex_2_index < intersections[j+1].vertex_2_index) {
                std::swap(intersections[j], intersections[j+1]);
                continue;
            }
        }
    }

    // 2. Make sure the first intersection is ENTITY_1_ENTERING

    if (intersections[0].type != IntersectionType::ENTITY_1_ENTERING) {
        for (int i = intersections.size()-1; i > 0; i--) {
            std::swap(intersections[0], intersections[i]);
        }
    }
    assert(intersections[0].type == IntersectionType::ENTITY_1_ENTERING);

    // 3. Create collision for each pair

    Collision collision;
    glm::vec2 dir;
    for (int i = 0; i < intersections.size(); i+=2) {
        Intersection &inter1 = intersections[i];
        Intersection &inter2 = intersections[i+1];
        // Both intersections should have same polygons
        assert(inter1.polygon_1_index == inter2.polygon_1_index);
        assert(inter1.polygon_2_index == inter2.polygon_2_index);
        const std::vector<glm::vec2> &vertices1 = polygons[inter1.polygon_1_index].vertices;
        const std::vector<glm::vec2> &vertices2 = polygons[inter1.polygon_2_index].vertices;

        collision.pos = 0.5f*(inter1.pos + inter2.pos);
        dir = inter2.pos - inter1.pos;
        // Normalise
        dir /= hypot(dir.x, dir.y);
        // Rotate 90 degrees
        collision.normal = {-dir.y, dir.x};

        if (inter1.vertex_1_index == inter2.vertex_1_index)
            collision.slide = true;
        if (inter1.vertex_2_index == inter2.vertex_2_index)
            collision.slide = true;

        // Now, hard bit is finding the intersection depth...

        int i1, i2;
        i1 = (inter1.vertex_1_index+1)%vertices1.size();
        i2 = inter1.vertex_2_index;
        int i1_end, i2_end;
        i1_end = (inter2.vertex_1_index+1)%vertices1.size();
        i2_end = inter2.vertex_2_index;

        double s1, s2, s_prev; // Lengths along collision surface
        double depth;
        double max_depth = INFINITY;

        int j = 0;
        while (i1 != i1_end || i2 != i2_end) {
            s1 = glm::dot(dir, vertices1[i1]);
            s2 = glm::dot(dir, vertices2[i2]);
            if (s1 > s2) {
                const glm::vec2 &v1a = vertices1[(i1-1) & vertices1.size()];
                const glm::vec2 &v1b = vertices1[i1];
                const glm::vec2 &v2 = vertices2[i2];
                s_prev = glm::dot(dir, v1a);
                depth = glm::dot(collision.normal, v1a - v2);
                depth += (s2 / (s1 - s_prev)) * glm::dot(collision.normal, v1b - v1a);
                i2 = (i2-1)%vertices2.size();
            } else {
                const glm::vec2 &v2a = vertices2[(i2+1) & vertices2.size()];
                const glm::vec2 &v2b = vertices2[i2 & vertices2.size()];
                const glm::vec2 &v1 = vertices1[i1 & vertices1.size()];
                s_prev = glm::dot(dir, v2a);
                depth = glm::dot(collision.normal, v2a - v1);
                depth += s1 / (s2 - s_prev) * glm::dot(collision.normal, v2b - v2a);
                i1 = (i1+1)%vertices1.size();
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

    CollisionPolygon polygon;
    BoundedEdge bounded_edge;
    for (const auto &element: terrain.elements) {
        bounded_edge.polygon_index = polygons.size();
        polygon.vertices.resize(element.vertices.size());
        for (int i = 0; i < element.vertices.size(); i++) {
            bounded_edge.edge.a = element.vertices[i] + element.pos;
            bounded_edge.edge.b = element.vertices[(i+1)%element.vertices.size()] + element.pos;
            bounded_edge.vertex_index = i;
            bounded_edge.compute_box();
            add_terrain_edge(bounded_edge);
            polygon.vertices[i] = element.pos + element.vertices[i];
        }
        polygons.push_back(polygon);
    }
}

void CollisionManager::load_sprite_polygons(const std::vector<SpritesheetConfig> &spritesheets)
{
    CollisionPolygon polygon;
    for (const auto &spritesheet: spritesheets) {
        for (const auto &sprite: spritesheet.sprites) {
            polygon.vertices = sprite.collision_polygon;
            if (2*(int)sprite.id+1 >= polygons.size()) {
                polygons.resize(2*(int)sprite.id+2);
            }
            polygons[2*(int)sprite.id] = polygon; // Original.
            polygons[2*(int)sprite.id+1] = polygon; // Allowed to be transformed.

            BoundingBox box;
            double max_dist = hypot(polygon.vertices[0].x, polygon.vertices[0].y);
            double dist = INFINITY;
            for (int i = 1; i < polygon.vertices.size(); i++) {
                dist = hypot(polygon.vertices[i].x, polygon.vertices[i].y);
                if (dist > max_dist) max_dist = dist;
            }
            box.right = max_dist;
            box.top = max_dist;
            box.bot = -max_dist;
            box.left = -max_dist;

            if ((int)sprite.id >= sprite_boxes.size()) {
                sprite_boxes.resize((int)sprite.id + 1);
            }
            sprite_boxes[(int)sprite.id] = box;
        }
    }
}

void CollisionManager::get_sprite_hitbox(SpriteId sprite_id, component::Hitbox &hitbox)const
{
    hitbox.original_box = sprite_boxes[(int)sprite_id];
    hitbox.sprite_id = sprite_id;
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

void CollisionManager::transform_sprite_polygon(const component::Transform &transform, const component::Hitbox &hitbox)
{
    const std::vector<glm::vec2> &original = polygons[2*(int)hitbox.sprite_id].vertices;
    std::vector<glm::vec2> &transformed = polygons[2*(int)hitbox.sprite_id+1].vertices;
    for (int i = 0; i < original.size(); i++) {
        transform_point(transform, original[i], transformed[i]);
    }
}

void CollisionManager::check_terrain_entity(
    const component::Transform &transform,
    const component::Hitbox &hitbox,
    std::vector<Collision> &collisions)
{
    // Assumes that the bounding box has previously been updated, which should be done
    // by the collision system prior to collision detection.

    bool transformed_polygon = false;

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
            if (!transformed_polygon) {
                transform_sprite_polygon(transform, hitbox);
                transformed_polygon = true;
            }
            // 1c. Check for intersections between all entity edges with the terrain edge
            const std::vector<glm::vec2> &vertices = polygons[2*(int)hitbox.sprite_id+1].vertices;
            Intersection intersection;
            Edge sprite_edge;
            intersection.polygon_1_index = bounded_edge.polygon_index;
            intersection.vertex_1_index = bounded_edge.vertex_index;
            intersection.polygon_2_index = 2*(int)hitbox.sprite_id+1;
            for (int i = 0; i < vertices.size(); i++) {
                sprite_edge.a = vertices[i];
                sprite_edge.b = vertices[(i+1)%vertices.size()];
                if (!check_edge_edge(bounded_edge.edge, sprite_edge, intersection)) continue;
                intersection.vertex_2_index = i;
                intersections.push_back(intersection);
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
