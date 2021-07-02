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
    std::vector<Collision> &collisions)const
{
    if (intersections.size() <= 1) return;

    // Just doing bubble sort for now since it is simple.
    // Plus, there probably aren't that many intersections (< 10) so sorting
    // performance isn't critical

    for (int i = 0; i < (int)intersections.size()-1; i++) {
        for (int j = 0; j < (int)intersections.size()-1; j++) {
            if (intersections[j].polygon_1_index < intersections[j+1].polygon_1_index) {
                std::swap(intersections[j], intersections[j+1]);
                continue;
            }
            if (intersections[j].polygon_1_index > intersections[j+1].polygon_1_index) continue;

            if (intersections[j].polygon_2_index < intersections[j+1].polygon_2_index) {
                std::swap(intersections[j], intersections[j+1]);
                continue;
            }
            if (intersections[j].polygon_2_index > intersections[j+1].polygon_2_index) continue;

            if (intersections[j].vertex_1_index > intersections[j+1].vertex_1_index) {
                std::swap(intersections[j], intersections[j+1]);
                continue;
            }
            if (intersections[j].vertex_1_index < intersections[j+1].vertex_1_index) continue;

            int v2 = intersections[j].vertex_2_index;
            int v2_next = intersections[i+1].vertex_2_index;
            if (v2_next == 0) v2_next += polygons[intersections[i].polygon_2_index].vertices.size();
            if (v2_next - v2 == 1) {
                std::swap(intersections[j], intersections[j+1]);
            }
        }
    }

    Collision collision;
    glm::vec2 dir;
    for (int i = 0; i < intersections.size(); i+=2) {
        Intersection &inter1 = intersections[i].type == IntersectionType::ENTITY_1_ENTERING ? intersections[i] : intersections[i+1];
        Intersection &inter2 = intersections[i].type == IntersectionType::ENTITY_1_ENTERING ? intersections[i+1] : intersections[i];
        // assert(inter1.type != inter2.type);
        if (inter1.type == inter2.type) return;

        // Both intersections should have same polygons
        // assert(inter1.polygon_1_index == inter2.polygon_1_index);
        // assert(inter1.polygon_2_index == inter2.polygon_2_index);
        if (inter1.polygon_1_index != inter2.polygon_1_index) {
            std::cout << "p1 index different" << std::endl;
            std::cout << inter1.polygon_1_index << std::endl;
            std::cout << inter2.polygon_1_index << std::endl;
            std::cout << intersections.size() << std::endl;
            return;
        }
        if (inter1.polygon_2_index != inter2.polygon_2_index) {
            std::cout << "p2 index different" << std::endl;
            std::cout << inter1.polygon_2_index << std::endl;
            std::cout << inter2.polygon_2_index << std::endl;
            return;
        }
        const std::vector<glm::vec2> &vertices1 = polygons[inter1.polygon_1_index].vertices;
        const std::vector<glm::vec2> &vertices2 = polygons[inter1.polygon_2_index].vertices;

        collision.pos = 0.5f*(inter1.pos + inter2.pos);
        dir = inter2.pos - inter1.pos;
        if (hypot(dir.x, dir.y) < 1e-12) continue;
        // Normalise
        dir /= hypot(dir.x, dir.y);
        // Rotate 90 degrees
        collision.normal = {-dir.y, dir.x};

        if (inter1.vertex_1_index == inter2.vertex_1_index)
            collision.slide = true;
        if (inter1.vertex_2_index == inter2.vertex_2_index)
            collision.slide = true;

        // Now, hard bit is finding the intersection depth...

        int i1_start, i2_start;
        i1_start = (inter1.vertex_1_index+1)%vertices1.size();
        i2_start = inter1.vertex_2_index;
        int i1_end, i2_end;
        i1_end = (inter2.vertex_1_index+1)%vertices1.size();
        i2_end = inter2.vertex_2_index;

        double depth;
        double max_depth = 0;

        {
        int i1 = i1_start;
        int i2_1 = i2_start+1;
        if (i2_1 == vertices2.size()) i2_1 = 0;
        int i2_2 = i2_start;
        while (i1 != i1_end) {
            double s1, s2_1, s2_2;
            s1 = glm::dot(dir, vertices1[i1]);
            while (i2_1 != i2_end) {
                s2_1 = glm::dot(dir, vertices2[i2_1]);
                s2_2 = glm::dot(dir, vertices2[i2_2]);
                if (s2_1 < s1 && s2_2 > s1) break;
                i2_1--;
                i2_2--;
                if (i2_1 == -1) i2_1 = (int)vertices2.size() - 1;
                if (i2_2 == -1) i2_2 = (int)vertices2.size() - 1;
            }
            if (i2_1 == i2_end) break;
            const glm::vec2 &v1 = vertices1[i1];
            const glm::vec2 &v2_1 = vertices2[i2_1];
            const glm::vec2 &v2_2 = vertices2[i2_2];
            depth = glm::dot(collision.normal, v1 - v2_1);
            depth -= (s1-s2_1) / (s2_2 - s2_1) * glm::dot(collision.normal, v2_2 - v2_1);
            if (fabs(depth) > fabs(max_depth)) max_depth = depth;
            i1++;
            if (i1 == vertices1.size()) i1 = 0;
        }
        }

        {
        int i2 = i2_start;
        int i1_1 = i1_start-1;
        if (i1_1 == -1) i1_1 = (int)vertices1.size() - 1;
        int i1_2 = i1_start;
        while (i2 != i2_end) {
            double s2, s1_1, s1_2;
            s2 = glm::dot(dir, vertices2[i2]);
            while (i1_1 != i1_end) {
                s1_1 = glm::dot(dir, vertices1[i1_1]);
                s1_2 = glm::dot(dir, vertices1[i1_2]);
                if (s1_1 < s2 && s1_2 > s2) break;
                i1_1++;
                i1_2++;
                if (i1_1 == vertices1.size()) i1_1 = 0;
                if (i1_2 == vertices1.size()) i1_2 = 0;
            }
            if (i1_1 == i1_end) break;
            const glm::vec2 &v2 = vertices2[i2];
            const glm::vec2 &v1_1 = vertices1[i1_1];
            const glm::vec2 &v1_2 = vertices1[i1_2];
            depth = glm::dot(collision.normal, v2 - v1_1);
            depth -= (s2-s1_1) / (s1_2 - s1_1) * glm::dot(collision.normal, v1_2 - v1_1);
            depth = -depth;
            if (fabs(depth) > fabs(max_depth)) max_depth = depth;
            i2--;
            if (i2 == -1) i2 = (int)vertices2.size()-1;
        }
        }

        collision.depth = max_depth;
        collisions.push_back(collision);
    }
}

void CollisionManager::add_terrain_edge(const BoundedEdge &edge)
{
    Octree *node = root;
    while (true) {
        if ((edge.box.right >= node->centre.x && edge.box.left <= node->centre.x)
            || (edge.box.top >= node->centre.y && edge.box.bot <= node->centre.y))
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

void CollisionManager::transform_sprite_polygon(const component::Transform &transform, const component::Hitbox &hitbox)const
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

static bool edge_in_view(const Edge &edge, const glm::vec2 &centre, double angle_start, double angle_width)
{
    // Check if edge is facing towards the centre
    glm::vec2 perp = edge.b - edge.a;
    perp = {-perp.y, perp.x};
    if (glm::dot(edge.a - centre, perp) > 0) return false;

    // Check either a or b is in the angle window
    glm::vec2 disp;
    double angle;
    // Check if a is in view
    disp = edge.a - centre;
    angle = atan2(disp.y, disp.x);
    angle -= angle_start;
    if (angle < 0) angle += 2*M_PI;
    if (angle > 2*M_PI) angle -= 2*M_PI;
    if (angle < angle_width) return true;
    // Or if a is in view
    disp = edge.b - centre;
    angle = atan2(disp.y, disp.x);
    angle -= angle_start;
    if (angle < 0) angle += 2*M_PI;
    if (angle > 2*M_PI) angle -= 2*M_PI;
    if (angle < angle_width) return true;
    return false;
}

void CollisionManager::get_occlusion_edges(
    const BoundingBox &view_box,
    double angle_start, double angle_width,
    std::vector<const Edge*> &edges)const
{
    glm::vec2 centre(0.5*(view_box.left+view_box.right), 0.5*(view_box.bot+view_box.top));
    glm::vec2 disp1, disp2;
    double angle1, angle2;

    std::stack<Octree*> nodes;
    nodes.push(root);
    while (!nodes.empty()) {
        // 1. Check edges at node
        Octree* node = nodes.top();
        nodes.pop();
        for (const auto &bounded_edge: node->edges) {
            // 1. Check that the edge is in the view box
            if (!check_box_box(bounded_edge.box, view_box)) continue;
            // 2. Check that is within the viewing angle
            if (!edge_in_view(bounded_edge.edge, centre, angle_start, angle_width)) continue;
            edges.push_back(&bounded_edge.edge);
        }
        // 2. Check for intersection with sub-regions (nodes)
        for (int i = 0; i < 4; i++) {
            if (node->nodes[i] == nullptr) continue;
            if (!check_box_box(node->nodes[i]->box, view_box)) continue;
            nodes.push(node->nodes[i]);
        }
    }
}

static bool check_point_box(const glm::vec2 &source, const BoundingBox &box)
{
    if (source.x < box.left) return false;
    if (source.x > box.right) return false;
    if (source.y < box.bot) return false;
    if (source.y > box.top) return false;
    return true;
}

bool CollisionManager::check_entity_click(
    glm::vec2 origin, glm::vec2 point,
    const component::Transform &transform,
    const component::Hitbox &hitbox,
    double &distance)const
{
    transform_sprite_polygon(transform, hitbox);

    // Check line intersects polygon
    const std::vector<glm::vec2> &vertices = polygons[2*(int)hitbox.sprite_id+1].vertices;
    Edge edge;
    glm::vec2 dir = point - origin;
    glm::vec2 perp = {-dir.y, dir.x};
    dir /= hypot(dir.x, dir.y);
    double ua, ub, va, vb;
    bool intersected = false;
    double min_distance = INFINITY;
    double current_distance;
    for (int i = 0; i < vertices.size(); i++) {
        edge.a = vertices[i];
        edge.b = vertices[(i+1)%vertices.size()];
        va = glm::dot(perp, edge.a-origin);
        vb = glm::dot(perp, edge.b-origin);
        if ((va < 0 && vb < 0) || (va > 0 && vb > 0)) continue;
        ua = glm::dot(dir, edge.a-origin);
        ub = glm::dot(dir, edge.b-origin);
        current_distance = ua + (ub-ua)*(0-va)/(vb-va);
        if (current_distance < 0) continue;
        if (current_distance < min_distance) min_distance = current_distance;
        intersected = true;
    }
    if (!intersected) return false;
    distance = min_distance;

    // Check that line of sight is uninterrupted
    BoundedEdge sight;
    sight.edge.a = origin;
    sight.edge.b = point;
    sight.compute_box();

    Intersection intersection; // Unused, but needed for check_edge_edge function
    std::stack<Octree*> nodes;
    nodes.push(root);
    while (!nodes.empty()) {
        Octree* node = nodes.top();
        nodes.pop();
        for (const auto &bounded_edge: node->edges) {
            if (!check_box_box(bounded_edge.box, sight.box)) continue;
            if (check_edge_edge(bounded_edge.edge, sight.edge, intersection)) {
                return false;
            }
        }
        for (int i = 0; i < 4; i++) {
            if (node->nodes[i] == nullptr) continue;
            if (!check_box_box(node->nodes[i]->box, hitbox.box)) continue;
            nodes.push(node->nodes[i]);
        }
    }
    return true;
}
